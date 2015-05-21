/* 
  FileSystem.h - SPIFS implementation for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _SPIFFS_CORE_FILESYSTEM_H_
#define _SPIFFS_CORE_FILESYSTEM_H_

#include <memory>
#include "spiffs/spiffs.h"
#include "Arduino.h"

#define FSFILE_READ SPIFFS_RDONLY
#define FSFILE_WRITE (SPIFFS_RDONLY | SPIFFS_WRONLY | SPIFFS_CREAT | SPIFFS_APPEND )
#define FSFILE_OVERWRITE (SPIFFS_RDONLY | SPIFFS_WRONLY | SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_TRUNC )

class FSFile : public Stream {
private:
  spiffs_stat _stats; 
  file_t _file;
  spiffs_DIR _dir;
  spiffs * _fs;

public:
  FSFile(spiffs* fs, String path);
  FSFile(spiffs* fs, file_t f);
  FSFile(void);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int read();
  virtual int peek();
  virtual int available();
  virtual void flush();
  int read(void *buf, uint16_t nbyte);
  uint32_t seek(uint32_t pos);
  uint32_t position();
  uint32_t size();
  bool eof();
  void close();
  bool remove();
  int lastError();
  void clearError();
  operator bool() { return _file > 0; }
  char * name();
  bool isDirectory(void);
  void rewindDirectory(void);
  FSFile openNextFile(void);

  template<typename T> size_t write(T &src){
    const size_t bufferSize = 64;
    uint8_t obuf[bufferSize];
    size_t bytesWritten = 0;
    while (true){
      size_t available = src.available();
      if(!available)
        return bytesWritten;
      size_t willWrite = (available < bufferSize) ? available : bufferSize;
      src.read(obuf, willWrite);
      size_t cb = write(obuf, willWrite);
      if (cb != willWrite) {
        return bytesWritten;
      }
      bytesWritten += cb;
    }
    return bytesWritten;
  }
  
  using Print::write;
};

class FSClass {
public:
  FSClass(uint32_t beginAddress, uint32_t endAddress, uint32_t pageSize, uint32_t blockSize, uint32_t maxOpenFiles);

  bool mount();
  void unmount();
  bool format();
  bool check();
  bool exists(char *filename);
  bool create(char *filepath);
  bool remove(char *filepath);
  bool rename(char *filename, char *newname);
  size_t totalBytes();
  size_t usedBytes();
  size_t size(){ return _fs.cfg.phys_size; }
  size_t blockSize(){ return _fs.cfg.log_block_size; }
  size_t totalBlocks(){ return _fs.block_count; }
  size_t freeBlocks(){ return _fs.free_blocks; }
  size_t pageSize(){ return _fs.cfg.log_page_size; }
  size_t allocatedPages(){ return _fs.stats_p_allocated; }
  size_t deletedPages(){ return _fs.stats_p_deleted; }
  
  FSFile open(char *filename, uint8_t mode = FSFILE_READ);
  FSFile open(spiffs_dirent* entry, uint8_t mode = FSFILE_READ);

protected:
  int _mountInternal();
  std::unique_ptr<uint8_t[]> _work;
  std::unique_ptr<uint8_t[]> _fds;
  size_t _fdsSize;
  std::unique_ptr<uint8_t[]> _cache;
  size_t _cacheSize;
  uint32_t _beginAddress;
  uint32_t _endAddress;
  uint32_t _pageSize;
  uint32_t _blockSize;
  uint32_t _maxOpenFiles;
  spiffs _fs;


private:
  friend class FSFile;
};

extern FSClass FS;

#endif
