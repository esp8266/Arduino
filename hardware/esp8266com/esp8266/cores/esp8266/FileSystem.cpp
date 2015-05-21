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
#include "spiffs/spiffs_esp8266.h"

// These addresses and sizes are defined in the linker script.
// For each flash memory size there is a linker script variant
// which sets spiffs location and size.
extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;
extern "C" uint32_t _SPIFFS_page;
extern "C" uint32_t _SPIFFS_block;

static s32_t api_spiffs_read(u32_t addr, u32_t size, u8_t *dst);
static s32_t api_spiffs_write(u32_t addr, u32_t size, u8_t *src);
static s32_t api_spiffs_erase(u32_t addr, u32_t size);

FSClass FS((uint32_t) &_SPIFFS_start, (uint32_t) &_SPIFFS_end, (uint32_t) &_SPIFFS_page, (uint32_t) &_SPIFFS_block, 4);

FSClass::FSClass(uint32_t beginAddress, uint32_t endAddress, uint32_t pageSize, uint32_t blockSize, uint32_t maxOpenFiles)
: _beginAddress(beginAddress)
, _endAddress(endAddress)
, _pageSize(pageSize)
, _blockSize(blockSize)
, _maxOpenFiles(maxOpenFiles)
, _fs({0})
{
}

int FSClass::_mountInternal(){
  if (_beginAddress == 0 || _beginAddress >= _endAddress){
    SPIFFS_API_DBG_E("Can't start file system, wrong address\r\n");
    return SPIFFS_ERR_NOT_CONFIGURED;
  }
  if(_pageSize == 0) _pageSize = 256;
  if(_blockSize == 0) _blockSize = 4096;
  
  spiffs_config cfg = {0};
  cfg.phys_addr = _beginAddress;
  cfg.phys_size = _endAddress - _beginAddress;
  cfg.phys_erase_block = INTERNAL_FLASH_SECTOR_SIZE;
  cfg.log_block_size = _blockSize;
  cfg.log_page_size = _pageSize;
  cfg.hal_read_f = api_spiffs_read;
  cfg.hal_write_f = api_spiffs_write;
  cfg.hal_erase_f = api_spiffs_erase;
  
  SPIFFS_API_DBG_V("FSClass::_mountInternal: start:%x, size:%d Kb\n", cfg.phys_addr, cfg.phys_size / 1024);

  _work.reset(new uint8_t[2*_pageSize]);
  _fdsSize = 32 * _maxOpenFiles;
  _fds.reset(new uint8_t[_fdsSize]);
  _cacheSize = (32 + _pageSize) * _maxOpenFiles;
  _cache.reset(new uint8_t[_cacheSize]);

  s32_t res = SPIFFS_mount(&_fs,
    &cfg,
    _work.get(),
    _fds.get(),
    _fdsSize,
    _cache.get(),
    _cacheSize,
    NULL);
  SPIFFS_API_DBG_V("FSClass::_mountInternal: %d\n", res);
  return res;
}

bool FSClass::mount() {
  if(SPIFFS_mounted(&_fs)) 
    return true;

  int res = _mountInternal();
  if(res != SPIFFS_OK){
    int formated = SPIFFS_format(&_fs);
    if(formated != SPIFFS_OK) 
      return false;
    res = _mountInternal();
  }
  return (res == SPIFFS_OK);
}

// TODO: need to invalidate open file objects
void FSClass::unmount() {
  if(SPIFFS_mounted(&_fs))
    SPIFFS_unmount(&_fs);
}

bool FSClass::format() {
  if(!SPIFFS_mounted(&_fs)){
    _mountInternal();
  }
  SPIFFS_unmount(&_fs);
  int formated = SPIFFS_format(&_fs);
  if(formated != SPIFFS_OK) 
    return false;
  return (_mountInternal() == SPIFFS_OK);
}

bool FSClass::check() {
  return SPIFFS_check(&_fs) == SPIFFS_OK;
}

bool FSClass::exists(char *filename) {
  spiffs_stat stat = {0};
  if (SPIFFS_stat(&_fs, filename, &stat) < 0) 
    return false;
  return stat.name[0] != '\0';
}

bool FSClass::create(char *filepath){
  return SPIFFS_creat(&_fs, filepath, 0) == SPIFFS_OK;
}

