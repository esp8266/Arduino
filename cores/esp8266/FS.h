/*
 FS.h - file system wrapper
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

#ifndef FS_H
#define FS_H

#include <Arduino.h>
#include <memory>

namespace fs {

class File;
class Dir;

class FileImpl;
typedef std::shared_ptr<FileImpl> FileImplPtr;
class FSImpl;
typedef std::shared_ptr<FSImpl> FSImplPtr;
class DirImpl;
typedef std::shared_ptr<DirImpl> DirImplPtr;

template <typename Tfs>
bool mount(Tfs& fs, const char* mountPoint);

enum SeekMode {
    SeekSet = 0,
    SeekCur = 1,
    SeekEnd = 2
};

class File : public Stream
{
public:
    File(FileImplPtr p = FileImplPtr()) : _p(p) {}

    // Print methods:
    size_t write(uint8_t) override;
    size_t write(const uint8_t *buf, size_t size) override;

    // Stream methods:
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;

    size_t read(uint8_t* buf, size_t size);
    bool seek(uint32_t pos, SeekMode mode);
    size_t position() const;
    size_t size() const;
    void close();
    operator bool() const;
    const char* name() const;

protected:
    FileImplPtr _p;
};

class Dir {
public:
    Dir(DirImplPtr impl = DirImplPtr()): _impl(impl) { }

    File openFile(const char* mode);
    String fileName();
    size_t fileSize();
    bool next();

protected:
    DirImplPtr _impl;
};

struct FSInfo {
    size_t totalBytes;
    size_t usedBytes;
    size_t blockSize;
    size_t pageSize;
    size_t maxOpenFiles;
    size_t maxPathLength;
};

class FS
{
public:
    FS(FSImplPtr impl) : _impl(impl) { }

    bool begin();

    bool format();
    bool info(FSInfo& info);

    File open(const char* path, const char* mode);
    File open(const String& path, const char* mode);

    bool exists(const char* path);
    bool exists(const String& path);

    Dir openDir(const char* path);
    Dir openDir(const String& path);

    bool remove(const char* path);
    bool remove(const String& path);

    bool rename(const char* pathFrom, const char* pathTo);
    bool rename(const String& pathFrom, const String& pathTo);

protected:
    FSImplPtr _impl;
};

} // namespace fs

using fs::FS;
using fs::File;
using fs::Dir;
using fs::SeekMode;
using fs::SeekSet;
using fs::SeekCur;
using fs::SeekEnd;
using fs::FSInfo;

extern FS SPIFFS;

#endif //FS_H
