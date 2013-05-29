/* Arduino FAT16 Library
 * Copyright (C) 2008 by William Greiman
 *
 * This file is part of the Arduino FAT16 Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with the Arduino Fat16 Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <avr/pgmspace.h>
#if ARDUINO < 100
#include <WProgram.h>
#else  // ARDUINO
#include <Arduino.h>
#endif  // ARDUINO
#include <Fat16.h>
//-----------------------------------------------------------------------------
// volume info
uint8_t  Fat16::volumeInitialized_ = 0;  // true if FAT16 volume is valid
uint8_t  Fat16::fatCount_;               // number of file allocation tables
uint8_t  Fat16::blocksPerCluster_;       // must be power of 2
uint16_t Fat16::rootDirEntryCount_;      // should be 512 for FAT16
fat_t    Fat16::blocksPerFat_;           // number of blocks in one FAT
fat_t    Fat16::clusterCount_;           // total clusters in volume
uint32_t Fat16::fatStartBlock_;          // start of first FAT
uint32_t Fat16::rootDirStartBlock_;      // start of root dir
uint32_t Fat16::dataStartBlock_;         // start of data clusters
//------------------------------------------------------------------------------
// raw block cache
SdCard *Fat16::rawDev_ = 0;             // class for block read and write
uint32_t Fat16::cacheBlockNumber_ = 0XFFFFFFFF;  // init to invalid block number
cache16_t  Fat16::cacheBuffer_;         // 512 byte cache for SdCard
uint8_t  Fat16::cacheDirty_ = 0;        // cacheFlush() will write block if true
uint32_t Fat16::cacheMirrorBlock_ = 0;  // mirror  block for second FAT
//------------------------------------------------------------------------------
// callback function for date/time
void (*Fat16::dateTime_)(uint16_t* date, uint16_t* time) = NULL;

#if ALLOW_DEPRECATED_FUNCTIONS
void (*Fat16::oldDateTime_)(uint16_t& date, uint16_t& time) = NULL;  // NOLINT
#endif  // ALLOW_DEPRECATED_FUNCTIONS
//------------------------------------------------------------------------------
// format 8.3 name for directory entry
static uint8_t make83Name(const char* str, uint8_t* name) {
  uint8_t c;
  uint8_t n = 7;  // max index for part before dot
  uint8_t i = 0;
  // blank fill name and extension
  while (i < 11) name[i++] = ' ';
  i = 0;
  while ((c = *str++) != '\0') {
    if (c == '.') {
      if (n == 10) return false;  // only one dot allowed
      n = 10;  // max index for full 8.3 name
      i = 8;   // place for extension
    } else {
      // illegal FAT characters
      PGM_P p = PSTR("|<>^+=?/[];,*\"\\");
      uint8_t b;
      while ((b = pgm_read_byte(p++))) if (b == c) return false;
      // check length and only allow ASCII printable characters
      if (i > n || c < 0X21 || c > 0X7E) return false;
      // only upper case allowed in 8.3 names - convert lower to upper
      name[i++] = c < 'a' || c > 'z' ?  c : c + ('A' - 'a');
    }
  }
  // must have a file name, extension is optional
  return name[0] != ' ';
}
//==============================================================================
// Fat16 member functions
//------------------------------------------------------------------------------
uint8_t Fat16::addCluster(void) {
  // start search after last cluster of file or at cluster two in FAT
  fat_t freeCluster = curCluster_ ? curCluster_ : 1;
  for (fat_t i = 0; ; i++) {
    // return no free clusters
    if (i >= clusterCount_) return false;
    // Fat has clusterCount + 2 entries
    if (freeCluster > clusterCount_) freeCluster = 1;
    freeCluster++;
    fat_t value;
    if (!fatGet(freeCluster, &value)) return false;
    if (value == 0) break;
  }
  // mark cluster allocated
  if (!fatPut(freeCluster, FAT16EOC)) return false;

  if (curCluster_ != 0) {
    // link cluster to chain
    if (!fatPut(curCluster_, freeCluster)) return false;
  } else {
    // first cluster of file so update directory entry
    flags_ |= F_FILE_DIR_DIRTY;
    firstCluster_ = freeCluster;
  }
  curCluster_ = freeCluster;
  return true;
}
//------------------------------------------------------------------------------
//
dir_t* Fat16::cacheDirEntry(uint16_t index, uint8_t action) {
  if (index >= rootDirEntryCount_) return NULL;
  if (!cacheRawBlock(rootDirStartBlock_ + (index >> 4), action)) return NULL;
  return &cacheBuffer_.dir[index & 0XF];
}
//------------------------------------------------------------------------------
//
uint8_t Fat16::cacheFlush(void) {
  if (cacheDirty_) {
    if (!rawDev_->writeBlock(cacheBlockNumber_, cacheBuffer_.data)) {
      return false;
    }
    // mirror FAT tables
    if (cacheMirrorBlock_) {
      if (!rawDev_->writeBlock(cacheMirrorBlock_, cacheBuffer_.data)) {
        return false;
      }
      cacheMirrorBlock_ = 0;
    }
    cacheDirty_ = 0;
  }
  return true;
}
//------------------------------------------------------------------------------
//
uint8_t Fat16::cacheRawBlock(uint32_t blockNumber, uint8_t action) {
  if (cacheBlockNumber_ != blockNumber) {
    if (!cacheFlush()) return false;
    if (!rawDev_->readBlock(blockNumber, cacheBuffer_.data)) return false;
    cacheBlockNumber_ = blockNumber;
  }
  cacheDirty_ |= action;
  return true;
}
//------------------------------------------------------------------------------
/**
 *  Close a file and force cached data and directory information
 *  to be written to the storage device.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include no file is open or an I/O error.
 */
