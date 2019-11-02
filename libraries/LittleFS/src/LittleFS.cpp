/*
 LittleFS.cpp - Wrapper for LittleFS for ESP8266
 Copyright (c_ 2019 Earle F. Philhower, III.  All rights reserved.

 Based extensively off of the ESP8266 SPIFFS code, which is
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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

#include <Arduino.h>
#include <stdlib.h>
#include <algorithm>
#include "LittleFS.h"
#include "debug.h"
#include "flash_hal.h"

extern "C" {
#include "c_types.h"
#include "spi_flash.h"
}

namespace littlefs_impl {

FileImplPtr LittleFSImpl::open(const char* path, OpenMode openMode, AccessMode accessMode) {
    if (!_mounted) {
        DEBUGV("LittleFSImpl::open() called on unmounted FS\n");
        return FileImplPtr();
    }
    if (!path || !path[0]) {
        DEBUGV("LittleFSImpl::open() called with invalid filename\n");
        return FileImplPtr();
    }
    if (!LittleFSImpl::pathValid(path)) {
        DEBUGV("LittleFSImpl::open() called with too long filename\n");
        return FileImplPtr();
    }

    int flags = _getFlags(openMode, accessMode);
    auto fd = std::make_shared<lfs_file_t>();

    if ((openMode && OM_CREATE) && strchr(path, '/')) {
        // For file creation, silently make subdirs as needed.  If any fail,
        // it will be caught by the real file open later on
        char *pathStr = strdup(path);
        if (pathStr) {
            // Make dirs up to the final fnamepart
            char *ptr = strchr(pathStr, '/');
            while (ptr) {
                *ptr = 0;
                lfs_mkdir(&_lfs, pathStr);
                *ptr = '/';
                ptr = strchr(ptr+1, '/');
            }
        }
        free(pathStr);
    }
    int rc = lfs_file_open(&_lfs, fd.get(), path, flags);
    if (rc == LFS_ERR_ISDIR) {
        // To support the SD.openNextFile, a null FD indicates to the LittleFSFile this is just
        // a directory whose name we are carrying around but which cannot be read or written
        return std::make_shared<LittleFSFileImpl>(this, path, nullptr);
    } else if (rc == 0) {
        return std::make_shared<LittleFSFileImpl>(this, path, fd);
    } else {
        DEBUGV("LittleFSDirImpl::openFile: rc=%d fd=%p path=`%s` openMode=%d accessMode=%d err=%d\n",
               rc, fd.get(), path, openMode, accessMode, rc);
        return FileImplPtr();
    }
}

DirImplPtr LittleFSImpl::openDir(const char *path) {
    if (!_mounted || !path) {
        return DirImplPtr();
    }
    char *pathStr = strdup(path); // Allow edits on our scratch copy
    // Get rid of any trailing slashes
    while (strlen(pathStr) && (pathStr[strlen(pathStr)-1]=='/')) {
        pathStr[strlen(pathStr)-1] = 0;
    }
    // At this point we have a name of "blah/blah/blah" or "blah" or ""
    // If that references a directory, just open it and we're done.
    lfs_info info;
    auto dir = std::make_shared<lfs_dir_t>();
    int rc;
    const char *filter = "";
    if (!pathStr[0]) {
        // openDir("") === openDir("/")
        rc = lfs_dir_open(&_lfs, dir.get(), "/");
        filter = "";
    } else if (lfs_stat(&_lfs, pathStr, &info) >= 0) {
        if (info.type == LFS_TYPE_DIR) {
            // Easy peasy, path specifies an existing dir!
            rc = lfs_dir_open(&_lfs, dir.get(), pathStr);
	    filter = "";
        } else {
            // This is a file, so open the containing dir
            char *ptr = strrchr(pathStr, '/');
            if (!ptr) {
                // No slashes, open the root dir
                rc = lfs_dir_open(&_lfs, dir.get(), "/");
		filter = pathStr;
            } else {
                // We've got slashes, open the dir one up
                *ptr = 0; // Remove slash, truncate string
                rc = lfs_dir_open(&_lfs, dir.get(), pathStr);
		filter = ptr + 1;
            }
        }
    } else { 
        // Name doesn't exist, so use the parent dir of whatever was sent in
        // This is a file, so open the containing dir
        char *ptr = strrchr(pathStr, '/');
        if (!ptr) {
            // No slashes, open the root dir
            rc = lfs_dir_open(&_lfs, dir.get(), "/");
	    filter = pathStr;
        } else {
            // We've got slashes, open the dir one up
            *ptr = 0; // Remove slash, truncate string
            rc = lfs_dir_open(&_lfs, dir.get(), pathStr);
	    filter = ptr + 1;
        }
    }
    if (rc < 0) {
        DEBUGV("LittleFSImpl::openDir: path=`%s` err=%d\n", path, rc);
        free(pathStr);
        return DirImplPtr();
    }
    // Skip the . and .. entries
    lfs_info dirent;
    lfs_dir_read(&_lfs, dir.get(), &dirent);
    lfs_dir_read(&_lfs, dir.get(), &dirent);

    auto ret = std::make_shared<LittleFSDirImpl>(filter, this, dir, pathStr);
    free(pathStr);
    return ret;
}

int LittleFSImpl::lfs_flash_read(const struct lfs_config *c,
    lfs_block_t block, lfs_off_t off, void *dst, lfs_size_t size) {
    LittleFSImpl *me = reinterpret_cast<LittleFSImpl*>(c->context);
    uint32_t addr = me->_start + (block * me->_blockSize) + off;
    return flash_hal_read(addr, size, static_cast<uint8_t*>(dst)) == FLASH_HAL_OK ? 0 : -1;
}

int LittleFSImpl::lfs_flash_prog(const struct lfs_config *c,
    lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
    LittleFSImpl *me = reinterpret_cast<LittleFSImpl*>(c->context);
    uint32_t addr = me->_start + (block * me->_blockSize) + off;
    const uint8_t *src = reinterpret_cast<const uint8_t *>(buffer);
    return flash_hal_write(addr, size, static_cast<const uint8_t*>(src)) == FLASH_HAL_OK ? 0 : -1;
}

int LittleFSImpl::lfs_flash_erase(const struct lfs_config *c, lfs_block_t block) {
    LittleFSImpl *me = reinterpret_cast<LittleFSImpl*>(c->context);
    uint32_t addr = me->_start + (block * me->_blockSize);
    uint32_t size = me->_blockSize;
    return flash_hal_erase(addr, size) == FLASH_HAL_OK ? 0 : -1;
}

int LittleFSImpl::lfs_flash_sync(const struct lfs_config *c) {
    /* NOOP */
    (void) c;
    return 0;
}


}; // namespace

extern "C" void littlefs_weak_end(void)
{
    //ets_printf("debug: not weak littlefs end\n");
    LittleFS.end();
}

// these symbols should be defined in the linker script for each flash layout
#ifndef CORE_MOCK
#ifdef ARDUINO
#ifndef FS_MAX_OPEN_FILES
#define FS_MAX_OPEN_FILES 5
#endif

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LITTLEFS)
FS LittleFS = FS(FSImplPtr(new littlefs_impl::LittleFSImpl(FS_PHYS_ADDR, FS_PHYS_SIZE, FS_PHYS_PAGE, FS_PHYS_BLOCK, FS_MAX_OPEN_FILES)));
#endif

#endif // !CORE_MOCK


#endif
