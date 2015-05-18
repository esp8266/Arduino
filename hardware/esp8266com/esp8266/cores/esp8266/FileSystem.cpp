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
  if(SPIFFS_mounted(&_filesystemStorageHandle)) return true;
  int res = spiffs_mount();
  if(res != 0){
    int formated = SPIFFS_format(&_filesystemStorageHandle);
    if(formated != 0) return false;
    res = spiffs_mount();
  }
  return (res == 0);
}

void FSClass::unmount() {
  if(SPIFFS_mounted(&_filesystemStorageHandle))
    SPIFFS_unmount(&_filesystemStorageHandle);
}

bool FSClass::format() {
  if(!SPIFFS_mounted(&_filesystemStorageHandle)){
    spiffs_mount();
  }
  SPIFFS_unmount(&_filesystemStorageHandle);
  int formated = SPIFFS_format(&_filesystemStorageHandle);
  if(formated != 0) return false;
  return (spiffs_mount() == 0);
}

bool FSClass::check() {
  return SPIFFS_check(&_filesystemStorageHandle) == 0;
}

bool FSClass::exists(char *filename) {
  spiffs_stat stat = {0};
  if (SPIFFS_stat(&_filesystemStorageHandle, filename, &stat) < 0) 
    return false;
  return stat.name[0] != '\0';
}

bool FSClass::create(char *filepath){
  return SPIFFS_creat(&_filesystemStorageHandle, filepath, 0) == 0;
}

bool FSClass::remove(char *filepath){
  return SPIFFS_remove(&_filesystemStorageHandle, filepath) == 0;
}

bool FSClass::rename(char *filename, char *newname) {
  return SPIFFS_rename(&_filesystemStorageHandle, filename, newname) == 0;
}

size_t FSClass::totalBytes(){
  u32_t totalBytes;
  u32_t usedBytes;
  if(SPIFFS_info(&_filesystemStorageHandle, &totalBytes, &usedBytes) == 0)
    return totalBytes;
  return 0;
}

size_t FSClass::usedBytes(){
  u32_t totalBytes;
  u32_t usedBytes;
  if(SPIFFS_info(&_filesystemStorageHandle, &totalBytes, &usedBytes) == 0)
    return usedBytes;
  return 0;
}