uint8_t Fat16::close(void) {
  if (!sync()) return false;
  flags_ = 0;
  return true;
}
//------------------------------------------------------------------------------
/**
 * Return a files directory entry
 *
 * \param[out] dir Location for return of the files directory entry.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
uint8_t Fat16::dirEntry(dir_t* dir) {
  if (!sync()) return false;
  dir_t* p = cacheDirEntry(dirEntryIndex_, CACHE_FOR_WRITE);
  if (!p) return false;
  memcpy(dir, p, sizeof(dir_t));
  return true;
}
//------------------------------------------------------------------------------
uint8_t Fat16::fatGet(fat_t cluster, fat_t* value) {
  if (cluster > (clusterCount_ + 1)) return false;
  uint32_t lba = fatStartBlock_ + (cluster >> 8);
  if (lba != cacheBlockNumber_) {
    if (!cacheRawBlock(lba)) return false;
  }
  *value = cacheBuffer_.fat[cluster & 0XFF];
  return true;
}
//------------------------------------------------------------------------------
uint8_t Fat16::fatPut(fat_t cluster, fat_t value) {
  if (cluster < 2) return false;
  if (cluster > (clusterCount_ + 1)) return false;
  uint32_t lba = fatStartBlock_ + (cluster >> 8);
  if (lba != cacheBlockNumber_) {
    if (!cacheRawBlock(lba)) return false;
  }
  cacheBuffer_.fat[cluster & 0XFF] = value;
  cacheSetDirty();
  // mirror second FAT
  if (fatCount_ > 1) cacheMirrorBlock_ = lba + blocksPerFat_;
  return true;
}
//------------------------------------------------------------------------------
// free a cluster chain
uint8_t Fat16::freeChain(fat_t cluster) {
  while (1) {
    fat_t next;
    if (!fatGet(cluster, &next)) return false;
    if (!fatPut(cluster, 0)) return false;
    if (isEOC(next)) return true;
    cluster = next;
  }
}
//------------------------------------------------------------------------------
/**
 *  Initialize a FAT16 volume.
 *
 * \param[in] dev The SdCard where the volume is located.
 *
 * \param[in] part The partition to be used.  Legal values for \a part are
 * 1-4 to use the corresponding partition on a device formatted with
 * a MBR, Master Boot Record, or zero if the device is formatted as
 * a super floppy with the FAT boot sector in block zero.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.  reasons for
 * failure include not finding a valid FAT16 file system in the
 * specified partition, a call to init() after a volume has
 * been successful initialized or an I/O error.
 *
 */
