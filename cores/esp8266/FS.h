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

protected:
    FileImplPtr _p;
};

class Dir {
public:
    Dir(DirImplPtr impl = DirImplPtr()): _impl(impl) { }

    File openFile(const char* mode);
    String fileName();
    bool next();

protected:
    DirImplPtr _impl;
};

class FS
{
public:
    FS(FSImplPtr impl) : _impl(impl) { }
    File open(const char* path, const char* mode);
    File open(const String& path, const char* mode);
    Dir openDir(const char* path);
    Dir openDir(const String& path);

protected:
    FSImplPtr _impl;

    template <typename Tfs>
    friend bool mount(Tfs& fs, const char* mountPoint);
};

extern FS SPIFFS;

template<>
bool mount<FS>(FS& fs, const char* mountPoint);

File open(const char* path, const char* mode);

Dir openDir(const char* path);

#endif //FS_H