FSFile FSClass::open(char *filename, uint8_t mode) {
  if(String(filename) == "" || String(filename) == "/") return FSFile("/");
  int repeats = 0;
  bool notExist;
  bool canRecreate = (mode & SPIFFS_CREAT) == SPIFFS_CREAT;
  int res;

  do{
    notExist = false;
    res = SPIFFS_open(&_filesystemStorageHandle, filename, (spiffs_flags)mode, 0);
    int code = SPIFFS_errno(&_filesystemStorageHandle);
    if (res < 0){
      SPIFFS_API_DBG_E("open errno %d\n", code);
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

FSFile FSClass::open(spiffs_dirent* entry, uint8_t mode){
  int res = SPIFFS_open_by_dirent(&_filesystemStorageHandle, entry, (spiffs_flags)mode, 0);
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

FSFile::FSFile(String path) {
  if(path == "/"){
    _file = 0x1;
    os_sprintf((char*)_stats.name, "%s", (char*)path.c_str());
    _stats.size = 0;
    _stats.type = SPIFFS_TYPE_DIR;
    SPIFFS_opendir(&_filesystemStorageHandle, (char*)_stats.name, &_dir);
  } else {
    _file = SPIFFS_open(&_filesystemStorageHandle, (char *)path.c_str(), (spiffs_flags)FSFILE_READ, 0);
    if(SPIFFS_fstat(&_filesystemStorageHandle, _file, &_stats) != 0){
      SPIFFS_API_DBG_E("fstat errno %d\n", SPIFFS_errno(&_filesystemStorageHandle));
    }
    //debugf("FSFile name: %s, size: %d, type: %d\n", _stats.name, _stats.size, _stats.type);
    if(_stats.type == SPIFFS_TYPE_DIR){
      SPIFFS_opendir(&_filesystemStorageHandle, (char*)_stats.name, &_dir);
    }
  }
}

FSFile::FSFile(file_t f) {
  _file = f;
  if(SPIFFS_fstat(&_filesystemStorageHandle, _file, &_stats) != 0){
    SPIFFS_API_DBG_E("fstat errno %d\n", SPIFFS_errno(&_filesystemStorageHandle));
  }
  //debugf("FSFile name: %s, size: %d, type: %d\n", _stats.name, _stats.size, _stats.type);
  if(_stats.type == SPIFFS_TYPE_DIR){
    SPIFFS_opendir(&_filesystemStorageHandle, (char*)_stats.name, &_dir);
  }
}

void FSFile::close() {
  if (! _file) return;
  if(_stats.type == SPIFFS_TYPE_DIR){
    SPIFFS_closedir(&_dir);
  }
  if(os_strlen((char*)_stats.name) > 1)
    SPIFFS_close(&_filesystemStorageHandle, _file);
  _file = 0;
}

char * FSFile::name(){
  return (char*)_stats.name;
}

bool FSFile::isDirectory(void) {
  return _stats.type == SPIFFS_TYPE_DIR;
}

void FSFile::rewindDirectory() {
  if (!_file || !isDirectory()) return;
  SPIFFS_closedir(&_dir);
  SPIFFS_opendir(&_filesystemStorageHandle, (char*)_stats.name, &_dir);
}

FSFile FSFile::openNextFile(){
  if (!_file || !isDirectory()) return FSFile();
  struct spiffs_dirent e;
  struct spiffs_dirent *pe = &e;
  if ((pe = SPIFFS_readdir(&_dir, pe))){
    return FS.open((char *)pe->name);
  }
  return FSFile();
}

uint32_t FSFile::size() {
  if(!_file) return 0;
  if(_stats.size) return _stats.size;
  uint32_t pos = SPIFFS_tell(&_filesystemStorageHandle, _file);
  SPIFFS_lseek(&_filesystemStorageHandle, _file, 0, SPIFFS_SEEK_END);
  _stats.size = SPIFFS_tell(&_filesystemStorageHandle, _file);
  SPIFFS_lseek(&_filesystemStorageHandle, _file, pos, SPIFFS_SEEK_SET);
  return _stats.size;
}

int FSFile::available() {
  if (!_file) return 0;
  uint32_t pos = SPIFFS_tell(&_filesystemStorageHandle, _file);
  return size() - pos;
}

uint32_t FSFile::seek(uint32_t pos) {
  if (!_file) return 0;
  return SPIFFS_lseek(&_filesystemStorageHandle, _file, pos, SPIFFS_SEEK_SET);
}

uint32_t FSFile::position() {
  if (!_file) return 0;
  return SPIFFS_tell(&_filesystemStorageHandle, _file);
}

bool FSFile::eof() {
  if (!_file) return 0;
  return SPIFFS_eof(&_filesystemStorageHandle, _file);
}

int FSFile::read(void *buf, uint16_t nbyte) {
  if (! _file || isDirectory()) return -1;
  return SPIFFS_read(&_filesystemStorageHandle, _file, buf, nbyte);
}

int FSFile::read() {
  if (! _file || isDirectory()) return -1;
  int val;
  if(SPIFFS_read(&_filesystemStorageHandle, _file, &val, 1) != 1) return -1;
  return val;
}

int FSFile::peek() {
  if (! _file || isDirectory()) return 0;
  int c = read();
  SPIFFS_lseek(&_filesystemStorageHandle, _file, -1, SPIFFS_SEEK_CUR);
  return c;
}

size_t FSFile::write(const uint8_t *buf, size_t size){
  if (! _file || isDirectory()) return 0;
  int res = SPIFFS_write(&_filesystemStorageHandle, _file, (uint8_t *)buf, size);
  return (res > 0)?(size_t)res:0;
}

size_t FSFile::write(uint8_t val) {
  if (! _file || isDirectory()) return 0;
  return write(&val, 1);
}

void FSFile::flush(){
  if (! _file || isDirectory()) return;
  SPIFFS_fflush(&_filesystemStorageHandle, _file);
}

bool FSFile::remove(){
  if (! _file) return 0;
  close();
	return SPIFFS_remove(&_filesystemStorageHandle, (char *)_stats.name) == 0;
}

int FSFile::lastError(){
  return SPIFFS_errno(&_filesystemStorageHandle);
}

void FSFile::clearError(){
  _filesystemStorageHandle.err_code = SPIFFS_OK;
}