uint8_t Fat16::init(SdCard* dev, uint8_t part) {
  // error if invalid partition
  if (part > 4) return false;
  rawDev_ = dev;
  uint32_t volumeStartBlock = 0;
  // if part == 0 assume super floppy with FAT16 boot sector in block zero
  // if part > 0 assume mbr volume with partition table
  if (part) {
    if (!cacheRawBlock(volumeStartBlock)) return false;
    volumeStartBlock = cacheBuffer_.mbr.part[part - 1].firstSector;
  }
  if (!cacheRawBlock(volumeStartBlock)) return false;
  // check boot block signature
  if (cacheBuffer_.data[510] != BOOTSIG0 ||
      cacheBuffer_.data[511] != BOOTSIG1) return false;
  bpb_t* bpb = &cacheBuffer_.fbs.bpb;
  fatCount_ = bpb->fatCount;
  blocksPerCluster_ = bpb->sectorsPerCluster;
  blocksPerFat_ = bpb->sectorsPerFat16;
  rootDirEntryCount_ = bpb->rootDirEntryCount;
  fatStartBlock_ = volumeStartBlock + bpb->reservedSectorCount;
  rootDirStartBlock_ = fatStartBlock_ + bpb->fatCount*bpb->sectorsPerFat16;
  dataStartBlock_ = rootDirStartBlock_
                    + ((32*bpb->rootDirEntryCount + 511)/512);
  uint32_t totalBlocks = bpb->totalSectors16 ?
                               bpb->totalSectors16 : bpb->totalSectors32;
  clusterCount_ = (totalBlocks - (dataStartBlock_ - volumeStartBlock))
                  /bpb->sectorsPerCluster;
  // verify valid FAT16 volume
  if (bpb->bytesPerSector != 512       // only allow 512 byte blocks
     || bpb->sectorsPerFat16 == 0      // zero for FAT32
     || clusterCount_ < 4085           // FAT12 if true
     || totalBlocks > 0X800000         // Max size for FAT16 volume
     || bpb->reservedSectorCount == 0  // invalid volume
     || bpb->fatCount == 0             // invalid volume
     || bpb->sectorsPerFat16 < (clusterCount_ >> 8)  // invalid volume
     || bpb->sectorsPerCluster == 0   // invalid volume
        // power of 2 test
     || bpb->sectorsPerCluster & (bpb->sectorsPerCluster - 1)) {
    // not a usable FAT16 bpb
    return false;
  }
  volumeInitialized_ = 1;
  return true;
}
//------------------------------------------------------------------------------
/** List directory contents to Serial.
 *
 * \param[in] flags The inclusive OR of
 *
 * LS_DATE - %Print file modification date
 *
 * LS_SIZE - %Print file size.
 */
void Fat16::ls(uint8_t flags) {
  dir_t d;
  for (uint16_t index = 0; readDir(&d, &index, DIR_ATT_VOLUME_ID); index++) {
    // print file name with possible blank fill
    printDirName(d, flags & (LS_DATE | LS_SIZE) ? 14 : 0);

    // print modify date/time if requested
    if (flags & LS_DATE) {
       printFatDate(d.lastWriteDate);
       Serial.write(' ');
       printFatTime(d.lastWriteTime);
    }

    // print size if requested
    if (DIR_IS_FILE(&d) && (flags & LS_SIZE)) {
      Serial.write(' ');
      Serial.print(d.fileSize);
    }
    Serial.println();
  }
}
//------------------------------------------------------------------------------
/**
 * Open a file by file name.
 *
 * \note The file must be in the root directory and must have a DOS
 * 8.3 name.
 *
 * \param[in] fileName A valid 8.3 DOS name for a file in the root directory.
 *
 * \param[in] oflag Values for \a oflag are constructed by a bitwise-inclusive
 *  OR of flags from the following list
 *
 * O_READ - Open for reading.
 *
 * O_RDONLY - Same as O_READ.
 *
 * O_WRITE - Open for writing.
 *
 * O_WRONLY - Same as O_WRITE.
 *
 * O_RDWR - Open for reading and writing.
 *
 * O_APPEND - If set, the file offset shall be set to the end of the
 * file prior to each write.
 *
 * O_CREAT - If the file exists, this flag has no effect except as noted
 * under O_EXCL below. Otherwise, the file shall be created
 *
 * O_EXCL - If O_CREAT and O_EXCL are set, open() shall fail if the file exists.
 *
 * O_SYNC - Call sync() after each write.  This flag should not be used with
 * write(uint8_t), write_P(PGM_P), writeln_P(PGM_P), or the Arduino Print class.
 * These functions do character a time writes so sync() will be called
 * after each byte.
 *
 * O_TRUNC - If the file exists and is a regular file, and the file is
 * successfully opened and is not read only, its length shall be truncated to 0.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include the FAT volume has not been initialized,
 * a file is already open, \a fileName is invalid, the file does not exist,
 * is a directory, or can't be opened in the access mode specified by oflag.
 */
