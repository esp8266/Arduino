/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <FileIO.h>

File::File(BridgeClass &b) : mode(255), bridge(b) {
  // Empty
}

File::File(const char *_filename, uint8_t _mode, BridgeClass &b) : mode(_mode), bridge(b) {
  filename = _filename;
  uint8_t err;
  char modes[] = {'r','w','a'};
  handle = bridge.fileOpen(filename, modes[mode], err);
  if (err != 0)
    mode = 255; // In case of error keep the file closed
  buffered = 0;
}

File::operator bool() {
  return (mode != 255);
}

File::~File() {
  close();
}

size_t File::write(uint8_t c) {
  return write(&c, 1);
}

size_t File::write(const uint8_t *buf, size_t size) {
  if (mode == 255)
	return -1;
  uint8_t err;
  bridge.fileWrite(handle, buf, size, err);
  if (err != 0)
	return -err;
  return size;
}

int File::read() {
  doBuffer();
  if (buffered == 0)
    return -1; // no chars available
  else {
    buffered--;
    return buffer[readPos++];
  }
}

int File::peek() {
  doBuffer();
  if (buffered == 0)
    return -1; // no chars available
  else
    return buffer[readPos];
}

boolean File::seek(uint32_t pos) {
  uint8_t err;
  bridge.fileSeek(handle, pos, err);
  return err==0;
}

void File::doBuffer() {
  // If there are already char in buffer exit
  if (buffered > 0)
    return;

  // Try to buffer up to 32 characters
  readPos = 0;
  uint8_t err;
  buffered = bridge.fileRead(handle, buffer, sizeof(buffer), err);
}

int File::available() {
  // Look if there is new data available
  doBuffer();
  return buffered;
}

void File::flush() {
}

//int read(void *buf, uint16_t nbyte)
//uint32_t position()
//uint32_t size()

void File::close() {
  if (mode == 255)
    return;
  bridge.fileClose(handle);
  mode = 255;
}

const char *File::name() {
  return filename.c_str();
}

//boolean isDirectory(void)
//File openNextFile(uint8_t mode = O_RDONLY);
//void rewindDirectory(void)






boolean SDClass::begin() {
  return true;
}

File SDClass::open(const char *filename, uint8_t mode) {
//  if (mode == FILE_READ) {
//    if (exists(filename))
//      return File(filename, mode);
//  }
//  if (mode == FILE_WRITE || mode == FILE_APPEND) {
    return File(filename, mode);
//  }
//  return File();
}

boolean SDClass::exists(const char *filepath) {
  Process ls;
  ls.begin("ls");
  ls.addParameter(filepath);
  int res = ls.run();
  return (res == 0);
}

boolean SDClass::mkdir(const char *filepath) {
  Process mk;
  mk.begin("mkdir");
  mk.addParameter("-p");
  mk.addParameter(filepath);
  int res = mk.run();
  return (res == 0);
}

boolean SDClass::remove(const char *filepath) {
  Process rm;
  rm.begin("rm");
  rm.addParameter(filepath);
  int res = rm.run();
  return (res == 0);
}

boolean SDClass::rmdir(const char *filepath) {
  Process rm;
  rm.begin("rmdir");
  rm.addParameter(filepath);
  int res = rm.run();
  return (res == 0);
}

SDClass SD;