bool FSClass::remove(char *filepath){
  return SPIFFS_remove(&_fs, filepath) == SPIFFS_OK;
}

bool FSClass::rename(char *filename, char *newname) {
  return SPIFFS_rename(&_fs, filename, newname) == SPIFFS_OK;
}

size_t FSClass::totalBytes(){
  u32_t totalBytes;
  u32_t usedBytes;
  if(SPIFFS_info(&_fs, &totalBytes, &usedBytes) == SPIFFS_OK)
    return totalBytes;
  return 0;
}

size_t FSClass::usedBytes(){
  u32_t totalBytes;
  u32_t usedBytes;
  if(SPIFFS_info(&_fs, &totalBytes, &usedBytes) == SPIFFS_OK)
    return usedBytes;
  return 0;
}

FSFile FSClass::open(char *filename, uint8_t mode) {
  if(strcmp(filename, "")  == 0 || 
     strcmp(filename, "/") == 0)
    return FSFile(&_fs, "/");

  int repeats = 0;
  bool notExist;
  bool canRecreate = (mode & SPIFFS_CREAT) == SPIFFS_CREAT;
  int res;

  do{
    notExist = false;
    res = SPIFFS_open(&_fs, filename, (spiffs_flags)mode, 0);
    int code = SPIFFS_errno(&_fs);
    if (res < 0){
      SPIFFS_API_DBG_E("open errno %d\n", code);
      notExist = (code == SPIFFS_ERR_NOT_FOUND || code == SPIFFS_ERR_DELETED || code == SPIFFS_ERR_FILE_DELETED || code == SPIFFS_ERR_IS_FREE);
      if (notExist && canRecreate)
        remove(filename); // fix for deleted files
    }
  } while (notExist && canRecreate && repeats++ < 3);
  
  if(res){
    return FSFile(&_fs, res);
  }
  return FSFile();
}

FSFile FSClass::open(spiffs_dirent* entry, uint8_t mode){
  int res = SPIFFS_open_by_dirent(&_fs, entry, (spiffs_flags)mode, 0);
  if (res){
    return FSFile(&_fs, res);
  }
  return FSFile();
}

FSFile::FSFile() 
: _file(0)
, _stats({0})
, _fs(0)
{
}

FSFile::FSFile(spiffs* fs, String path) 
: _fs(fs) 
{
  if(path == "/"){
    _file = 0x1;
    os_sprintf((char*)_stats.name, "%s", (char*)path.c_str());
    _stats.size = 0;
    _stats.type = SPIFFS_TYPE_DIR;
    SPIFFS_opendir(_fs, (char*)_stats.name, &_dir);
  } else {
    _file = SPIFFS_open(_fs, (char *)path.c_str(), (spiffs_flags)FSFILE_READ, 0);
    if(SPIFFS_fstat(_fs, _file, &_stats) != 0){
      SPIFFS_API_DBG_E("fstat errno %d\n", SPIFFS_errno(_fs));
    }
    //debugf("FSFile name: %s, size: %d, type: %d\n", _stats.name, _stats.size, _stats.type);
    if(_stats.type == SPIFFS_TYPE_DIR){
      SPIFFS_opendir(_fs, (char*)_stats.name, &_dir);
    }
  }
}

FSFile::FSFile(spiffs* fs, file_t f)
: _file(f)
, _fs(fs)
{
  if(SPIFFS_fstat(_fs, _file, &_stats) != 0){
    SPIFFS_API_DBG_E("fstat errno %d\n", SPIFFS_errno(_fs));
  }
  //debugf("FSFile name: %s, size: %d, type: %d\n", _stats.name, _stats.size, _stats.type);
  if(_stats.type == SPIFFS_TYPE_DIR){
    SPIFFS_opendir(_fs, (char*)_stats.name, &_dir);
  }
}

void FSFile::close() {
  if (!_file)
    return;
  if(_stats.type == SPIFFS_TYPE_DIR){
    SPIFFS_closedir(&_dir);
  }
  if(os_strlen((char*)_stats.name) > 1)
    SPIFFS_close(_fs, _file);
  _file = 0;
}

char * FSFile::name(){
  return (char*)_stats.name;
}

bool FSFile::isDirectory(void) {
  return _stats.type == SPIFFS_TYPE_DIR;
}