uint8_t Fat16::open(const char* fileName, uint8_t oflag) {
  uint8_t dname[11];   // name formated for dir entry
  int16_t empty = -1;  // index of empty slot
  dir_t* p;            // pointer to cached dir entry

  if (!volumeInitialized_ || isOpen()) return false;

  // error if invalid name
  if (!make83Name(fileName, dname)) return false;

  for (uint16_t index = 0; index < rootDirEntryCount_; index++) {
    if (!(p = cacheDirEntry(index))) return false;
    if (p->name[0] == DIR_NAME_FREE || p->name[0] == DIR_NAME_DELETED) {
      // remember first empty slot
      if (empty < 0) empty = index;
      // done if no entries follow
      if (p->name[0] == DIR_NAME_FREE) break;
    } else if (!memcmp(dname, p->name, 11)) {
      // don't open existing file if O_CREAT and O_EXCL
      if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) return false;

      // open existing file
      return open(index, oflag);
    }
  }
  // error if directory is full
  if (empty < 0) return false;

  // only create file if O_CREAT and O_WRITE
  if ((oflag & (O_CREAT | O_WRITE)) != (O_CREAT | O_WRITE)) return false;

  if (!(p = cacheDirEntry(empty, CACHE_FOR_WRITE))) return false;

  // initialize as empty file
  memset(p, 0, sizeof(dir_t));
  memcpy(p->name, dname, 11);

  // set timestamps
  if (dateTime_) {
    // call user function
    dateTime_(&p->creationDate, &p->creationTime);
  } else {
    // use default date/time
    p->creationDate = FAT_DEFAULT_DATE;
    p->creationTime = FAT_DEFAULT_TIME;
  }
  p->lastAccessDate = p->creationDate;
  p->lastWriteDate = p->creationDate;
  p->lastWriteTime = p->creationTime;

  // insure created directory entry will be written to storage device
  if (!cacheFlush()) return false;

  // open entry
  return open(empty, oflag);
}
//------------------------------------------------------------------------------
/**
 * Open a file by file index.
 *
 * \param[in] index The root directory index of the file to be opened.  See \link
 *  Fat16::readDir() readDir()\endlink.
 *
 * \param[in] oflag  See \link Fat16::open(const char*, uint8_t)\endlink.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include the FAT volume has not been initialized,
 * a file is already open, \a index is invalid or is not the index of a
 * file or the file cannot be opened in the access mode specified by oflag.
 */
uint8_t Fat16::open(uint16_t index, uint8_t oflag) {
  if (!volumeInitialized_ || isOpen()) return false;
  if ((oflag & O_TRUNC) && !(oflag & O_WRITE)) return false;
  dir_t* d = cacheDirEntry(index);
  // if bad file index or I/O error
  if (!d) return false;

  // error if unused entry
  if (d->name[0] == DIR_NAME_FREE || d->name[0] == DIR_NAME_DELETED) {
    return false;
  }
  // error if long name, volume label or subdirectory
  if ((d->attributes & (DIR_ATT_VOLUME_ID | DIR_ATT_DIRECTORY)) != 0) {
    return false;
  }
  // don't allow write or truncate if read-only
  if (d->attributes & DIR_ATT_READ_ONLY) {
    if (oflag & (O_WRITE | O_TRUNC)) return false;
  }

  curCluster_ = 0;
  curPosition_ = 0;
  dirEntryIndex_ = index;
  fileSize_ = d->fileSize;
  firstCluster_ = d->firstClusterLow;
  flags_ = oflag & (O_ACCMODE | O_SYNC | O_APPEND);

  if (oflag & O_TRUNC ) return truncate(0);
  return true;
}
//------------------------------------------------------------------------------
/** %Print the name field of a directory entry in 8.3 format to Serial.
 *
 * \param[in] dir The directory structure containing the name.
 * \param[in] width Blank fill name if length is less than \a width.
 */
