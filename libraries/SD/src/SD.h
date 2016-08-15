/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#ifndef __SD_H__
#define __SD_H__

#include <Arduino.h>

#include <utility/SdFat.h>
#include <utility/SdFatUtil.h>

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT)

class File : public Stream {
 private:
  char _name[13]; // our name
  SdFile *_file;  // underlying file pointer

public:
  File(SdFile f, const char *name);     // wraps an underlying SdFile
  File(void);      // 'empty' constructor
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int read();
  virtual int peek();
  virtual int available();
  virtual void flush();
  int read(void *buf, uint16_t nbyte);
  boolean seek(uint32_t pos);
  uint32_t position();
  uint32_t size();
  void close();
  operator bool();
  char * name();

  boolean isDirectory(void);
  File openNextFile(uint8_t mode = O_RDONLY);
  void rewindDirectory(void);

  template<typename T> size_t write(T &src){
    uint8_t obuf[512];
    size_t doneLen = 0;
    size_t sentLen;
    int i;

    while (src.available() > 512){
      src.read(obuf, 512);
      sentLen = write(obuf, 512);
      doneLen = doneLen + sentLen;
      if(sentLen != 512){
        return doneLen;
      }
    }
  
    size_t leftLen = src.available();
    src.read(obuf, leftLen);
    sentLen = write(obuf, leftLen);
    doneLen = doneLen + sentLen;
    return doneLen;
  }
  
  using Print::write;
};

class SDClass {

private:
  // These are required for initialisation and use of sdfatlib
  Sd2Card card;
  SdVolume volume;
  SdFile root;
  
  // my quick&dirty iterator, should be replaced
  SdFile getParentDir(const char *filepath, int *indx);
public:
  // This needs to be called to set up the connection to the SD card
  // before other methods are used.
  boolean begin(uint8_t csPin = SD_CHIP_SELECT_PIN, uint32_t speed = SPI_HALF_SPEED);

  // Open the specified file/directory with the supplied mode (e.g. read or
  // write, etc). Returns a File object for interacting with the file.
  // Note that currently only one file can be open at a time.
  File open(const char *filename, uint8_t mode = FILE_READ);
  File open(const String &filename, uint8_t mode = FILE_READ) { return open( filename.c_str(), mode ); }

  // Methods to determine if the requested file path exists.
  boolean exists(char *filepath);
  boolean exists(const String &filepath) { return exists(filepath.c_str()); }

  // Create the requested directory heirarchy--if intermediate directories
  // do not exist they will be created.
  boolean mkdir(char *filepath);
  boolean mkdir(const String &filepath) { return mkdir(filepath.c_str()); }
  
  // Delete the file.
  boolean remove(char *filepath);
  boolean remove(const String &filepath) { return remove(filepath.c_str()); }
  
  boolean rmdir(char *filepath);
  boolean rmdir(const String &filepath) { return rmdir(filepath.c_str()); }

  uint8_t type(){ return card.type(); }
  uint8_t fatType(){ return volume.fatType(); }
  size_t blocksPerCluster(){ return volume.blocksPerCluster(); }
  size_t totalClusters(){ return volume.clusterCount(); }
  size_t blockSize(){ return (size_t)0x200; }
  size_t totalBlocks(){ return (totalClusters() / blocksPerCluster()); }
  size_t clusterSize(){ return blocksPerCluster() * blockSize(); }
  size_t size(){ return (clusterSize() * totalClusters()); }
private:

  // This is used to determine the mode used to open a file
  // it's here because it's the easiest place to pass the 
  // information through the directory walking function. But
  // it's probably not the best place for it.
  // It shouldn't be set directly--it is set via the parameters to `open`.
  int fileOpenMode;
  
  friend class File;
  friend boolean callback_openPath(SdFile&, char *, boolean, void *); 
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SD)
extern SDClass SD;
#endif

#endif
