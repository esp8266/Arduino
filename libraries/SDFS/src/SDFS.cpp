/*
 SDFS.cpp - file system wrapper for SdFat
 Copyright (c) 2019 Earle F. Philhower, III. All rights reserved.

 Based on spiffs_api.cpp which is:
 | Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.

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
#include "SDFS.h"
#include "SDFSFormatter.h"
#include <FS.h>
#include <debug.h>

using namespace fs;


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SDFS)
FS SDFS = FS(FSImplPtr(new sdfs::SDFSImpl()));
#endif

namespace sdfs {


FileImplPtr SDFSImpl::open(const char* path, OpenMode openMode, AccessMode accessMode)
{
    if (!_mounted) {
        DEBUGV("SDFSImpl::open() called on unmounted FS\n");
        return FileImplPtr();
    }
    if (!path || !path[0]) {
        DEBUGV("SDFSImpl::open() called with invalid filename\n");
        return FileImplPtr();
    }
    int flags = _getFlags(openMode, accessMode);
    if ((openMode && OM_CREATE) && strchr(path, '/')) {
        // For file creation, silently make subdirs as needed.  If any fail,
        // it will be caught by the real file open later on
        char *pathStr = strdup(path);
        if (pathStr) {
            // Make dirs up to the final fnamepart
            char *ptr = strrchr(pathStr, '/');
            if (ptr && ptr != pathStr) { // Don't try to make root dir!
                *ptr = 0;
                _fs.mkdir(pathStr, true);
            }
        }
        free(pathStr);
    }
    sdfat::File fd = _fs.open(path, flags);
    if (!fd) {
        DEBUGV("SDFSImpl::openFile: fd=%p path=`%s` openMode=%d accessMode=%d",
               &fd, path, openMode, accessMode);
        return FileImplPtr();
    }
    auto sharedFd = std::make_shared<sdfat::File>(fd);
    return std::make_shared<SDFSFileImpl>(this, sharedFd, path);
}

DirImplPtr SDFSImpl::openDir(const char* path)
{
    if (!_mounted) {
        return DirImplPtr();
    }
    char *pathStr = strdup(path); // Allow edits on our scratch copy
    if (!pathStr) {
        // OOM
        return DirImplPtr();
    }
    // Get rid of any trailing slashes
    while (strlen(pathStr) && (pathStr[strlen(pathStr)-1]=='/')) {
        pathStr[strlen(pathStr)-1] = 0;
    }
    // At this point we have a name of "/blah/blah/blah" or "blah" or ""
    // If that references a directory, just open it and we're done.
    sdfat::File dirFile;
    const char *filter = "";
    if (!pathStr[0]) {
        // openDir("") === openDir("/")
        dirFile = _fs.open("/", sdfat::O_RDONLY);
        filter = "";
    } else if (_fs.exists(pathStr)) {
        dirFile = _fs.open(pathStr, sdfat::O_RDONLY);
        if (dirFile.isDir()) {
            // Easy peasy, path specifies an existing dir!
            filter = "";
        } else {
            dirFile.close();
            // This is a file, so open the containing dir
            char *ptr = strrchr(pathStr, '/');
            if (!ptr) {
                // No slashes, open the root dir
                dirFile = _fs.open("/", sdfat::O_RDONLY);
                filter = pathStr;
            } else {
                // We've got slashes, open the dir one up
                *ptr = 0; // Remove slash, truncare string
                dirFile = _fs.open(pathStr, sdfat::O_RDONLY);
                filter = ptr + 1;
            }
        }
    } else {
        // Name doesn't exist, so use the parent dir of whatever was sent in
        // This is a file, so open the containing dir
        char *ptr = strrchr(pathStr, '/');
        if (!ptr) {
            // No slashes, open the root dir
            dirFile = _fs.open("/", sdfat::O_RDONLY);
            filter = pathStr;
        } else {
            // We've got slashes, open the dir one up
            *ptr = 0; // Remove slash, truncare string
            dirFile = _fs.open(pathStr, sdfat::O_RDONLY);
            filter = ptr + 1;
        }
    }
    if (!dirFile) {
        DEBUGV("SDFSImpl::openDir: path=`%s`\n", path);
        return DirImplPtr();
    }
    auto sharedDir = std::make_shared<sdfat::File>(dirFile);
    auto ret = std::make_shared<SDFSDirImpl>(filter, this, sharedDir, pathStr);
    free(pathStr);
    return ret;
}

bool SDFSImpl::format() {
    if (_mounted) {
        return false;
    }
    SDFSFormatter formatter;
    bool ret = formatter.format(&_fs, _cfg._csPin, _cfg._spiSettings);
    return ret;
}


}; // namespace sdfs