void Fat16::printDirName(const dir_t& dir, uint8_t width) {
  uint8_t w = 0;
  for (uint8_t i = 0; i < 11; i++) {
    if (dir.name[i] == ' ') continue;
    if (i == 8) {
      Serial.write('.');
      w++;
    }
    Serial.write(dir.name[i]);
    w++;
  }
  if (DIR_IS_SUBDIR(&dir)) {
    Serial.write('/');
    w++;
  }
  while (w < width) {
    Serial.write(' ');
    w++;
  }
}
//------------------------------------------------------------------------------
/** %Print a directory date field to Serial.
 *
 *  Format is yyyy-mm-dd.
 *
 * \param[in] fatDate The date field from a directory entry.
 */
void Fat16::printFatDate(uint16_t fatDate) {
  Serial.print(FAT_YEAR(fatDate));
  Serial.write('-');
  printTwoDigits(FAT_MONTH(fatDate));
  Serial.write('-');
  printTwoDigits(FAT_DAY(fatDate));
}
//------------------------------------------------------------------------------
/** %Print a directory time field to Serial.
 *
 * Format is hh:mm:ss.
 *
 * \param[in] fatTime The time field from a directory entry.
 */
void Fat16::printFatTime(uint16_t fatTime) {
  printTwoDigits(FAT_HOUR(fatTime));
  Serial.write(':');
  printTwoDigits(FAT_MINUTE(fatTime));
  Serial.write(':');
  printTwoDigits(FAT_SECOND(fatTime));
}

//------------------------------------------------------------------------------
/** %Print a value as two digits to Serial.
 *
 * \param[in] v Value to be printed, 0 <= \a v <= 99
 */
void Fat16::printTwoDigits(uint8_t v) {
  char str[3];
  str[0] = '0' + v/10;
  str[1] = '0' + v % 10;
  str[2] = 0;
  Serial.print(str);
}
//------------------------------------------------------------------------------
/**
 * Read the next byte from a file.
 *
 * \return For success read returns the next byte in the file as an int.
 * If an error occurs or end of file is reached -1 is returned.
 */
int16_t Fat16::read(void) {
  uint8_t b;
  return read(&b, 1) == 1 ? b : -1;
}
//------------------------------------------------------------------------------
/**
 * Read data from a file at starting at the current file position.
 *
 * \param[out] buf Pointer to the location that will receive the data.
 *
 * \param[in] nbyte Maximum number of bytes to read.
 *
 * \return For success read returns the number of bytes read.
 * A value less than \a nbyte, including zero, may be returned
 * if end of file is reached.
 * If an error occurs, read returns -1.  Possible errors include
 * read called before a file has been opened, the file has not been opened in
 * read mode, a corrupt file system, or an I/O error.
 */
int16_t Fat16::read(void* buf, uint16_t nbyte) {
  // convert void pointer to uin8_t pointer
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);

  // error if not open for read
  if (!(flags_ & O_READ)) return -1;

  // don't read beyond end of file
  if ((curPosition_ + nbyte) > fileSize_) nbyte = fileSize_ - curPosition_;

  // bytes left to read in loop
  uint16_t nToRead = nbyte;
  while (nToRead > 0) {
    uint8_t blkOfCluster = blockOfCluster(curPosition_);
    uint16_t blockOffset = cacheDataOffset(curPosition_);
    if (blkOfCluster == 0 && blockOffset == 0) {
      // start next cluster
      if (curCluster_ == 0) {
        curCluster_ = firstCluster_;
      } else {
        if (!fatGet(curCluster_, &curCluster_)) return -1;
      }
      // return error if bad cluster chain
      if (curCluster_ < 2 || isEOC(curCluster_)) return -1;
    }
    // cache data block
    if (!cacheRawBlock(dataBlockLba(curCluster_, blkOfCluster))) return -1;

    // location of data in cache
    uint8_t* src = cacheBuffer_.data + blockOffset;

    // max number of byte available in block
    uint16_t n = 512 - blockOffset;

    // lesser of available and amount to read
    if (n > nToRead) n = nToRead;

    // copy data to caller
    memcpy(dst, src, n);

    curPosition_ += n;
    dst += n;
    nToRead -= n;
  }
  return nbyte;
}
//------------------------------------------------------------------------------
/**
 *  Read the next short, 8.3, directory entry.
 *
 *  Unused entries and entries for long names are skipped.
 *
 * \param[out] dir Location that will receive the entry.
 *
 * \param[in,out] index The search starts at \a index and \a index is
 * updated with the root directory index of the found directory entry.
 * If the entry is a file, it may be opened by calling
 * \link Fat16::open(uint16_t, uint8_t) \endlink.
 *
 * \param[in] skip Skip entries that have these attributes. If \a skip
 * is not specified, the default is to skip the volume label and directories.
 *
 * \return The value one, true, is returned for success and the value zero,
 * false, is returned if an error occurs or the end of the root directory is
 * reached.  On success, \a entry is set to the index of the found directory
 * entry.
 */
