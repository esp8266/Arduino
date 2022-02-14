#ifndef SDFS_H
#define SDFS_H

/*
 SDFS.h - file system wrapper for SdLib
 Copyright (c) 2019 Earle F. Philhower, III.  All rights reserved.

 Based on spiffs_api.h, which is:
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
#include <limits>
#include <assert.h>
#include <FSImpl.h>
#include "debug.h"
#include <SPI.h>
#include <SdFat.h>
#include <FS.h>

namespace sdfs {

class SDFSFileImpl;
class SDFSDirImpl;
class SDFSConfig : public fs::FSConfig
{
public:
    static constexpr uint32_t FSId = 0x53444653;

    SDFSConfig(uint8_t csPin = 4, uint32_t spi = SD_SCK_MHZ(10)) : FSConfig(FSId, false), _csPin(csPin), _part(0), _spiSettings(spi)  { }

    SDFSConfig setAutoFormat(bool val = true) {
        _autoFormat = val;
        return *this;
    }
    SDFSConfig setCSPin(uint8_t pin) {
        _csPin = pin;
        return *this;
    }
    SDFSConfig setSPI(uint32_t spi) {
        _spiSettings = spi;
        return *this;
    }
    SDFSConfig setPart(uint8_t part) {
        _part = part;
        return *this;
    }

    // Inherit _type and _autoFormat
    uint8_t   _csPin;
    uint8_t   _part;
    uint32_t  _spiSettings;
};

class SDFSImpl : public fs::FSImpl
{
public:
    SDFSImpl() : _mounted(false)
    {
    }

    fs::FileImplPtr open(const char* path, fs::OpenMode openMode, fs::AccessMode accessMode) override;

    bool exists(const char* path) override {
        return _mounted ? _fs.exists(path) : false;
    }

    fs::DirImplPtr openDir(const char* path) override;

    bool rename(const char* pathFrom, const char* pathTo) override {
        return _mounted ? _fs.rename(pathFrom, pathTo) : false;
    }

    bool info64(fs::FSInfo64& info) override {
        if (!_mounted) {
            DEBUGV("SDFS::info: FS not mounted\n");
            return false;
        }
        info.maxOpenFiles = 999; // TODO - not valid
        info.blockSize = _fs.vol()->bytesPerCluster();
        info.pageSize = 0; // TODO ?
        info.maxPathLength = 255; // TODO ?
        info.totalBytes =_fs.vol()->clusterCount() * info.blockSize;
        info.usedBytes = (_fs.vol()->clusterCount() - _fs.vol()->freeClusterCount()) * info.blockSize;
        return true;
    }

    bool info(fs::FSInfo& info) override {
        fs::FSInfo64 i;
        if (!info64(i)) {
            return false;
        }
        info.blockSize     = i.blockSize;
        info.pageSize      = i.pageSize;
        info.maxOpenFiles  = i.maxOpenFiles;
        info.maxPathLength = i.maxPathLength;
#ifdef DEBUG_ESP_PORT
        if (i.totalBytes > std::numeric_limits<uint32_t>::max()) {
            // This catches both total and used cases, since used must always be < total.
            DEBUG_ESP_PORT.printf_P(PSTR("WARNING: SD card size overflow (%lld >= 4GB).  Please update source to use info64().\n"), (long long)i.totalBytes);
        }
#endif
        info.totalBytes    = (size_t)i.totalBytes;
        info.usedBytes     = (size_t)i.usedBytes;
        return true;
    }

    bool remove(const char* path) override {
        return _mounted ? _fs.remove(path) : false;
    }

    bool mkdir(const char* path) override {
        return _mounted ? _fs.mkdir(path) : false;
    }

    bool rmdir(const char* path) override {
        return _mounted ?_fs.rmdir(path) : false;
    }

    bool setConfig(const fs::FSConfig &cfg) override
    {
        if ((cfg._type != SDFSConfig::FSId) || _mounted) {
            DEBUGV("SDFS::setConfig: invalid config or already mounted\n");
            return false;
        }
	_cfg = *static_cast<const SDFSConfig *>(&cfg);
        return true;
    }

    bool begin() override {
        if (_mounted) {
            return true;
        }
        _mounted = _fs.begin(_cfg._csPin, _cfg._spiSettings);
        if (!_mounted && _cfg._autoFormat) {
            format();
            _mounted = _fs.begin(_cfg._csPin, _cfg._spiSettings);
        }
	FsDateTime::setCallback(dateTimeCB);
        return _mounted;
    }

    void end() override {
        _mounted = false;
        // TODO
    }

    bool format() override;

    // The following are not common FS interfaces, but are needed only to
    // support the older SD.h exports
    uint8_t type() {
        return _fs.card()->type();
    }
    uint8_t fatType() {
        return _fs.vol()->fatType();
    }
    size_t blocksPerCluster() {
        return _fs.vol()->sectorsPerCluster();
    }
    size_t totalClusters() {
        return _fs.vol()->clusterCount();
    }
    size_t totalBlocks() {
        return (totalClusters() / blocksPerCluster());
    }
    size_t clusterSize() {
        return _fs.vol()->bytesPerCluster();
    }
    size_t size() {
        return (clusterSize() * totalClusters());
    }

    // Helper function, takes FAT and makes standard time_t
    static time_t FatToTimeT(uint16_t d, uint16_t t) {
        struct tm tiempo;
        memset(&tiempo, 0, sizeof(tiempo));
        tiempo.tm_sec  = (((int)t) <<  1) & 0x3e;
        tiempo.tm_min  = (((int)t) >>  5) & 0x3f;
        tiempo.tm_hour = (((int)t) >> 11) & 0x1f;
        tiempo.tm_mday = (int)(d & 0x1f);
        tiempo.tm_mon  = ((int)(d >> 5) & 0x0f) - 1;
        tiempo.tm_year = ((int)(d >> 9) & 0x7f) + 80;
        tiempo.tm_isdst = -1;
        return mktime(&tiempo);
    }

    virtual void setTimeCallback(time_t (*cb)(void)) override {
        extern time_t (*__sdfs_timeCallback)(void);
        __sdfs_timeCallback = cb;
    }

    // Because SdFat has a single, global setting for this we can only use a
    // static member of our class to return the time/date.
    static void dateTimeCB(uint16_t *dosYear, uint16_t *dosTime) {
        time_t now;
	extern time_t (*__sdfs_timeCallback)(void);
        if (__sdfs_timeCallback) {
            now = __sdfs_timeCallback();
        } else {
            now = time(nullptr);
        }
        struct tm *tiempo = localtime(&now);
        *dosYear = ((tiempo->tm_year - 80) << 9) | ((tiempo->tm_mon + 1) << 5) | tiempo->tm_mday;
        *dosTime = (tiempo->tm_hour << 11) | (tiempo->tm_min << 5) | tiempo->tm_sec;
    }

protected:
    friend class SDFileImpl;
    friend class SDFSDirImpl;

    SdFat* getFs() {
        return &_fs;
    }


    static uint8_t _getFlags(fs::OpenMode openMode, fs::AccessMode accessMode) {
        uint8_t mode = 0;
        if (openMode & fs::OM_CREATE) {
            mode |= fs::O_CREAT;
        }
        if (openMode & fs::OM_APPEND) {
            mode |= fs::O_AT_END;
        }
        if (openMode & fs::OM_TRUNCATE) {
            mode |= fs::O_TRUNC;
        }
        if ((accessMode & (fs::AM_READ | fs::AM_WRITE)) == (fs::AM_READ | fs::AM_WRITE)) {
            mode |= fs::O_RDWR;
        } else if (accessMode & fs::AM_READ) {
            mode |= fs::O_READ;
        } else if (accessMode & fs::AM_WRITE) {
            mode |= fs::O_WRITE;
        }
        return mode;
    }

    SdFat        _fs;
    SDFSConfig   _cfg;
    bool         _mounted;
};


class SDFSFileImpl : public fs::FileImpl
{
public:
    SDFSFileImpl(SDFSImpl *fs, std::shared_ptr<File32> fd, const char *name)
        : _fs(fs), _fd(fd), _opened(true)
    {
        _name = std::shared_ptr<char>(new char[strlen(name) + 1], std::default_delete<char[]>());
        strcpy(_name.get(), name);
    }

    ~SDFSFileImpl() override
    {
        flush();
        close();
    }

    int availableForWrite() override
    {
        return _opened ? _fd->availableSpaceForWrite() : 0;
    }

    size_t write(const uint8_t *buf, size_t size) override
    {
        return _opened ? _fd->write(buf, size) : -1;
    }

    int read(uint8_t* buf, size_t size) override
    {
        return _opened ? _fd->read(buf, size) : -1;
    }

    void flush() override
    {
        if (_opened) {
            _fd->sync();
        }
    }

    bool seek(uint32_t pos, fs::SeekMode mode) override
    {
        if (!_opened) {
            return false;
        }
        switch (mode) {
            case fs::SeekSet:
                return _fd->seekSet(pos);
            case fs::SeekEnd:
                return _fd->seekEnd(-pos); // TODO again, odd from POSIX
            case fs::SeekCur:
                return _fd->seekCur(pos);
            default:
                // Should not be hit, we've got an invalid seek mode
                DEBUGV("SDFSFileImpl::seek: invalid seek mode %d\n", mode);
		assert((mode==fs::SeekSet) || (mode==fs::SeekEnd) || (mode==fs::SeekCur)); // Will fail and give meaningful assert message
		return false;
        }
    }

    size_t position() const override
    {
        return _opened ? _fd->curPosition() : 0;
    }

    size_t size() const override
    {
        return _opened ? _fd->fileSize() : 0;
    }

    bool truncate(uint32_t size) override
    {
        if (!_opened) {
            DEBUGV("SDFSFileImpl::truncate: file not opened\n");
            return false;
        }
        return _fd->truncate(size);
    }

    void close() override
    {
        if (_opened) {
            _fd->close();
            _opened = false;
        }
    }

    const char* name() const override
    {
        if (!_opened) {
            DEBUGV("SDFSFileImpl::name: file not opened\n");
            return nullptr;
        } else {
            const char *p = _name.get();
            const char *slash = strrchr(p, '/');
            // For names w/o any path elements, return directly
            // If there are slashes, return name after the last slash
            // (note that strrchr will return the address of the slash,
            // so need to increment to ckip it)
            return (slash && slash[1]) ? slash + 1 : p;
        }
    }

    const char* fullName() const override
    {
        return _opened ? _name.get() : nullptr;
    }

    bool isFile() const override
    {
        return _opened ? _fd->isFile() : false;;
    }

    bool isDirectory() const override
    {
        return _opened ? _fd->isDir() : false;
    }

    time_t getLastWrite() override {
        time_t ftime = 0;
        if (_opened && _fd) {
            DirFat_t tmp;
            if (_fd.get()->dirEntry(&tmp)) {
                ftime = SDFSImpl::FatToTimeT(*(uint16_t*)tmp.modifyDate, *(uint16_t*)tmp.modifyTime);
            }
        }
        return ftime;
    }

    time_t getCreationTime() override {
        time_t ftime = 0;
        if (_opened && _fd) {
            DirFat_t tmp;
            if (_fd.get()->dirEntry(&tmp)) {
                ftime = SDFSImpl::FatToTimeT(*(uint16_t*)tmp.createDate, *(uint16_t*)tmp.createTime);
            }
        }
        return ftime;
    }

protected:
    SDFSImpl*                _fs;
    std::shared_ptr<File32>  _fd;
    std::shared_ptr<char>    _name;
    bool                     _opened;
};

class SDFSDirImpl : public fs::DirImpl
{
public:
    SDFSDirImpl(const String& pattern, SDFSImpl* fs, std::shared_ptr<File32> dir, const char *dirPath = nullptr)
        : _pattern(pattern), _fs(fs), _dir(dir), _valid(false), _dirPath(nullptr)
    {
        if (dirPath) {
            _dirPath = std::shared_ptr<char>(new char[strlen(dirPath) + 1], std::default_delete<char[]>());
            strcpy(_dirPath.get(), dirPath);
        }
    }

    ~SDFSDirImpl() override
    {
        _dir->close();
    }

    fs::FileImplPtr openFile(fs::OpenMode openMode, fs::AccessMode accessMode) override
    {
        if (!_valid) {
            return fs::FileImplPtr();
        }
        // MAX_PATH on FAT32 is potentially 260 bytes per most implementations
        char tmpName[260];
        snprintf(tmpName, sizeof(tmpName), "%s%s%s", _dirPath.get() ? _dirPath.get() : "", _dirPath.get()&&_dirPath.get()[0]?"/":"", _lfn);
        return _fs->open((const char *)tmpName, openMode, accessMode);
    }

    const char* fileName() override
    {
        if (!_valid) {
            DEBUGV("SDFSDirImpl::fileName: directory not valid\n");
            return nullptr;
        }
        return (const char*) _lfn; //_dirent.name;
    }

    size_t fileSize() override
    {
        if (!_valid) {
            return 0;
        }

        return _size;
    }

    time_t fileTime() override
    {
        if (!_valid) {
            return 0;
        }

        return _time;
    }

    time_t fileCreationTime() override
    {
        if (!_valid) {
            return 0;
        }

        return _creation;
    }

    bool isFile() const override
    {
        return _valid ? _isFile : false;
    }

    bool isDirectory() const override
    {
        return _valid ? _isDirectory : false;
    }

    bool next() override
    {
        const int n = _pattern.length();
        do {
            File32 file;
            file.openNext(_dir.get(), O_READ);
            if (file) {
                _valid = 1;
                _size = file.fileSize();
                _isFile = file.isFile();
                _isDirectory = file.isDir();
                DirFat_t tmp;
                if (file.dirEntry(&tmp)) {
                    _time = SDFSImpl::FatToTimeT(*(uint16_t*)tmp.modifyDate, *(uint16_t*)tmp.modifyTime);
                    _creation = SDFSImpl::FatToTimeT(*(uint16_t*)tmp.createDate, *(uint16_t*)tmp.createTime);
		} else {
                    _time = 0;
                    _creation = 0;
               }
                file.getName(_lfn, sizeof(_lfn));
                file.close();
            } else {
                _valid = 0;
            }
        } while(_valid && strncmp((const char*) _lfn, _pattern.c_str(), n) != 0);
        return _valid;
    }

    bool rewind() override
    {
        _valid = false;
        _dir->rewind();
        return true;
    }

protected:
    String                  _pattern;
    SDFSImpl*               _fs;
    std::shared_ptr<File32> _dir;
    bool                    _valid;
    char                    _lfn[64];
    time_t                  _time;
    time_t                  _creation;
    std::shared_ptr<char>   _dirPath;
    uint32_t                _size;
    bool                    _isFile;
    bool                    _isDirectory;
};

}; // namespace sdfs

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SDFS)
extern fs::FS SDFS;
using sdfs::SDFSConfig;
#endif

#endif // SDFS.h
