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
  bool eof();
  void close();
  int lastError();
  void clearError();
  operator bool() { return _file > 0; }
  char * name();
  bool isDirectory(void);

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

private:
  bool _mounted = false;
  
public:
  bool mount();
  void unmount();
  bool format();
  bool exists(const char *filename);
  bool create(const char *filepath);
  bool remove(const char *filepath);
  bool rename(const char *filename, const char *newname);
  
  FSFile open(const char *filename, uint8_t mode = FSFILE_READ);

private:
  friend class FSFile;
};

extern FSClass FS;

#endif