uint8_t Fat16::readDir(dir_t* dir, uint16_t* index, uint8_t skip) {
  dir_t* p;
  for (uint16_t i = *index; ; i++) {
    if (i >= rootDirEntryCount_) return false;
    if (!(p = cacheDirEntry(i))) return false;

    // done if beyond last used entry
    if (p->name[0] == DIR_NAME_FREE) return false;

    // skip deleted entry
    if (p->name[0] == DIR_NAME_DELETED) continue;

    // skip long names
    if ((p->attributes & DIR_ATT_LONG_NAME_MASK) == DIR_ATT_LONG_NAME) continue;

    // skip if attribute match
    if (p->attributes & skip) continue;

    // return found index
    *index = i;
    break;
  }
  memcpy(dir, p, sizeof(dir_t));
  return true;
}
//------------------------------------------------------------------------------
/**
 * Remove a file.  The directory entry and all data for the file are deleted.
 *
 * \note This function should not be used to delete the 8.3 version of a
 * file that has a long name. For example if a file has the long name
 * "New Text Document.txt" you should not delete the 8.3 name "NEWTEX~1.TXT".
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include the file is not open for write
 * or an I/O error occurred.
 */
uint8_t Fat16::remove(void) {
  // error if file is not open for write
  if (!(flags_ & O_WRITE)) return false;
  if (firstCluster_) {
    if (!freeChain(firstCluster_)) return false;
  }
  dir_t* d = cacheDirEntry(dirEntryIndex_, CACHE_FOR_WRITE);
  if (!d) return false;
  d->name[0] = DIR_NAME_DELETED;
  flags_ = 0;
  return cacheFlush();
}
//------------------------------------------------------------------------------
/**
 * Remove a file.
 *
 * The directory entry and all data for the file are deleted.
 *
 * \param[in] fileName The name of the file to be removed.
 *
 * \note This function should not be used to delete the 8.3 version of a
 * file that has a long name. For example if a file has the long name
 * "New Text Document.txt" you should not delete the 8.3 name "NEWTEX~1.TXT".
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include the file is read only, \a fileName is not found
 * or an I/O error occurred.
 */
uint8_t Fat16::remove(const char* fileName) {
  Fat16 file;
  if (!file.open(fileName, O_WRITE)) return false;
  return file.remove();
}
//------------------------------------------------------------------------------
/**
 * Sets the file's read/write position.
 *
 * \param[in] pos The new position in bytes from the beginning of the file.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
uint8_t Fat16::seekSet(uint32_t pos) {
  // error if file not open or seek past end of file
  if (!isOpen() || pos > fileSize_) return false;
  if (pos == 0) {
    // set position to start of file
    curCluster_ = 0;
    curPosition_ = 0;
    return true;
  }
  fat_t n = ((pos - 1) >> 9)/blocksPerCluster_;
  if (pos < curPosition_ || curPosition_ == 0) {
    // must follow chain from first cluster
    curCluster_ = firstCluster_;
  } else {
    // advance from curPosition
    n -= ((curPosition_ - 1) >> 9)/blocksPerCluster_;
  }
  while (n--) {
    if (!fatGet(curCluster_, &curCluster_)) return false;
  }
  curPosition_ = pos;
  return true;
}
//------------------------------------------------------------------------------
/**
 *  The sync() call causes all modified data and directory fields
 *  to be written to the storage device.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include a call to sync() before a file has been
 * opened or an I/O error.
 */
