/*
 spiffs_mock.cpp - SPIFFS HAL mock for host side testing
 Copyright © 2016 Ivan Grokhotkov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
*/


#include "spiffs_mock.h"
#include "spiffs/spiffs.h"
#include "debug.h"
#include <flash_utils.h>
#include <stdlib.h>

#include <spiffs_api.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern "C"
{
    static uint32_t s_phys_addr = 0;
    uint32_t s_phys_size = 0;
    uint32_t s_phys_page = 0;
    uint32_t s_phys_block = 0;
    uint8_t* s_phys_data = nullptr;
}

FS SPIFFS(nullptr);

SpiffsMock::SpiffsMock(ssize_t fs_size, size_t fs_block, size_t fs_page, const String& storage)
{
    m_storage = storage;
    if ((m_overwrite = (fs_size < 0)))
        fs_size = -fs_size;

    fprintf(stderr, "SPIFFS: %zd bytes\n", fs_size);

    m_fs.resize(fs_size, 0xff);
    s_phys_addr  = 0;
    s_phys_size  = static_cast<uint32_t>(fs_size);
    s_phys_page  = static_cast<uint32_t>(fs_page);
    s_phys_block = static_cast<uint32_t>(fs_block);
    s_phys_data  = m_fs.data();
    reset();
}

void SpiffsMock::reset()
{
    SPIFFS = FS(FSImplPtr(new SPIFFSImpl(0, s_phys_size, s_phys_page, s_phys_block, 5)));
    load();
}

SpiffsMock::~SpiffsMock()
{
    save();
    s_phys_addr  = 0;
    s_phys_size  = 0;
    s_phys_page  = 0;
    s_phys_block = 0;
    s_phys_data  = nullptr;
    m_fs.resize(0);
    SPIFFS = FS(FSImplPtr(nullptr));
}

void SpiffsMock::load ()
{
    if (!m_fs.size() || !m_storage.length())
        return;
    
    int fs = ::open(m_storage.c_str(), O_RDONLY);
    if (fs == -1)
    {
        fprintf(stderr, "SPIFFS: loading '%s': %s\n", m_storage.c_str(), strerror(errno));
        return;
    }
    
    off_t flen = lseek(fs, 0, SEEK_END);
    if (flen == (off_t)-1)
    {
        fprintf(stderr, "SPIFFS: checking size of '%s': %s\n", m_storage.c_str(), strerror(errno));
        return;
    }
    lseek(fs, 0, SEEK_SET);
    
    if (flen != (off_t)m_fs.size())
    {
        fprintf(stderr, "SPIFFS: size of '%s': %d does not match requested size %zd\n", m_storage.c_str(), (int)flen, m_fs.size());
        if (!m_overwrite)
        {
            fprintf(stderr, "SPIFFS: aborting at user request\n");
            exit(1);
        }
        fprintf(stderr, "SPIFFS: continuing without loading at user request, '%s' will be overwritten\n", m_storage.c_str());
    }
    else
    {
        fprintf(stderr, "SPIFFS: loading %zi bytes from '%s'\n", m_fs.size(), m_storage.c_str());
        ssize_t r = ::read(fs, m_fs.data(), m_fs.size());
        if (r != (ssize_t)m_fs.size())
            fprintf(stderr, "SPIFFS: reading %zi bytes: returned %zd: %s\n", m_fs.size(), r, strerror(errno));
    }
    ::close(fs);
}

void SpiffsMock::save ()
{
    if (!m_fs.size() || !m_storage.length())
        return;

    int fs = ::open(m_storage.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fs == -1)
    {
        fprintf(stderr, "SPIFFS: saving '%s': %s\n", m_storage.c_str(), strerror(errno));
        return;
    }
    fprintf(stderr, "SPIFFS: saving %zi bytes to '%s'\n", m_fs.size(), m_storage.c_str());

    if (::write(fs, m_fs.data(), m_fs.size()) != (ssize_t)m_fs.size())
        fprintf(stderr, "SPIFFS: writing %zi bytes: %s\n", m_fs.size(), strerror(errno));
    if (::close(fs) == -1)
        fprintf(stderr, "SPIFFS: closing %s: %s\n", m_storage.c_str(), strerror(errno));
}

int32_t spiffs_hal_read(uint32_t addr, uint32_t size, uint8_t *dst) {
    memcpy(dst, s_phys_data + addr, size);
    return SPIFFS_OK;
}

int32_t spiffs_hal_write(uint32_t addr, uint32_t size, uint8_t *src) {
    memcpy(s_phys_data + addr, src, size);
    return SPIFFS_OK;
}

int32_t spiffs_hal_erase(uint32_t addr, uint32_t size) {
    if ((size & (FLASH_SECTOR_SIZE - 1)) != 0 ||
        (addr & (FLASH_SECTOR_SIZE - 1)) != 0) {
        abort();
    }
    const uint32_t sector = addr / FLASH_SECTOR_SIZE;
    const uint32_t sectorCount = size / FLASH_SECTOR_SIZE;
    for (uint32_t i = 0; i < sectorCount; ++i) {
        memset(s_phys_data + (sector + i) * FLASH_SECTOR_SIZE, 0xff, FLASH_SECTOR_SIZE);
    }
    return SPIFFS_OK;
}
