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
    virtual int availableForWrite() { return 0; }
    virtual bool truncate(uint32_t size) = 0;
    virtual void close() = 0;
    virtual const char* name() const = 0;
    virtual const char* fullName() const = 0;
    virtual bool isFile() const = 0;
    virtual bool isDirectory() const = 0;

    // Filesystems *may* support a timestamp per-file, so allow the user to override with
    // their own callback for *this specific* file (as opposed to the FSImpl call of the
    // same name.  The default implementation simply returns time(null)
    virtual void setTimeCallback(time_t (*cb)(void)) { _timeCallback = cb; }

    // Return the last written time for a file.  Undefined when called on a writable file
    // as the FS is allowed to return either the time of the last write() operation or the
    // time present in the filesystem metadata (often the last time the file was closed)
    virtual time_t getLastWrite() { return 0; } // Default is to not support timestamps
    // Same for creation time.
    virtual time_t getCreationTime() { return 0; } // Default is to not support timestamps

protected:
    time_t (*_timeCallback)(void) = nullptr;
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
    // Return the last written time for a file.  Undefined when called on a writable file
    // as the FS is allowed to return either the time of the last write() operation or the
    // time present in the filesystem metadata (often the last time the file was closed)
    virtual time_t fileTime() { return 0; } // By default, FS doesn't report file times
    virtual time_t fileCreationTime() { return 0; } // By default, FS doesn't report file times
    virtual bool isFile() const = 0;
    virtual bool isDirectory() const = 0;
    virtual bool next() = 0;
    virtual bool rewind() = 0;

    // Filesystems *may* support a timestamp per-file, so allow the user to override with
    // their own callback for *this specific* file (as opposed to the FSImpl call of the
    // same name.  The default implementation simply returns time(null)
    virtual void setTimeCallback(time_t (*cb)(void)) { _timeCallback = cb; }

protected:
    time_t (*_timeCallback)(void) = nullptr;
};

class FSImpl {
public:
    virtual ~FSImpl () { }
    virtual bool setConfig(const FSConfig &cfg) = 0;
    virtual bool begin() = 0;
    virtual void end() = 0;
    virtual bool format() = 0;
    virtual bool info(FSInfo& info) = 0;
    virtual bool info64(FSInfo64& info) = 0;
    virtual FileImplPtr open(const char* path, OpenMode openMode, AccessMode accessMode) = 0;
    virtual bool exists(const char* path) = 0;
    virtual DirImplPtr openDir(const char* path) = 0;
    virtual bool rename(const char* pathFrom, const char* pathTo) = 0;
    virtual bool remove(const char* path) = 0;
    virtual bool mkdir(const char* path) = 0;
    virtual bool rmdir(const char* path) = 0;
    virtual bool gc() { return true; } // May not be implemented in all file systems.
    virtual bool check() { return true; } // May not be implemented in all file systems.
    virtual time_t getCreationTime() { return 0; } // May not be implemented in all file systems.

    // Filesystems *may* support a timestamp per-file, so allow the user to override with
    // their own callback for all files on this FS.  The default implementation simply
    // returns the present time as reported by time(null)
    virtual void setTimeCallback(time_t (*cb)(void)) { _timeCallback = cb; }

protected:
    time_t (*_timeCallback)(void) = nullptr;
};

} // namespace fs

#endif //FSIMPL_H