uint8_t Fat16::sync(void) {
  if (flags_ & F_FILE_DIR_DIRTY) {
    // cache directory entry
    dir_t* d = cacheDirEntry(dirEntryIndex_, CACHE_FOR_WRITE);
    if (!d) return false;

    // update file size and first cluster
    d->fileSize = fileSize_;
    d->firstClusterLow = firstCluster_;

    // set modify time if user supplied a callback date/time function
    if (dateTime_) {
      dateTime_(&d->lastWriteDate, &d->lastWriteTime);
      d->lastAccessDate = d->lastWriteDate;
    }
    flags_ &= ~F_FILE_DIR_DIRTY;
  }
  return cacheFlush();
}
//------------------------------------------------------------------------------
/**
 * The timestamp() call sets a file's timestamps in its directory entry.
 *
 * \param[in] flags Values for \a flags are constructed by a bitwise-inclusive
 * OR of flags from the following list
 *
 * T_ACCESS - Set the file's last access date.
 *
 * T_CREATE - Set the file's creation date and time.
 *
 * T_WRITE - Set the file's last write/modification date and time.
 *
 * \param[in] year Valid range 1980 - 2107 inclusive.
 *
 * \param[in] month Valid range 1 - 12 inclusive.
 *
 * \param[in] day Valid range 1 - 31 inclusive.
 *
 * \param[in] hour Valid range 0 - 23 inclusive.
 *
 * \param[in] minute Valid range 0 - 59 inclusive.
 *
 * \param[in] second Valid range 0 - 59 inclusive
 *
 * \note It is possible to set an invalid date since there is no check for
 * the number of days in a month.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
uint8_t Fat16::timestamp(uint8_t flags, uint16_t year, uint8_t month,
         uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
  if (!isOpen()
    || year < 1980
    || year > 2107
    || month < 1
    || month > 12
    || day < 1
    || day > 31
    || hour > 23
    || minute > 59
    || second > 59) {
      return false;
  }
  dir_t* d = cacheDirEntry(dirEntryIndex_, CACHE_FOR_WRITE);
  if (!d) return false;
  uint16_t dirDate = FAT_DATE(year, month, day);
  uint16_t dirTime = FAT_TIME(hour, minute, second);
  if (flags & T_ACCESS) {
    d->lastAccessDate = dirDate;
  }
  if (flags & T_CREATE) {
    d->creationDate = dirDate;
    d->creationTime = dirTime;
    // seems to be units of 1/100 second not 1/10 as Microsoft standard states
    d->creationTimeTenths = second & 1 ? 100 : 0;
  }
  if (flags & T_WRITE) {
    d->lastWriteDate = dirDate;
    d->lastWriteTime = dirTime;
  }
  cacheSetDirty();
  return sync();
}
//------------------------------------------------------------------------------
/**
 * Truncate a file to a specified length.  The current file position
 * will be maintained if it is less than or equal to \a length otherwise
 * it will be set to end of file.
 *
 * \param[in] length The desired length for the file.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include file is read only, file is a directory,
 * \a length is greater than the current file size or an I/O error occurs.
 */
uint8_t Fat16::truncate(uint32_t length) {
  // error if file is not open for write
  if (!(flags_ & O_WRITE)) return false;

  if (length > fileSize_) return false;

  // fileSize and length are zero - nothing to do
  if (fileSize_ == 0) return true;
  uint32_t newPos = curPosition_ > length ? length : curPosition_;
  if (length == 0) {
    // free all clusters
    if (!freeChain(firstCluster_)) return false;
    curCluster_ = firstCluster_ = 0;
  } else {
    fat_t toFree;
    if (!seekSet(length)) return false;
    if (!fatGet(curCluster_, &toFree)) return false;
    if (!isEOC(toFree)) {
      // free extra clusters
      if (!fatPut(curCluster_, FAT16EOC)) return false;
      if (!freeChain(toFree)) return false;
    }
  }
  fileSize_ = length;
  flags_ |= F_FILE_DIR_DIRTY;
  if (!sync()) return false;
  return seekSet(newPos);
}
//------------------------------------------------------------------------------
/**
 * Write data at the current position of an open file.
 *
 * \note Data is moved to the cache but may not be written to the
 * storage device until sync() is called.
 *
 * \param[in] buf Pointer to the location of the data to be written.
 *
 * \param[in] nbyte Number of bytes to write.
 *
 * \return For success write() returns the number of bytes written, always
 * \a nbyte.  If an error occurs, write() returns -1.  Possible errors include
 * write() is called before a file has been opened, the file has not been opened
 * for write, device is full, a corrupt file system or an I/O error.
 *
 */
