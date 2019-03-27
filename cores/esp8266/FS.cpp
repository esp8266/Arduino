/*
 FS.cpp - file system wrapper
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

#include "FS.h"
#include "FSImpl.h"

using namespace fs;

static bool sflags(const char* mode, OpenMode& om, AccessMode& am);

size_t File::write(uint8_t c) {
    if (!_p)
        return 0;

    return _p->write(&c, 1);
}

size_t File::write(const uint8_t *buf, size_t size) {
    if (!_p)
        return 0;

    return _p->write(buf, size);
}

int File::available() {
    if (!_p)
        return false;

    return _p->size() - _p->position();
}

int File::read() {
    if (!_p)
        return -1;

    uint8_t result;
    if (_p->read(&result, 1) != 1) {
        return -1;
    }

    return result;
}

int File::read(uint8_t* buf, size_t size) {
    if (!_p)
        return -1;

    return _p->read(buf, size);
}

int File::peek() {
    if (!_p)
        return -1;

    size_t curPos = _p->position();
    int result = read();
    seek(curPos, SeekSet);
    return result;
}

void File::flush() {
    if (!_p)
        return;

    _p->flush();
}

bool File::seek(uint32_t pos, SeekMode mode) {
    if (!_p)
        return false;

    return _p->seek(pos, mode);
}

size_t File::position() const {
    if (!_p)
        return 0;

    return _p->position();
}

size_t File::size() const {
    if (!_p)
        return 0;

    return _p->size();
}

void File::close() {
    if (_p) {
        _p->close();
        _p = nullptr;
    }
}

File::operator bool() const {
    return !!_p;
}

bool File::truncate(uint32_t size) {
    if (!_p)
        return false;

    return _p->truncate(size);
}

const char* File::name() const {
    if (!_p)
        return nullptr;

    return _p->name();
}

const char* File::fullName() const {
    if (!_p)
        return nullptr;

    return _p->fullName();
}

bool File::isFile() const {
    if (!_p)
        return false;

    return _p->isFile();
}

bool File::isDirectory() const {
    if (!_p)
        return false;

    return _p->isDirectory();
}

void File::rewindDirectory() {
    if (!_fakeDir) {
        _fakeDir = std::make_shared<Dir>(_baseFS->openDir(fullName()));
    } else {
        _fakeDir->rewind();
   }
}

File File::openNextFile() {
    if (!_fakeDir) {
        _fakeDir = std::make_shared<Dir>(_baseFS->openDir(fullName()));
    }
    _fakeDir->next();
    return _fakeDir->openFile("r");
}

String File::readString()
{
    String ret;
    ret.reserve(size() - position());
    char temp[256+1];
    int countRead = readBytes(temp, sizeof(temp)-1);
    while (countRead > 0)
    {
        temp[countRead] = 0;
        ret += temp;
        countRead = readBytes(temp, sizeof(temp)-1);
    }
    return ret;
}

File Dir::openFile(const char* mode) {
    if (!_impl) {
        return File();
    }

    OpenMode om;
    AccessMode am;
    if (!sflags(mode, om, am)) {
        DEBUGV("Dir::openFile: invalid mode `%s`\r\n", mode);
        return File();
    }

    return File(_impl->openFile(om, am), _baseFS);
}

String Dir::fileName() {
    if (!_impl) {
        return String();
    }

    return _impl->fileName();
}

size_t Dir::fileSize() {
    if (!_impl) {
        return 0;
    }

    return _impl->fileSize();
}

bool Dir::isFile() const {
    if (!_impl)
        return false;

    return _impl->isFile();
}

bool Dir::isDirectory() const {
    if (!_impl)
        return false;

    return _impl->isDirectory();
}

bool Dir::next() {
    if (!_impl) {
        return false;
    }

    return _impl->next();
}

bool Dir::rewind() {
    if (!_impl) {
        return false;
    }

    return _impl->rewind();
}

bool FS::setConfig(const FSConfig &cfg) {
    if (!_impl) {
        return false;
    }

    return _impl->setConfig(cfg);
}

bool FS::begin() {
    if (!_impl) {
        return false;
    }
    return _impl->begin();
}

void FS::end() {
    if (_impl) {
        _impl->end();
    }
}

bool FS::format() {
    if (!_impl) {
        return false;
    }
    return _impl->format();
}

bool FS::info(FSInfo& info){
    if (!_impl) {
        return false;
    }
    return _impl->info(info);
}

File FS::open(const String& path, const char* mode) {
    return open(path.c_str(), mode);
}

File FS::open(const char* path, const char* mode) {
    if (!_impl) {
        return File();
    }

    OpenMode om;
    AccessMode am;
    if (!sflags(mode, om, am)) {
        DEBUGV("FS::open: invalid mode `%s`\r\n", mode);
        return File();
    }
    return File(_impl->open(path, om, am), this);
}

bool FS::exists(const char* path) {
    if (!_impl) {
        return false;
    }
    return _impl->exists(path);
}

bool FS::exists(const String& path) {
    return exists(path.c_str());
}

Dir FS::openDir(const char* path) {
    if (!_impl) {
        return Dir();
    }
    DirImplPtr p = _impl->openDir(path);
    return Dir(p, this);
}

Dir FS::openDir(const String& path) {
    return openDir(path.c_str());
}

bool FS::remove(const char* path) {
    if (!_impl) {
        return false;
    }
    return _impl->remove(path);
}

bool FS::remove(const String& path) {
    return remove(path.c_str());
}

bool FS::rmdir(const char* path) {
    if (!_impl) {
        return false;
    }
    return _impl->rmdir(path);
}

bool FS::rmdir(const String& path) {
    return rmdir(path.c_str());
}

bool FS::mkdir(const char* path) {
    if (!_impl) {
        return false;
    }
    return _impl->mkdir(path);
}

bool FS::mkdir(const String& path) {
    return mkdir(path.c_str());
}

bool FS::rename(const char* pathFrom, const char* pathTo) {
    if (!_impl) {
        return false;
    }
    return _impl->rename(pathFrom, pathTo);
}

bool FS::rename(const String& pathFrom, const String& pathTo) {
    return rename(pathFrom.c_str(), pathTo.c_str());
}



static bool sflags(const char* mode, OpenMode& om, AccessMode& am) {
    switch (mode[0]) {
        case 'r':
            am = AM_READ;
            om = OM_DEFAULT;
            break;
        case 'w':
            am = AM_WRITE;
            om = (OpenMode) (OM_CREATE | OM_TRUNCATE);
            break;
        case 'a':
            am = AM_WRITE;
            om = (OpenMode) (OM_CREATE | OM_APPEND);
            break;
        default:
            return false;
    }
    switch(mode[1]) {
        case '+':
            am = (AccessMode) (AM_WRITE | AM_READ);
            break;
        case 0:
            break;
        default:
            return false;
    }
    return true;
}


#if defined(FS_FREESTANDING_FUNCTIONS)

/*
TODO: move these functions to public API:
*/
File open(const char* path, const char* mode);
File open(String& path, const char* mode);

