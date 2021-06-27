/*
 LittleFS.h - Filesystem wrapper for LittleFS on the ESP8266
 Copyright (c) 2019 Earle F. Philhower, III.  All rights reserved.

 Based heavily off of the SPIFFS equivalent code in the ESP8266 core
 "Copyright (c) 2015 Ivan Grokhotkov. All rights reserved."

 This code was influenced by NodeMCU and Sming libraries, and first version of
 Arduino wrapper written by Hristo Gochkov.

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


#ifndef __LITTLEFS_H
#define __LITTLEFS_H

#include <limits>
#include <FS.h>
#include <FSImpl.h>
#include <debug.h>
#include <flash_utils.h>
#include <flash_hal.h>

#define LFS_NAME_MAX 32
#include "../lib/littlefs/lfs.h"

using namespace fs;

namespace littlefs_impl {

class LittleFSFileImpl;
class LittleFSDirImpl;

class LittleFSConfig : public FSConfig
{
public:
    static constexpr uint32_t FSId = 0x4c495454;
    LittleFSConfig(bool autoFormat = true) : FSConfig(FSId, autoFormat) { }
};

class LittleFSImpl : public FSImpl
{
public:
    LittleFSImpl(uint32_t start, uint32_t size, uint32_t pageSize, uint32_t blockSize, uint32_t maxOpenFds)
        : _start(start) , _size(size) , _pageSize(pageSize) , _blockSize(blockSize) , _maxOpenFds(maxOpenFds),
          _mounted(false) {
        memset(&_lfs, 0, sizeof(_lfs));
        memset(&_lfs_cfg, 0, sizeof(_lfs_cfg));
        if (_size && _blockSize) {
            _lfs_cfg.context = (void*) this;
            _lfs_cfg.read = lfs_flash_read;
            _lfs_cfg.prog = lfs_flash_prog;
            _lfs_cfg.erase = lfs_flash_erase;
            _lfs_cfg.sync = lfs_flash_sync;
            _lfs_cfg.read_size = 64;
            _lfs_cfg.prog_size = 64;
            _lfs_cfg.block_size =  _blockSize;
            _lfs_cfg.block_count = _size / _blockSize;
            _lfs_cfg.block_cycles = 16; // TODO - need better explanation
            _lfs_cfg.cache_size = 64;
            _lfs_cfg.lookahead_size = 64;
            _lfs_cfg.read_buffer = nullptr;
            _lfs_cfg.prog_buffer = nullptr;
            _lfs_cfg.lookahead_buffer = nullptr;
            _lfs_cfg.name_max = 0;
            _lfs_cfg.file_max = 0;
            _lfs_cfg.attr_max = 0;
        }
    }

    ~LittleFSImpl() {
        if (_mounted) {
            lfs_unmount(&_lfs);
        }
    }

    FileImplPtr open(const char* path, OpenMode openMode, AccessMode accessMode) override;
    DirImplPtr openDir(const char *path) override;

    bool exists(const char* path) override {
        if (!_mounted || !path || !path[0]) {
            return false;
        }
        lfs_info info;
        int rc = lfs_stat(&_lfs, path, &info);
        return rc == 0;
    }

    bool rename(const char* pathFrom, const char* pathTo) override {
        if (!_mounted || !pathFrom || !pathFrom[0] || !pathTo || !pathTo[0]) {
            return false;
        }
        int rc = lfs_rename(&_lfs, pathFrom, pathTo);
        if (rc != 0) {
            DEBUGV("lfs_rename: rc=%d, from=`%s`, to=`%s`\n", rc, pathFrom, pathTo);
            return false;
        }
        return true;
    }

    bool info(FSInfo& info) override {
        if (!_mounted) {
            return false;
        }
        info.maxOpenFiles = _maxOpenFds;
        info.blockSize = _blockSize;
        info.pageSize = _pageSize;
        info.maxOpenFiles = _maxOpenFds;
        info.maxPathLength = LFS_NAME_MAX;
        info.totalBytes = _size;
        info.usedBytes = _getUsedBlocks() * _blockSize;
        return true;
    }

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

    bool remove(const char* path) override {
        if (!_mounted || !path || !path[0]) {
            return false;
        }
        int rc = lfs_remove(&_lfs, path);
        if (rc != 0) {
            DEBUGV("lfs_remove: rc=%d path=`%s`\n", rc, path);
            return false;
        }
        // Now try and remove any empty subdirs this makes, silently
        char *pathStr = strdup(path);
        if (pathStr) {
            char *ptr = strrchr(pathStr, '/');
            while (ptr) {
                *ptr = 0;
                lfs_remove(&_lfs, pathStr); // Don't care if fails if there are files left
                ptr = strrchr(pathStr, '/');
            }
            free(pathStr);
        }
        return true;
    }

    bool mkdir(const char* path) override {
        if (!_mounted || !path || !path[0]) {
            return false;
        }
        int rc = lfs_mkdir(&_lfs, path);
        return (rc==0);
    }

    bool rmdir(const char* path) override {
        return remove(path);  // Same call on LittleFS
    }

    bool setConfig(const FSConfig &cfg) override {
        if ((cfg._type != LittleFSConfig::FSId) || _mounted) {
            return false;
        }
        _cfg = *static_cast<const LittleFSConfig *>(&cfg);
       return true;
    }

    bool begin() override {
        if ((_blockSize <= 0) || (_size <= 0)) {
            DEBUGV("LittleFS size is <= zero");
            return false;
        }
        if (_tryMount()) {
            return true;
        }
        if (!_cfg._autoFormat || !format()) {
            return false;
        }
        return _tryMount();
    }

    void end() override {
        if (!_mounted) {
            return;
        }
        lfs_unmount(&_lfs);
        _mounted = false;
    }

    bool format() override {
        if ((_blockSize <= 0) || (_size <= 0)) {
            DEBUGV("lfs size is zero\n");
            return false;
        }

        bool wasMounted = _mounted;
        if (_mounted) {
            lfs_unmount(&_lfs);
            _mounted = false;
        }

        memset(&_lfs, 0, sizeof(_lfs));
        int rc = lfs_format(&_lfs, &_lfs_cfg);
        if (rc != 0) {
            DEBUGV("lfs_format: rc=%d\n", rc);
            return false;
        }

        if(_timeCallback && _tryMount()) {
            // Mounting is required to set attributes

            time_t t = _timeCallback();
            rc = lfs_setattr(&_lfs, "/", 'c', &t, 8);
            if (rc != 0) {
                DEBUGV("lfs_format, lfs_setattr 'c': rc=%d\n", rc);
                return false;
            }

            rc = lfs_setattr(&_lfs, "/", 't', &t, 8);
            if (rc != 0) {
                DEBUGV("lfs_format, lfs_setattr 't': rc=%d\n", rc);
                return false;
            }
            
            lfs_unmount(&_lfs);
            _mounted = false;
        }

        if (wasMounted) {
            return _tryMount();
        }

        return true;
    }

    time_t getCreationTime() override {
        time_t t;
        uint32_t t32b;

        if (lfs_getattr(&_lfs, "/", 'c', &t, 8) == 8) {
            return t;
        } else if (lfs_getattr(&_lfs, "/", 'c', &t32b, 4) == 4) {
            return (time_t)t32b;
        } else {
            return 0;
        }
    }


protected:
    friend class LittleFSFileImpl;
    friend class LittleFSDirImpl;

    lfs_t* getFS() {
        return &_lfs;
    }

    bool _tryMount() {
        if (_mounted) {
            lfs_unmount(&_lfs);
            _mounted = false;
        }
        memset(&_lfs, 0, sizeof(_lfs));
        int rc = lfs_mount(&_lfs, &_lfs_cfg);
        if (rc==0) {
            _mounted = true;
        }
        return _mounted;
    }

    int _getUsedBlocks() {
        if (!_mounted) {
            return 0;
        }
        return lfs_fs_size(&_lfs);
    }

    static int _getFlags(OpenMode openMode, AccessMode accessMode) {
        int mode = 0;
        if (openMode & OM_CREATE) {
            mode |= LFS_O_CREAT;
        }
        if (openMode & OM_APPEND) {
            mode |= LFS_O_APPEND;
        }
        if (openMode & OM_TRUNCATE) {
            mode |= LFS_O_TRUNC;
        }
        if (accessMode & AM_READ) {
            mode |= LFS_O_RDONLY;
        }
        if (accessMode & AM_WRITE) {
           mode |= LFS_O_WRONLY;
        }
        return mode;
    }

    // Check that no components of path beyond max len
    static bool pathValid(const char *path) {
        while (*path) {
            const char *slash = strchr(path, '/');
            if (!slash) {
                if (strlen(path) >= LFS_NAME_MAX) {
                    // Terminal filename is too long
                    return false;
                }
                break;
            }
            if ((slash - path) >= LFS_NAME_MAX) {
                // This subdir name too long
                return false;
            }
            path = slash + 1;
        }
        return true;
    }

    // The actual flash accessing routines
    static int lfs_flash_read(const struct lfs_config *c, lfs_block_t block,
                              lfs_off_t off, void *buffer, lfs_size_t size);
    static int lfs_flash_prog(const struct lfs_config *c, lfs_block_t block,
                              lfs_off_t off, const void *buffer, lfs_size_t size);
    static int lfs_flash_erase(const struct lfs_config *c, lfs_block_t block);
    static int lfs_flash_sync(const struct lfs_config *c);

    lfs_t       _lfs;
    lfs_config  _lfs_cfg;

    LittleFSConfig _cfg;

    uint32_t _start;
    uint32_t _size;
    uint32_t _pageSize;
    uint32_t _blockSize;
    uint32_t _maxOpenFds;

    bool     _mounted;
};


class LittleFSFileImpl : public FileImpl
{
public:
    LittleFSFileImpl(LittleFSImpl* fs, const char *name, std::shared_ptr<lfs_file_t> fd, int flags, time_t creation) : _fs(fs), _fd(fd), _opened(true), _flags(flags), _creation(creation) {
        _name = std::shared_ptr<char>(new char[strlen(name) + 1], std::default_delete<char[]>());
        strcpy(_name.get(), name);
    }

    ~LittleFSFileImpl() override {
        if (_opened) {
            close();
        }
    }

    size_t write(const uint8_t *buf, size_t size) override {
        if (!_opened || !_fd || !buf) {
            return 0;
        }
        int result = lfs_file_write(_fs->getFS(), _getFD(), (void*) buf, size);
        if (result < 0) {
            DEBUGV("lfs_write rc=%d\n", result);
            return 0;
        }
        return result;
    }

    int read(uint8_t* buf, size_t size) override {
        if (!_opened || !_fd | !buf) {
            return 0;
        }
        int result = lfs_file_read(_fs->getFS(), _getFD(), (void*) buf, size);
        if (result < 0) {
            DEBUGV("lfs_read rc=%d\n", result);
            return 0;
        }

        return result;
    }

    void flush() override {
        if (!_opened || !_fd) {
            return;
        }
        int rc = lfs_file_sync(_fs->getFS(), _getFD());
        if (rc < 0) {
            DEBUGV("lfs_file_sync rc=%d\n", rc);
        }
    }

    bool seek(uint32_t pos, SeekMode mode) override {
        if (!_opened || !_fd) {
            return false;
        }
        int32_t offset = static_cast<int32_t>(pos);
        if (mode == SeekEnd) {
            offset = -offset; // TODO - this seems like its plain wrong vs. POSIX
        }
        auto lastPos = position();
        int rc = lfs_file_seek(_fs->getFS(), _getFD(), offset, (int)mode); // NB. SeekMode === LFS_SEEK_TYPES
        if (rc < 0) {
            DEBUGV("lfs_file_seek rc=%d\n", rc);
            return false;
        }
        if (position() > size()) {
            seek(lastPos, SeekSet); // Pretend the seek() never happened
            return false;
        }
        return true;
    }

    size_t position() const override {
        if (!_opened || !_fd) {
            return 0;
        }
        int result = lfs_file_tell(_fs->getFS(), _getFD());
        if (result < 0) {
            DEBUGV("lfs_file_tell rc=%d\n", result);
            return 0;
        }

        return result;
    }

    size_t size() const override {
        return (_opened && _fd)? lfs_file_size(_fs->getFS(), _getFD()) : 0;
    }

    bool truncate(uint32_t size) override {
        if (!_opened || !_fd) {
            return false;
        }
        int rc = lfs_file_truncate(_fs->getFS(), _getFD(), size);
        if (rc < 0) {
            DEBUGV("lfs_file_truncate rc=%d\n", rc);
            return false;
        }
        return true;
    }

    void close() override {
        if (_opened && _fd) {
            lfs_file_close(_fs->getFS(), _getFD());
            _opened = false;
            DEBUGV("lfs_file_close: fd=%p\n", _getFD());
            if (_timeCallback && (_flags & LFS_O_WRONLY)) {
                // If the file opened with O_CREAT, write the creation time attribute
                if (_creation) {
                    int rc = lfs_setattr(_fs->getFS(), _name.get(), 'c', (const void *)&_creation, sizeof(_creation));
                    if (rc < 0) {
                        DEBUGV("Unable to set creation time on '%s' to %d\n", _name.get(), _creation);
                    }
                }
                // Add metadata with last write time
                time_t now = _timeCallback();
                int rc = lfs_setattr(_fs->getFS(), _name.get(), 't', (const void *)&now, sizeof(now));
                if (rc < 0) {
                    DEBUGV("Unable to set last write time on '%s' to %d\n", _name.get(), now);
                }
            }
        }
    }

    time_t getLastWrite() override {
        time_t ftime = 0;
        if (_opened && _fd) {
            int rc = lfs_getattr(_fs->getFS(), _name.get(), 't', (void *)&ftime, sizeof(ftime));
            if (rc != sizeof(ftime))
                ftime = 0; // Error, so clear read value
        }
        return ftime;
    }

    time_t getCreationTime() override {
        time_t ftime = 0;
        if (_opened && _fd) {
            int rc = lfs_getattr(_fs->getFS(), _name.get(), 'c', (void *)&ftime, sizeof(ftime));
            if (rc != sizeof(ftime))
                ftime = 0; // Error, so clear read value
        }
        return ftime;
    }

    const char* name() const override {
        if (!_opened) {
            return nullptr;
        } else {
            const char *p = _name.get();
            const char *slash = strrchr(p, '/');
            return (slash && slash[1]) ? slash + 1 : p;
        }
    }

    const char* fullName() const override {
        return _opened ? _name.get() : nullptr;
    }

    bool isFile() const override {
        if (!_opened || !_fd) {
            return false;
        }
        lfs_info info;
        int rc = lfs_stat(_fs->getFS(), fullName(), &info);
        return (rc == 0) && (info.type == LFS_TYPE_REG);
    }

    bool isDirectory() const override {
        if (!_opened) {
            return false;
        } else if (!_fd) {
            return true;
        }
        lfs_info info;
        int rc = lfs_stat(_fs->getFS(), fullName(), &info);
        return (rc == 0) && (info.type == LFS_TYPE_DIR);
    }

protected:
    lfs_file_t *_getFD() const {
        return _fd.get();
    }

    LittleFSImpl                *_fs;
    std::shared_ptr<lfs_file_t>  _fd;
    std::shared_ptr<char>        _name;
    bool                         _opened;
    int                          _flags;
    time_t                       _creation;
};

class LittleFSDirImpl : public DirImpl
{
public:
    LittleFSDirImpl(const String& pattern, LittleFSImpl* fs, std::shared_ptr<lfs_dir_t> dir, const char *dirPath = nullptr)
        : _pattern(pattern) , _fs(fs) , _dir(dir) , _dirPath(nullptr), _valid(false), _opened(true)
    {
        memset(&_dirent, 0, sizeof(_dirent));
        if (dirPath) {
            _dirPath = std::shared_ptr<char>(new char[strlen(dirPath) + 1], std::default_delete<char[]>());
            strcpy(_dirPath.get(), dirPath);
        }
    }

    ~LittleFSDirImpl() override {
        if (_opened) {
            lfs_dir_close(_fs->getFS(), _getDir());
        }
    }

    FileImplPtr openFile(OpenMode openMode, AccessMode accessMode) override {
        if (!_valid) {
            return FileImplPtr();
        }
        int nameLen = 3; // Slashes, terminator
        nameLen += _dirPath.get() ? strlen(_dirPath.get()) : 0;
        nameLen += strlen(_dirent.name);
        char tmpName[nameLen];
        snprintf(tmpName, nameLen, "%s%s%s", _dirPath.get() ? _dirPath.get() : "", _dirPath.get()&&_dirPath.get()[0]?"/":"", _dirent.name);
        auto ret = _fs->open((const char *)tmpName, openMode, accessMode);
        return ret;
    }

    const char* fileName() override {
        if (!_valid) {
            return nullptr;
        }
        return (const char*) _dirent.name;
    }

    size_t fileSize() override {
        if (!_valid) {
            return 0;
        }
        return _dirent.size;
    }

    time_t fileTime() override {
        time_t t;
        int32_t t32b;

        // If the attribute is 8-bytes, we're all set
        if (_getAttr('t', 8, &t)) {
            return t;
        } else if (_getAttr('t', 4, &t32b)) {
            // If it's 4 bytes silently promote to 64b
            return (time_t)t32b;
        } else {
            // OTW, none present
            return 0;
        }
    }

    time_t fileCreationTime() override {
        time_t t;
        int32_t t32b;

        // If the attribute is 8-bytes, we're all set
        if (_getAttr('c', 8, &t)) {
            return t;
        } else if (_getAttr('c', 4, &t32b)) {
            // If it's 4 bytes silently promote to 64b
            return (time_t)t32b;
        } else {
            // OTW, none present
            return 0;
        }
    }


    bool isFile() const override {
        return _valid && (_dirent.type == LFS_TYPE_REG);
    }

    bool isDirectory() const override {
        return _valid && (_dirent.type == LFS_TYPE_DIR);
    }

    bool rewind() override {
        _valid = false;
        int rc = lfs_dir_rewind(_fs->getFS(), _getDir());
        // Skip the . and .. entries
        lfs_info dirent;
        lfs_dir_read(_fs->getFS(), _getDir(), &dirent);
        lfs_dir_read(_fs->getFS(), _getDir(), &dirent);
        return (rc == 0);
    }

    bool next() override {
        const int n = _pattern.length();
        bool match;
        do {
            _dirent.name[0] = 0;
            int rc = lfs_dir_read(_fs->getFS(), _getDir(), &_dirent);
            _valid = (rc == 1);
            match = (!n || !strncmp((const char*) _dirent.name, _pattern.c_str(), n));
        } while (_valid && !match);
        return _valid;
    }

protected:
    lfs_dir_t *_getDir() const {
        return _dir.get();
    }

    bool _getAttr(char attr, int len, void *dest) {
        if (!_valid || !len || !dest) {
            return false;
        }
        int nameLen = 3; // Slashes, terminator
        nameLen += _dirPath.get() ? strlen(_dirPath.get()) : 0;
        nameLen += strlen(_dirent.name);
        char tmpName[nameLen];
        snprintf(tmpName, nameLen, "%s%s%s", _dirPath.get() ? _dirPath.get() : "", _dirPath.get()&&_dirPath.get()[0]?"/":"", _dirent.name);
        int rc = lfs_getattr(_fs->getFS(), tmpName, attr, dest, len);
        return (rc == len);
    }

    String                      _pattern;
    LittleFSImpl               *_fs;
    std::shared_ptr<lfs_dir_t>  _dir;
    std::shared_ptr<char>       _dirPath;
    lfs_info                    _dirent;
    bool                        _valid;
    bool                        _opened;
};

};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LITTLEFS)
extern FS LittleFS;
using littlefs_impl::LittleFSConfig;
#endif // ARDUINO


#endif // !defined(__LITTLEFS_H)
