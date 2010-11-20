/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#include <SD.h>

void File::write(uint8_t val) {
  SD.file.write(val);
}

void File::write(const char *str) {
  SD.file.write(str);
}

void File::write(const uint8_t *buf, size_t size) {
  SD.file.write(buf, size);
}

int File::read() {
  return SD.file.read();
}

void File::close() {
  SD.file.close();
}

File::operator bool() {
  return SD.file.isOpen();
}
