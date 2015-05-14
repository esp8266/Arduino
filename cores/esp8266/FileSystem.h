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

#include "spiffs/spiffs.h"
#include "Arduino.h"
class String;

#define FSFILE_READ SPIFFS_RDONLY
#define FSFILE_WRITE (SPIFFS_RDONLY | SPIFFS_WRONLY | SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_TRUNC)

class FSFile : public Stream {
 private:
  spiffs_stat _stats; 
  file_t _file;

public:
  FSFile(file_t f);
  FSFile(void);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int read();
  virtual int peek();
  virtual int available();
  virtual void flush();
  int read(void *buf, uint16_t nbyte);
  uint32_t seek(uint32_t pos);
  uint32_t remove();
  uint32_t position();
  uint32_t size();
  boolean eof();
  void close();
  int lastError();
  void clearError();
  operator bool(){ return _file > 0; }
  char * name();
  boolean isDirectory(void);

  template<typename T> size_t write(T &src){
    uint8_t obuf[64];
    size_t doneLen = 0;
    size_t sentLen;
    int i;

    while (src.available() > 64){
      src.read(obuf, 64);
      sentLen = write(obuf, 64);
      doneLen = doneLen + sentLen;
      if(sentLen != 64){
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

class FSClass {

private:
  boolean _mounted;
  
public:
  boolean mount();
  void unmount();
  boolean format();
  boolean exists(const char *filename);
  boolean create(const char *filepath);
  boolean remove(const char *filepath);
  boolean rename(const char *filename, const char *newname);
  
  FSFile open(const char *filename, uint8_t mode = FSFILE_READ);

private:
  friend class FSFile;
};

extern FSClass FS;

#endif
