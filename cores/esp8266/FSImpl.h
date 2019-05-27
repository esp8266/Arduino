/*
 FSImpl.h - base file system interface
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
#ifndef FSIMPL_H
#define FSIMPL_H

#include <stddef.h>
#include <stdint.h>
#include <FS.h>

namespace fs {

class FileImpl {
public:
    virtual ~FileImpl() { }
    virtual size_t write(const uint8_t *buf, size_t size) = 0;
    virtual size_t read(uint8_t* buf, size_t size) = 0;
    virtual void flush() = 0;
    virtual bool seek(uint32_t pos, SeekMode mode) = 0;
    virtual size_t position() const = 0;
    virtual size_t size() const = 0;
    virtual bool truncate(uint32_t size) = 0;
    virtual void close() = 0;
    virtual const char* name() const = 0;
    virtual const char* fullName() const = 0;
    virtual bool isFile() const = 0;
    virtual bool isDirectory() const = 0;
};

enum OpenMode {
    OM_DEFAULT = 0,
    OM_CREATE = 1,
    OM_APPEND = 2,
    OM_TRUNCATE = 4
};

enum AccessMode {
    AM_READ = 1,
    AM_WRITE = 2,
    AM_RW = AM_READ | AM_WRITE
};

class DirImpl {
public:
    virtual ~DirImpl() { }
    virtual FileImplPtr openFile(OpenMode openMode, AccessMode accessMode) = 0;
    virtual const char* fileName() = 0;
    virtual size_t fileSize() = 0;
    virtual bool isFile() const = 0;
    virtual bool isDirectory() const = 0;
    virtual bool next() = 0;
    virtual bool rewind() = 0;
};

class FSImpl {
public:
    virtual ~FSImpl () { }
    virtual bool setConfig(const FSConfig &cfg) = 0;
    virtual bool begin() = 0;
    virtual void end() = 0;
    virtual bool format() = 0;
    virtual bool info(FSInfo& info) = 0;
    // For seamless backward compatibilty, supply default which just copies form the 32bit version
    virtual bool info64(FSInfo64& info64) {
        FSInfo i;
        if (!info(i)) {
            return false;
        }
        info64.blockSize     = i.blockSize;
        info64.pageSize      = i.pageSize;
        info64.maxOpenFiles  = i.maxOpenFiles;
        info64.maxPathLength = i.maxPathLength;
        info64.totalBytes    = i.totalBytes;
        info64.usedBytes     = i.usedBytes;
        return true;
    }
    virtual FileImplPtr open(const char* path, OpenMode openMode, AccessMode accessMode) = 0;
    virtual bool exists(const char* path) = 0;
    virtual DirImplPtr openDir(const char* path) = 0;
    virtual bool rename(const char* pathFrom, const char* pathTo) = 0;
    virtual bool remove(const char* path) = 0;
    virtual bool mkdir(const char* path) = 0;
    virtual bool rmdir(const char* path) = 0;
    virtual bool gc() { return true; } // May not be implemented in all file systems.
};

} // namespace fs

#endif //FSIMPL_H
