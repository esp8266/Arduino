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

#define SPIFFS_FILE_NAME "spiffs.bin"

extern "C"
{
    static uint32_t s_phys_addr = 0;
    uint32_t s_phys_size = 0;
    uint32_t s_phys_page = 0;
    uint32_t s_phys_block = 0;
    uint8_t* s_phys_data = nullptr;
}

FS SPIFFS(nullptr);

SpiffsMock::SpiffsMock(size_t fs_size, size_t fs_block, size_t fs_page, bool storage)
{
    fprintf(stderr, "SPIFFS: %zd bytes\n", fs_size);

    m_storage = storage;
    m_fs = new uint8_t[m_fs_size = fs_size];
    memset(&m_fs[0], 0xff, m_fs_size);

    s_phys_addr  = 0;
    s_phys_size  = static_cast<uint32_t>(fs_size);
    s_phys_page  = static_cast<uint32_t>(fs_page);
    s_phys_block = static_cast<uint32_t>(fs_block);
    s_phys_data  = &m_fs[0];
    reset();
}

void SpiffsMock::reset()
{
    SPIFFS = FS(FSImplPtr(new SPIFFSImpl(0, s_phys_size, s_phys_page, s_phys_block, 5)));
    if (m_storage)
        load();
}

SpiffsMock::~SpiffsMock()
{
    if (m_storage)
        save();
    s_phys_addr  = 0;
    s_phys_size  = 0;
    s_phys_page  = 0;
    s_phys_block = 0;
    s_phys_data  = nullptr;
    delete [] m_fs;
    m_fs = nullptr;
    m_fs_size = 0;
    SPIFFS = FS(FSImplPtr(nullptr));
}

void SpiffsMock::load ()
{
    if (!m_fs_size)
        return;

    const char* fname = getenv("SPIFFS_PATH");
    if (!fname)
        fname = DEFAULT_SPIFFS_FILE_NAME;
    int fs = ::open(SPIFFS_FILE_NAME, O_RDONLY);
    if (fs == -1)
    {
        fprintf(stderr, "SPIFFS: loading '%s': %s\n", fname, strerror(errno));
        return;
    }
    fprintf(stderr, "SPIFFS: loading %zi bytes from '%s'\n", m_fs_size, fname);
    if (::read(fs, &m_fs[0], m_fs_size) != (ssize_t)m_fs_size)
        fprintf(stderr, "SPIFFS: reading %zi bytes: %s\n", m_fs_size, strerror(errno));
    ::close(fs);
}

void SpiffsMock::save ()
{
    if (!m_fs_size)
        return;

    const char* fname = getenv("SPIFFS_PATH");
    if (!fname)
        fname = DEFAULT_SPIFFS_FILE_NAME;
    int fs = ::open(SPIFFS_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fs == -1)
    {
        fprintf(stderr, "SPIFFS: saving '%s': %s\n", fname, strerror(errno));
        return;
    }
    fprintf(stderr, "SPIFFS: saving %zi bytes to '%s'\n", m_fs_size, fname);

// this can be a valgrind error, I don't understand how it happens
//for (size_t i = 0; i < m_fs_size; i++) printf("\r%zd:%d   ", i, (int)m_fs[i]);

    if (::write(fs, &m_fs[0], m_fs_size) != (ssize_t)m_fs_size)
        fprintf(stderr, "SPIFFS: writing %zi bytes: %s\n", m_fs_size, strerror(errno));
    if (::close(fs) == -1)
        fprintf(stderr, "SPIFFS: closing %s: %s\n", fname, strerror(errno));
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
