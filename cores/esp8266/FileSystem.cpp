/* 
  FileSystem.cpp - SPIFS implementation for esp8266

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
#include "FileSystem.h"
#include "Arduino.h"

bool FSClass::mount() {
  if (_mounted) 
    return true;

  _mounted = spiffs_mount();
  return _mounted;
}

void FSClass::unmount() {
  if (!_mounted)
    return;

  spiffs_unmount();
  _mounted = false;
}

bool FSClass::format() {
  return spiffs_format();
}

bool FSClass::exists(const char *filename) {
  spiffs_stat stat = {0};
  if (SPIFFS_stat(&_filesystemStorageHandle, filename, &stat) < 0) 
    return false;
  return stat.name[0] != '\0';
}

bool FSClass::create(const char *filepath){
  return SPIFFS_creat(&_filesystemStorageHandle, filepath, 0) == 0;
}

bool FSClass::remove(const char *filepath){
  return SPIFFS_remove(&_filesystemStorageHandle, filepath) == 0;
}

bool FSClass::rename(const char *filename, const char *newname) {
  return SPIFFS_rename(&_filesystemStorageHandle, filename, newname) == 0;
}

FSFile FSClass::open(const char *filename, uint8_t mode) {
  int repeats = 0;
  bool notExist;
  bool canRecreate = (mode & SPIFFS_CREAT) == SPIFFS_CREAT;
  int res;

  do{
    notExist = false;
    res = SPIFFS_open(&_filesystemStorageHandle, filename, (spiffs_flags)mode, 0);
    int code = SPIFFS_errno(&_filesystemStorageHandle);
    if (res < 0){
      debugf("open errno %d\n", code);
      notExist = (code == SPIFFS_ERR_NOT_FOUND || code == SPIFFS_ERR_DELETED || code == SPIFFS_ERR_FILE_DELETED || code == SPIFFS_ERR_IS_FREE);
      if (notExist && canRecreate)
        remove(filename); // fix for deleted files
    }
  } while (notExist && canRecreate && repeats++ < 3);
  
  if(res){
    return FSFile(res);
  }
  return FSFile();
}

FSClass FS;

FSFile::FSFile() {
  _file = 0;
  _stats = {0};
}

FSFile::FSFile(file_t f) {
  _file = f;
  if(SPIFFS_fstat(&_filesystemStorageHandle, _file, &_stats) != 0){
    debugf("mount errno %d\n", SPIFFS_errno(&_filesystemStorageHandle));
  }
}

void FSFile::close() {
  if (! _file) return;
  SPIFFS_close(&_filesystemStorageHandle, _file);
  _file = 0;
}

uint32_t FSFile::size() {
  if(! _file) return 0;
  uint32_t pos = SPIFFS_tell(&_filesystemStorageHandle, _file);
  SPIFFS_lseek(&_filesystemStorageHandle, _file, 0, SPIFFS_SEEK_END);
  uint32_t size = SPIFFS_tell(&_filesystemStorageHandle, _file);
  SPIFFS_lseek(&_filesystemStorageHandle, _file, pos, SPIFFS_SEEK_SET);
  return size;
}

uint32_t FSFile::seek(uint32_t pos) {
  if (! _file) return 0;
  return SPIFFS_lseek(&_filesystemStorageHandle, _file, pos, SPIFFS_SEEK_SET);
}

uint32_t FSFile::position() {
  if (! _file) return 0;
  return SPIFFS_tell(&_filesystemStorageHandle, _file);
}

bool FSFile::eof() {
  if (! _file) return 0;
  return SPIFFS_eof(&_filesystemStorageHandle, _file);
}

bool FSFile::isDirectory(void) {
  return false;
}

int FSFile::read(void *buf, uint16_t nbyte) {
  if (! _file) return -1;
  return SPIFFS_read(&_filesystemStorageHandle, _file, buf, nbyte);
}

int FSFile::read() {
  if (! _file) return -1;
  int val;
  if(SPIFFS_read(&_filesystemStorageHandle, _file, &val, 1) != 1) return -1;
  return val;
}

int FSFile::peek() {
  if (! _file) return 0;
  int c = read();
  SPIFFS_lseek(&_filesystemStorageHandle, _file, -1, SPIFFS_SEEK_CUR);
  return c;
}

int FSFile::available() {
  if (! _file) return 0;
  uint32_t pos = SPIFFS_tell(&_filesystemStorageHandle, _file);
  SPIFFS_lseek(&_filesystemStorageHandle, _file, 0, SPIFFS_SEEK_END);
  uint32_t size = SPIFFS_tell(&_filesystemStorageHandle, _file);
  SPIFFS_lseek(&_filesystemStorageHandle, _file, pos, SPIFFS_SEEK_SET);
  return size - pos;
}

size_t FSFile::write(const uint8_t *buf, size_t size){
  if (! _file) return 0;
  int res = SPIFFS_write(&_filesystemStorageHandle, _file, (uint8_t *)buf, size);
  return (res > 0)?res:0;
}

size_t FSFile::write(uint8_t val) {
  if (! _file) return 0;
  return write(&val, 1);
}

void FSFile::flush(){
  if (! _file) return;
  SPIFFS_fflush(&_filesystemStorageHandle, _file);
}

uint32_t FSFile::remove(){
  if (! _file) return 0;
	return SPIFFS_fremove(&_filesystemStorageHandle, _file);
  _file = 0;
}

int FSFile::lastError(){
  return SPIFFS_errno(&_filesystemStorageHandle);
}

void FSFile::clearError(){
  _filesystemStorageHandle.errno = SPIFFS_OK;
}

char * FSFile::name(){
  return 0;
}






/*
spiffs_DIR *dirOpen(spiffs_DIR *d){
  return SPIFFS_opendir(&_filesystemStorageHandle, 0, d);
}

int dirClose(spiffs_DIR *d){
  return SPIFFS_closedir(d);
}

file_t dirOpenFile(spiffs_dirent* entry, uint8_t flags){
  return SPIFFS_open_by_dirent(&_filesystemStorageHandle, entry, (spiffs_flags)flags, 0);
}
*/
