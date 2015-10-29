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
    virtual void close() = 0;
    virtual const char* name() const = 0;
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
    virtual bool next() = 0;
};

class FSImpl {
public:
    virtual bool begin() = 0;
    virtual bool format() = 0;
    virtual bool info(uint32_t *total, uint32_t *used) = 0;
    virtual FileImplPtr open(const char* path, OpenMode openMode, AccessMode accessMode) = 0;
    virtual bool exists(const char* path) = 0;
    virtual DirImplPtr openDir(const char* path) = 0;
    virtual bool rename(const char* pathFrom, const char* pathTo) = 0;
    virtual bool remove(const char* path) = 0;

};

} // namespace fs

#endif //FSIMPL_H