Dir openDir(const char* path);
Dir openDir(String& path);

template<>
bool mount<FS>(FS& fs, const char* mountPoint);
/*
*/


struct MountEntry {
    FSImplPtr fs;
    String    path;
    MountEntry* next;
};

static MountEntry* s_mounted = nullptr;

template<>
bool mount<FS>(FS& fs, const char* mountPoint) {
    FSImplPtr p = fs._impl;
    if (!p || !p->mount()) {
        DEBUGV("FSImpl mount failed\r\n");
        return false;
    }

    !make sure mountPoint has trailing '/' here

    MountEntry* entry = new MountEntry;
    entry->fs = p;
    entry->path = mountPoint;
    entry->next = s_mounted;
    s_mounted = entry;
    return true;
}


/*
    iterate over MountEntries and look for the ones which match the path
*/
File open(const char* path, const char* mode) {
    OpenMode om;
    AccessMode am;
    if (!sflags(mode, om, am)) {
        DEBUGV("open: invalid mode `%s`\r\n", mode);
        return File();
    }

    for (MountEntry* entry = s_mounted; entry; entry = entry->next) {
        size_t offset = entry->path.length();
        if (strstr(path, entry->path.c_str())) {
            File result = entry->fs->open(path + offset);
            if (result)
                return result;
        }
    }

    return File();
}

File open(const String& path, const char* mode) {
    return FS::open(path.c_str(), mode);
}

Dir openDir(const String& path) {
    return openDir(path.c_str());
}
#endif