void FSFile::rewindDirectory() {
  if (!_file || !isDirectory()) 
    return;
  SPIFFS_closedir(&_dir);
  SPIFFS_opendir(_fs, (char*)_stats.name, &_dir);
}

FSFile FSFile::openNextFile(){
  if (!_file || !isDirectory()) 
    return FSFile();
  struct spiffs_dirent e;
  struct spiffs_dirent *pe = &e;
  if ((pe = SPIFFS_readdir(&_dir, pe))){
    // TODO: store actual FS pointer
    return FS.open((char *)pe->name);
  }
  return FSFile();
}

uint32_t FSFile::size() {
  if(!_file)
    return 0;
  if(_stats.size) 
    return _stats.size;
  uint32_t pos = SPIFFS_tell(_fs, _file);
  SPIFFS_lseek(_fs, _file, 0, SPIFFS_SEEK_END);
  _stats.size = SPIFFS_tell(_fs, _file);
  SPIFFS_lseek(_fs, _file, pos, SPIFFS_SEEK_SET);
  return _stats.size;
}

int FSFile::available() {
  if (!_file) 
    return 0;
  uint32_t pos = SPIFFS_tell(_fs, _file);
  return size() - pos;
}

uint32_t FSFile::seek(uint32_t pos) {
  if (!_file) 
    return 0;
  return SPIFFS_lseek(_fs, _file, pos, SPIFFS_SEEK_SET);
}

uint32_t FSFile::position() {
  if (!_file) 
    return 0;
  return SPIFFS_tell(_fs, _file);
}

bool FSFile::eof() {
  if (!_file) 
    return 0;
  return SPIFFS_eof(_fs, _file);
}

int FSFile::read(void *buf, uint16_t nbyte) {
  if (!_file || isDirectory()) 
    return -1;
  return SPIFFS_read(_fs, _file, buf, nbyte);
}

int FSFile::read() {
  if (! _file || isDirectory()) 
    return -1;
  int val;
  if(SPIFFS_read(_fs, _file, &val, 1) != 1) return -1;
  return val;
}

int FSFile::peek() {
  if (!_file || isDirectory()) 
    return 0;
  int c = read();
  SPIFFS_lseek(_fs, _file, -1, SPIFFS_SEEK_CUR);
  return c;
}

size_t FSFile::write(const uint8_t *buf, size_t size){
  if (!_file || isDirectory()) 
    return 0;
  int res = SPIFFS_write(_fs, _file, (uint8_t *)buf, size);
  return (res > 0)?(size_t)res:0;
}

size_t FSFile::write(uint8_t val) {
  if (!_file || isDirectory()) 
    return 0;
  return write(&val, 1);
}

void FSFile::flush(){
  if (!_file || isDirectory()) 
    return;
  SPIFFS_fflush(_fs, _file);
}

bool FSFile::remove(){
  if (!_file) 
    return 0;
  close();
	return SPIFFS_remove(_fs, (char *)_stats.name) == 0;
}

int FSFile::lastError(){
  return SPIFFS_errno(_fs);
}

void FSFile::clearError(){
  _fs->err_code = SPIFFS_OK;
}


static s32_t api_spiffs_read(u32_t addr, u32_t size, u8_t *dst){
  SPIFFS_API_DBG_V("api_spiffs_read: 0x%08x len: %u\n", addr, size);
  flashmem_read(dst, addr, size);
  return SPIFFS_OK;
}

static s32_t api_spiffs_write(u32_t addr, u32_t size, u8_t *src){
  SPIFFS_API_DBG_V("api_spiffs_write: 0x%08x len: %u\n", addr, size);
  flashmem_write(src, addr, size);
  return SPIFFS_OK;
}

static s32_t api_spiffs_erase(u32_t addr, u32_t size){
  SPIFFS_API_DBG_V("api_spiffs_erase: 0x%08x len: %u\n", addr, size);
  u32_t sect_first = flashmem_get_sector_of_address(addr);
  u32_t sect_last = flashmem_get_sector_of_address(addr+size);
  while( sect_first <= sect_last )
    if( !flashmem_erase_sector( sect_first ++ ) )
      return SPIFFS_ERR_INTERNAL;
  return SPIFFS_OK;
} 


