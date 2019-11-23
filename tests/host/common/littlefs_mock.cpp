/*
 littlefs_mock.cpp - LittleFS mock for host side testing
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
#include <flash_hal.h>

#define LITTLEFS_FILE_NAME "littlefs.bin"

FS LittleFS(nullptr);

LittleFSMock::LittleFSMock(ssize_t fs_size, size_t fs_block, size_t fs_page, const String& storage)
{
    m_storage = storage;
    if ((m_overwrite = (fs_size < 0)))
        fs_size = -fs_size;

    fprintf(stderr, "LittleFS: %zd bytes\n", fs_size);

    m_fs.resize(fs_size, 0xff);
    s_phys_addr  = 0;
    s_phys_size  = static_cast<uint32_t>(fs_size);
    s_phys_page  = static_cast<uint32_t>(fs_page);
    s_phys_block = static_cast<uint32_t>(fs_block);
    s_phys_data  = m_fs.data();
    reset();
}

void LittleFSMock::reset()
{
    LittleFS = FS(FSImplPtr(new littlefs_impl::LittleFSImpl(0, s_phys_size, s_phys_page, s_phys_block, 5)));
    load();
}

LittleFSMock::~LittleFSMock()
{
    save();
    s_phys_addr  = 0;
    s_phys_size  = 0;
    s_phys_page  = 0;
    s_phys_block = 0;
    s_phys_data  = nullptr;
    m_fs.resize(0);
    LittleFS = FS(FSImplPtr(nullptr));
}

void LittleFSMock::load ()
{
    if (!m_fs.size() || !m_storage.length())
        return;
    
    int fs = ::open(m_storage.c_str(), O_RDONLY);
    if (fs == -1)
    {
        fprintf(stderr, "LittleFS: loading '%s': %s\n", m_storage.c_str(), strerror(errno));
        return;
    }
    
    off_t flen = lseek(fs, 0, SEEK_END);
    if (flen == (off_t)-1)
    {
        fprintf(stderr, "LittleFS: checking size of '%s': %s\n", m_storage.c_str(), strerror(errno));
        return;
    }
    lseek(fs, 0, SEEK_SET);
    
    if (flen != (off_t)m_fs.size())
    {
        fprintf(stderr, "LittleFS: size of '%s': %d does not match requested size %zd\n", m_storage.c_str(), (int)flen, m_fs.size());
        if (!m_overwrite)
        {
            fprintf(stderr, "LittleFS: aborting at user request\n");
            exit(1);
        }
        fprintf(stderr, "LittleFS: continuing without loading at user request, '%s' will be overwritten\n", m_storage.c_str());
    }
    else
    {
        fprintf(stderr, "LittleFS: loading %zi bytes from '%s'\n", m_fs.size(), m_storage.c_str());
        ssize_t r = ::read(fs, m_fs.data(), m_fs.size());
        if (r != (ssize_t)m_fs.size())
            fprintf(stderr, "LittleFS: reading %zi bytes: returned %zd: %s\n", m_fs.size(), r, strerror(errno));
    }
    ::close(fs);
}

void LittleFSMock::save ()
{
    if (!m_fs.size() || !m_storage.length())
        return;

    int fs = ::open(m_storage.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fs == -1)
    {
        fprintf(stderr, "LittleFS: saving '%s': %s\n", m_storage.c_str(), strerror(errno));
        return;
    }
    fprintf(stderr, "LittleFS: saving %zi bytes to '%s'\n", m_fs.size(), m_storage.c_str());

    if (::write(fs, m_fs.data(), m_fs.size()) != (ssize_t)m_fs.size())
        fprintf(stderr, "LittleFS: writing %zi bytes: %s\n", m_fs.size(), strerror(errno));
    if (::close(fs) == -1)
        fprintf(stderr, "LittleFS: closing %s: %s\n", m_storage.c_str(), strerror(errno));
}
