/*
 spiffs_api.cpp - file system wrapper for SPIFFS
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.

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

#include "FS.h"
#undef max
#undef min
#include "FSImpl.h"
#include "spiffs/spiffs.h"
#include "debug.h"
#include <limits>

extern "C" {
#include "c_types.h"
#include "spi_flash.h"
}

using namespace fs;

extern int32_t spiffs_hal_write(uint32_t addr, uint32_t size, uint8_t *src);
extern int32_t spiffs_hal_erase(uint32_t addr, uint32_t size);
extern int32_t spiffs_hal_read(uint32_t addr, uint32_t size, uint8_t *dst);

int getSpiffsMode(OpenMode openMode, AccessMode accessMode);

class SPIFFSFileImpl;
class SPIFFSDirImpl;

class SPIFFSImpl : public FSImpl {
public:
    SPIFFSImpl(uint32_t start, uint32_t size, uint32_t pageSize, uint32_t blockSize, uint32_t maxOpenFds)
    : _fs({0})
    , _start(start)
    , _size(size)
    , _pageSize(pageSize)
    , _blockSize(blockSize)
    , _maxOpenFds(maxOpenFds)
    {
    }

    FileImplPtr open(const char* path, OpenMode openMode, AccessMode accessMode) override;
    bool exists(const char* path) override;
    DirImplPtr openDir(const char* path) override;

    bool rename(const char* pathFrom, const char* pathTo) override {
        auto rc = SPIFFS_rename(&_fs, pathFrom, pathTo);
        if (rc != SPIFFS_OK) {
            DEBUGV("SPIFFS_rename: rc=%d, from=`%s`, to=`%s`\r\n", rc,
                pathFrom, pathTo);
            return false;
        }
        return true;
    }

    bool info(uint32_t *total, uint32_t *used) override{
    auto rc = SPIFFS_info(&_fs, total, used);
     if (rc != SPIFFS_OK) {
            DEBUGV("SPIFFS_format: rc=%d, err=%d\r\n", rc, _fs.err_code);
            return false;
        }
        return true;
    }

    bool remove(const char* path) override {
        auto rc = SPIFFS_remove(&_fs, path);
        if (rc != SPIFFS_OK) {
            DEBUGV("SPIFFS_remove: rc=%d path=`%s`\r\n", rc, path);
            return false;
        }
        return true;
    }

    bool begin() override {
        if (SPIFFS_mounted(&_fs) != 0) {
            return true;
        }
        if (_size == 0) {
            DEBUGV("SPIFFS size is zero");
            return false;
        }
        if (_tryMount()) {
            return true;
        }
        auto rc = SPIFFS_format(&_fs);
        if (rc != SPIFFS_OK) {
            DEBUGV("SPIFFS_format: rc=%d, err=%d\r\n", rc, _fs.err_code);
            return false;
        }
        return _tryMount();
    }

    bool format() override {
        if (_size == 0) {
            DEBUGV("SPIFFS size is zero");
            return false;
        }

        bool wasMounted = (SPIFFS_mounted(&_fs) != 0);

        if (_tryMount()) {
            SPIFFS_unmount(&_fs);
        }
        auto rc = SPIFFS_format(&_fs);
        if (rc != SPIFFS_OK) {
            DEBUGV("SPIFFS_format: rc=%d, err=%d\r\n", rc, _fs.err_code);
            return false;
        }

        if (wasMounted) {
            return _tryMount();
        }

        return true;
    }

protected:
    friend class SPIFFSFileImpl;
    friend class SPIFFSDirImpl;

    spiffs* getFs() {
        return &_fs;
    }

    bool _tryMount() {
        spiffs_config config = {0};

        config.hal_read_f       = &spiffs_hal_read;
        config.hal_write_f      = &spiffs_hal_write;
        config.hal_erase_f      = &spiffs_hal_erase;
        config.phys_size        = _size;
        config.phys_addr        = _start;
        config.phys_erase_block = SPI_FLASH_SEC_SIZE;
        config.log_block_size   = _blockSize;
        config.log_page_size    = _pageSize;


        if (((uint32_t) std::numeric_limits<spiffs_block_ix>::max()) < (_size / _blockSize)) {
            DEBUGV("spiffs_block_ix type too small");
            abort();
        }

        if (((uint32_t) std::numeric_limits<spiffs_page_ix>::max()) < (_size / _pageSize)) {
            DEBUGV("spiffs_page_ix type too small");
            abort();
        }

        if (((uint32_t) std::numeric_limits<spiffs_obj_id>::max()) < (2 + (_size / (2*_pageSize))*2)) {
            DEBUGV("spiffs_obj_id type too small");
            abort();
        }

        if (((uint32_t) std::numeric_limits<spiffs_span_ix>::max()) < (_size / _pageSize - 1)) {
            DEBUGV("spiffs_span_ix type too small");
            abort();
        }

        // hack: even though fs is not initialized at this point,
        // SPIFFS_buffer_bytes_for_cache uses only fs->config.log_page_size
        // suggestion: change SPIFFS_buffer_bytes_for_cache to take
        // spiffs_config* instead of spiffs* as an argument
        _fs.cfg.log_page_size = config.log_page_size;

        size_t workBufSize = 2 * _pageSize;
        size_t fdsBufSize = SPIFFS_buffer_bytes_for_filedescs(&_fs, _maxOpenFds);
        size_t cacheBufSize = SPIFFS_buffer_bytes_for_cache(&_fs, _maxOpenFds);

        if (!_workBuf) {
            DEBUGV("SPIFFSImpl: allocating %d+%d+%d=%d bytes\r\n",
                workBufSize, fdsBufSize, cacheBufSize,
                workBufSize + fdsBufSize + cacheBufSize);
            _workBuf.reset(new uint8_t[workBufSize]);
            _fdsBuf.reset(new uint8_t[fdsBufSize]);
            _cacheBuf.reset(new uint8_t[cacheBufSize]);
        }

        DEBUGV("SPIFFSImpl: mounting fs @%x, size=%x, block=%x, page=%x\r\n",
            _start, _size, _blockSize, _pageSize);

        auto err = SPIFFS_mount(&_fs, &config, _workBuf.get(),
            _fdsBuf.get(), fdsBufSize, _cacheBuf.get(), cacheBufSize,
            &SPIFFSImpl::_check_cb);

        DEBUGV("SPIFFSImpl: mount rc=%d\r\n", err);

        return err == SPIFFS_OK;
    }

    static void _check_cb(spiffs_check_type type, spiffs_check_report report,
                          uint32_t arg1, uint32_t arg2) {
        // TODO: spiffs doesn't pass any context pointer along with _check_cb,
        // so we can't do anything useful here other than perhaps
        // feeding the watchdog
    }

    spiffs _fs;

    uint32_t _start;
    uint32_t _size;
    uint32_t _pageSize;
    uint32_t _blockSize;
    uint32_t _maxOpenFds;

    std::unique_ptr<uint8_t[]> _workBuf;
    std::unique_ptr<uint8_t[]> _fdsBuf;
    std::unique_ptr<uint8_t[]> _cacheBuf;
};

#define CHECKFD() while (_fd == 0) { DEBUGV("SPIFFSFileImpl(%d) _fd == 0\r\n", __LINE__); abort(); }

class SPIFFSFileImpl : public FileImpl {
public:
    SPIFFSFileImpl(SPIFFSImpl* fs, spiffs_file fd)
    : _fs(fs)
    , _fd(fd)
    , _stat({0})
    , _written(false)
    {
        _getStat();
    }

    ~SPIFFSFileImpl() override {
        close();
    }

    size_t write(const uint8_t *buf, size_t size) override {
        CHECKFD();

        auto result = SPIFFS_write(_fs->getFs(), _fd, (void*) buf, size);
        if (result < 0) {
            DEBUGV("SPIFFS_write rc=%d\r\n", result);
            return 0;
        }
        _written = true;
        return result;
    }

    size_t read(uint8_t* buf, size_t size) override {
        CHECKFD();

        auto result = SPIFFS_read(_fs->getFs(), _fd, (void*) buf, size);
        if (result < 0) {
            DEBUGV("SPIFFS_read rc=%d\r\n", result);
            return 0;
        }

        return result;
    }

    void flush() override {
        CHECKFD();

        auto rc = SPIFFS_fflush(_fs->getFs(), _fd);
        if (rc < 0) {
            DEBUGV("SPIFFS_fflush rc=%d\r\n", rc);
        }
        _written = true;
    }

    bool seek(uint32_t pos, SeekMode mode) override {
        CHECKFD();

        int32_t offset = static_cast<int32_t>(pos);
        if (mode == SeekEnd) {
            offset = -offset;
        }
        auto rc = SPIFFS_lseek(_fs->getFs(), _fd, pos, (int) mode);
        if (rc < 0) {
            DEBUGV("SPIFFS_lseek rc=%d\r\n", rc);
            return false;
        }

        return true;
    }

    size_t position() const override {
        CHECKFD();

        auto result = SPIFFS_lseek(_fs->getFs(), _fd, 0, SPIFFS_SEEK_CUR);
        if (result < 0) {
            DEBUGV("SPIFFS_tell rc=%d\r\n", result);
            return 0;
        }

        return result;
    }

    size_t size() const override {
        CHECKFD();
        if (_written) {
            _getStat();
        }
        return _stat.size;
    }

    void close() override {
        CHECKFD();

        SPIFFS_close(_fs->getFs(), _fd);
        DEBUGV("SPIFFS_close: fd=%d\r\n", _fd);
    }

    const char* name() const override {
        CHECKFD();

        return (const char*) _stat.name;
    }

protected:
    void _getStat() const{
        CHECKFD();
        auto rc = SPIFFS_fstat(_fs->getFs(), _fd, &_stat);
        if (rc != SPIFFS_OK) {
            DEBUGV("SPIFFS_fstat rc=%d\r\n", rc);
            _stat = {0};
        }
        _written = false;
    }

    SPIFFSImpl* _fs;
    spiffs_file _fd;
    mutable spiffs_stat _stat;
    mutable bool        _written;
};

class SPIFFSDirImpl : public DirImpl {
public:
    SPIFFSDirImpl(const String& pattern, SPIFFSImpl* fs, spiffs_DIR& dir)
    : _pattern(pattern)
    , _fs(fs)
    , _dir(dir)
    , _valid(false)
    {
    }

    ~SPIFFSDirImpl() override {
        SPIFFS_closedir(&_dir);
    }

    FileImplPtr openFile(OpenMode openMode, AccessMode accessMode) override {
        if (!_valid) {
            return FileImplPtr();
        }
        int mode = getSpiffsMode(openMode, accessMode);
        auto fs = _fs->getFs();
        spiffs_file fd = SPIFFS_open_by_dirent(fs, &_dirent, mode, 0);
        if (fd < 0) {
            DEBUGV("SPIFFSDirImpl::openFile: fd=%d path=`%s` openMode=%d accessMode=%d err=%d\r\n",
                fd, _dirent.name, openMode, accessMode, fs->err_code);
            return FileImplPtr();
        }
        return std::make_shared<SPIFFSFileImpl>(_fs, fd);
    }

    const char* fileName() override {
        if (!_valid)
            return nullptr;

        return (const char*) _dirent.name;
    }

    size_t fileSize() override {
        if (!_valid)
            return 0;

        return _dirent.size;
    }

    bool next() override {
        const int n = _pattern.length();
        do {
            spiffs_dirent* result = SPIFFS_readdir(&_dir, &_dirent);
            _valid = (result != nullptr);
        } while(_valid && strncmp((const char*) _dirent.name, _pattern.c_str(), n) != 0);
        return _valid;
    }

protected:
    String _pattern;
    SPIFFSImpl* _fs;
    spiffs_DIR  _dir;
    spiffs_dirent _dirent;
    bool _valid;
};


FileImplPtr SPIFFSImpl::open(const char* path, OpenMode openMode, AccessMode accessMode) {
    int mode = getSpiffsMode(openMode, accessMode);
    int fd = SPIFFS_open(&_fs, path, mode, 0);
    if (fd < 0) {
        DEBUGV("SPIFFSImpl::open: fd=%d path=`%s` openMode=%d accessMode=%d err=%d\r\n",
            fd, path, openMode, accessMode, _fs.err_code);
        return FileImplPtr();
    }
    return std::make_shared<SPIFFSFileImpl>(this, fd);
}

bool SPIFFSImpl::exists(const char* path) {
    spiffs_stat stat;
    int rc = SPIFFS_stat(&_fs, path, &stat);
    return rc == SPIFFS_OK;
}

DirImplPtr SPIFFSImpl::openDir(const char* path) {
    spiffs_DIR dir;
    spiffs_DIR* result = SPIFFS_opendir(&_fs, path, &dir);
    if (!result) {
        DEBUGV("SPIFFSImpl::openDir: path=`%s` err=%d\r\n", path, _fs.err_code);
        return DirImplPtr();
    }
    return std::make_shared<SPIFFSDirImpl>(path, this, dir);
}

int getSpiffsMode(OpenMode openMode, AccessMode accessMode) {
    int mode = 0;
    if (openMode & OM_CREATE) {
        mode |= SPIFFS_CREAT;
    }
    if (openMode & OM_APPEND) {
        mode |= SPIFFS_APPEND;
    }
    if (openMode & OM_TRUNCATE) {
        mode |= SPIFFS_TRUNC;
    }
    if (accessMode & AM_READ) {
        mode |= SPIFFS_RDONLY;
    }
    if (accessMode & AM_WRITE) {
        mode |= SPIFFS_WRONLY;
    }
    return mode;
}

// these symbols should be defined in the linker script for each flash layout
extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;
extern "C" uint32_t _SPIFFS_page;
extern "C" uint32_t _SPIFFS_block;

static SPIFFSImpl s_defaultFs(
    (uint32_t) (&_SPIFFS_start) - 0x40200000,
    (uint32_t) (&_SPIFFS_end) - (uint32_t) (&_SPIFFS_start),
    (uint32_t) &_SPIFFS_page,
    (uint32_t) &_SPIFFS_block,
    5);

FS SPIFFS = FS(FSImplPtr(&s_defaultFs));