int16_t Fat16::write(const void* buf, uint16_t nbyte) {
  uint16_t nToWrite = nbyte;
  const uint8_t* src = reinterpret_cast<const uint8_t*>(buf);

  // error if file is not open for write
  if (!(flags_ & O_WRITE)) goto writeErrorReturn;

  // go to end of file if O_APPEND
  if ((flags_ & O_APPEND) && curPosition_ != fileSize_) {
    if (!seekEnd()) goto writeErrorReturn;
  }
  while (nToWrite > 0) {
    uint8_t blkOfCluster = blockOfCluster(curPosition_);
    uint16_t blockOffset = cacheDataOffset(curPosition_);
    if (blkOfCluster == 0 && blockOffset == 0) {
      // start of new cluster
      if (curCluster_ == 0) {
        if (firstCluster_ == 0) {
          // allocate first cluster of file
          if (!addCluster()) goto writeErrorReturn;
        } else {
          curCluster_ = firstCluster_;
        }
      } else {
        fat_t next;
        if (!fatGet(curCluster_, &next)) goto writeErrorReturn;
        if (isEOC(next)) {
          // add cluster if at end of chain
          if (!addCluster()) goto writeErrorReturn;
        } else {
          curCluster_ = next;
        }
      }
    }
    uint32_t lba = dataBlockLba(curCluster_, blkOfCluster);
    if (blockOffset == 0 && curPosition_ >= fileSize_) {
      // start of new block don't need to read into cache
      if (!cacheFlush()) goto writeErrorReturn;
      cacheBlockNumber_ = lba;
      cacheSetDirty();
    } else {
      // rewrite part of block
      if (!cacheRawBlock(lba, CACHE_FOR_WRITE)) return -1;
    }
    uint8_t* dst = cacheBuffer_.data + blockOffset;

    // max space in block
    uint16_t n = 512 - blockOffset;

    // lesser of space and amount to write
    if (n > nToWrite) n = nToWrite;

    // copy data to cache
    memcpy(dst, src, n);

    curPosition_ += n;
    nToWrite -= n;
    src += n;
  }
  if (curPosition_ > fileSize_) {
    // update fileSize and insure sync will update dir entry
    fileSize_ = curPosition_;
    flags_ |= F_FILE_DIR_DIRTY;
  } else if (dateTime_ && nbyte) {
    // insure sync will update modified date and time
    flags_ |= F_FILE_DIR_DIRTY;
  }

  if (flags_ & O_SYNC) {
    if (!sync()) goto writeErrorReturn;
  }
  return nbyte;

 writeErrorReturn:
  writeError = true;
  return -1;
}
//------------------------------------------------------------------------------
/**
 * Write a byte to a file. Required by the Arduino Print class.
 *
 *  Use Fat16::writeError to check for errors.
 */
#if ARDUINO < 100
void Fat16::write(uint8_t b) {
  write(&b, 1);
}
#else  // ARDUINO < 100
size_t Fat16::write(uint8_t b) {
  return write(&b, 1) == 1 ? 1 : 0;
}
#endif  // ARDUINO < 100
//------------------------------------------------------------------------------
/**
 * Write a string to a file. Used by the Arduino Print class.
 *
 * Use Fat16::writeError to check for errors.
 */
#if ARDUINO < 100
void Fat16::write(const char* str) {
  write(str, strlen(str));
}
#else  // ARDUINO < 100
int16_t Fat16::write(const char* str) {
  return write(str, strlen(str));
}
#endif  // ARDUINO < 100
//------------------------------------------------------------------------------
/**
 * Write a PROGMEM string to a file.
 *
 * Use Fat16::writeError to check for errors.
 */
void Fat16::write_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++) write(c);
}
//------------------------------------------------------------------------------
/**
 * Write a PROGMEM string followed by CR/LF to a file.
 *
 * Use Fat16::writeError to check for errors.
 */
void Fat16::writeln_P(PGM_P str) {
  write_P(str);
  println();
}
