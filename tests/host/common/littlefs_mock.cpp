/*
 littlefs_mock.cpp - SPIFFS HAL mock for host side testing
 Copyright © 2019 Earle F. Philhower, III

 Based off spiffs_mock.cpp:
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


#include "littlefs_mock.h"
#include "spiffs_mock.h"
#include "spiffs/spiffs.h"
#include "debug.h"
#include <flash_utils.h>
#include <stdlib.h>
#include <LittleFS.h>

#include <spiffs_api.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define LITTLEFS_FILE_NAME "littlefs.bin"

extern "C"
{
    extern uint32_t s_phys_size;
    extern uint32_t s_phys_page;
    extern uint32_t s_phys_block;
    extern uint8_t* s_phys_data;
}

FS LittleFS(nullptr);

LittleFSMock::LittleFSMock(size_t fs_size, size_t fs_block, size_t fs_page, bool storage)
{
    fprintf(stderr, "LittleFS: %zd bytes\n", fs_size);

    m_storage = storage;
    m_fs = new uint8_t[m_fs_size = fs_size];
    memset(&m_fs[0], 0xff, m_fs_size);

    s_phys_size  = static_cast<uint32_t>(fs_size);
    s_phys_page  = static_cast<uint32_t>(fs_page);
    s_phys_block = static_cast<uint32_t>(fs_block);
    s_phys_data  = &m_fs[0];
    reset();
}

void LittleFSMock::reset()
{
    LittleFS = FS(FSImplPtr(new littlefs_impl::LittleFSImpl(0, s_phys_size, s_phys_page, s_phys_block, 5)));
    if (m_storage)
        load();
}

LittleFSMock::~LittleFSMock()
{
    if (m_storage)
        save();
    s_phys_size  = 0;
    s_phys_page  = 0;
    s_phys_block = 0;
    s_phys_data  = nullptr;
    delete [] m_fs;
    m_fs = nullptr;
    m_fs_size = 0;
    LittleFS = FS(FSImplPtr(nullptr));
}

void LittleFSMock::load ()
{
    if (!m_fs_size)
        return;

    const char* fname = getenv("LITTLEFS_PATH");
    if (!fname)
        fname = DEFAULT_LITTLEFS_FILE_NAME;
    int fs = ::open(LITTLEFS_FILE_NAME, O_RDONLY);
    if (fs == -1)
    {
        fprintf(stderr, "LittleFS: loading '%s': %s\n", fname, strerror(errno));
        return;
    }
    fprintf(stderr, "LittleFS: loading %zi bytes from '%s'\n", m_fs_size, fname);
    if (::read(fs, &m_fs[0], m_fs_size) != (ssize_t)m_fs_size)
        fprintf(stderr, "LittleFS: reading %zi bytes: %s\n", m_fs_size, strerror(errno));
    ::close(fs);
}

void LittleFSMock::save ()
{
    if (!m_fs_size)
        return;

    const char* fname = getenv("LITTLEFS_PATH");
    if (!fname)
        fname = DEFAULT_LITTLEFS_FILE_NAME;
    int fs = ::open(LITTLEFS_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fs == -1)
    {
        fprintf(stderr, "LittleFS: saving '%s': %s\n", fname, strerror(errno));
        return;
    }
    fprintf(stderr, "LittleFS: saving %zi bytes to '%s'\n", m_fs_size, fname);

// this can be a valgrind error, I don't understand how it happens
//for (size_t i = 0; i < m_fs_size; i++) printf("\r%zd:%d   ", i, (int)m_fs[i]);

    if (::write(fs, &m_fs[0], m_fs_size) != (ssize_t)m_fs_size)
        fprintf(stderr, "LittleFS: writing %zi bytes: %s\n", m_fs_size, strerror(errno));
    if (::close(fs) == -1)
        fprintf(stderr, "LittleFS: closing %s: %s\n", fname, strerror(errno));
}
