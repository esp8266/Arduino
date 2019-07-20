/*
 spiffs_api.cpp - file system wrapper for SPIFFS
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.

 This code was influenced by NodeMCU and Sming libraries, and first version of
 Arduino wrapper written by Hristo Gochkov.

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
#include "spiffs_api.h"

using namespace fs;

namespace spiffs_impl {


FileImplPtr SPIFFSImpl::open(const char* path, OpenMode openMode, AccessMode accessMode)
{
    if (!isSpiffsFilenameValid(path)) {
        DEBUGV("SPIFFSImpl::open: invalid path=`%s` \r\n", path);
        return FileImplPtr();
    }
    int mode = getSpiffsMode(openMode, accessMode);
    int fd = SPIFFS_open(&_fs, path, mode, 0);
    if (fd < 0 && _fs.err_code == SPIFFS_ERR_DELETED && (openMode & OM_CREATE)) {
        DEBUGV("SPIFFSImpl::open: fd=%d path=`%s` openMode=%d accessMode=%d err=%d, trying to remove\r\n",
               fd, path, openMode, accessMode, _fs.err_code);
        auto rc = SPIFFS_remove(&_fs, path);
        if (rc != SPIFFS_OK) {
            DEBUGV("SPIFFSImpl::open: SPIFFS_ERR_DELETED, but failed to remove path=`%s` openMode=%d accessMode=%d err=%d\r\n",
                   path, openMode, accessMode, _fs.err_code);
            return FileImplPtr();
        }
        fd = SPIFFS_open(&_fs, path, mode, 0);
    }
    if (fd < 0) {
        DEBUGV("SPIFFSImpl::open: fd=%d path=`%s` openMode=%d accessMode=%d err=%d\r\n",
               fd, path, openMode, accessMode, _fs.err_code);
        return FileImplPtr();
    }
    if (!(mode & SPIFFS_O_RDONLY) && _fs.obj_meta_len) {
        time_t t = time(NULL);
        struct tm tmr;
        localtime_r(&t, &tmr);
        time_t meta = mktime(&tmr);
	DEBUGV("SPIFFSImpl::open updating file write time to %ld\r\n", meta);
        SPIFFS_fupdate_meta(&_fs, fd, &meta);
    }
    return std::make_shared<SPIFFSFileImpl>(this, fd);
}

bool SPIFFSImpl::exists(const char* path)
{
    if (!isSpiffsFilenameValid(path)) {
        DEBUGV("SPIFFSImpl::exists: invalid path=`%s` \r\n", path);
        return false;
    }
    spiffs_stat stat;
    int rc = SPIFFS_stat(&_fs, path, &stat);
    return rc == SPIFFS_OK;
}

DirImplPtr SPIFFSImpl::openDir(const char* path) 
{
    if (strlen(path) > 0 && !isSpiffsFilenameValid(path)) {
        DEBUGV("SPIFFSImpl::openDir: invalid path=`%s` \r\n", path);
        return DirImplPtr();
    }
    spiffs_DIR dir;
    spiffs_DIR* result = SPIFFS_opendir(&_fs, path, &dir);
    if (!result) {
        DEBUGV("SPIFFSImpl::openDir: path=`%s` err=%d\r\n", path, _fs.err_code);
        return DirImplPtr();
    }
    return std::make_shared<SPIFFSDirImpl>(path, this, dir);
}

int getSpiffsMode(OpenMode openMode, AccessMode accessMode)
{
    int mode = 0;
    if (openMode & OM_CREATE) {
        mode |= SPIFFS_CREAT;
    }
    if (openMode & OM_APPEND) {
        mode |= SPIFFS_APPEND;
    }
    if (openMode & OM_TRUNCATE) {
        mode |= SPIFFS_TRUNC;
    }
    if (accessMode & AM_READ) {
        mode |= SPIFFS_RDONLY;
    }
    if (accessMode & AM_WRITE) {
        mode |= SPIFFS_WRONLY;
    }
    return mode;
}

bool isSpiffsFilenameValid(const char* name)
{
    if (name == nullptr) {
        return false;
    }
    auto len = strlen(name);
    return len > 0 && len < SPIFFS_OBJ_NAME_LEN;
}

}; // namespace

// these symbols should be defined in the linker script for each flash layout
#ifndef CORE_MOCK
#ifdef ARDUINO
#ifndef SPIFFS_MAX_OPEN_FILES
#define SPIFFS_MAX_OPEN_FILES 5
#endif

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPIFFS)
FS SPIFFS = FS(FSImplPtr(new spiffs_impl::SPIFFSImpl(
                             FS_PHYS_ADDR,
                             FS_PHYS_SIZE,
                             FS_PHYS_PAGE,
                             FS_PHYS_BLOCK,
                             SPIFFS_MAX_OPEN_FILES)));
#endif // ARDUINO
#endif // !CORE_MOCK

#endif
