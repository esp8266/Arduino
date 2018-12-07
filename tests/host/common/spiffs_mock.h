/*
 spiffs_mock.h - SPIFFS HAL mock for host side testing
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

#ifndef spiffs_mock_hpp
#define spiffs_mock_hpp

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <FS.h>

#define DEFAULT_SPIFFS_FILE_NAME "spiffs.bin"

class SpiffsMock {
public:
    SpiffsMock(size_t fs_size, size_t fs_block, size_t fs_page, bool storage = true);
    void reset();
    ~SpiffsMock();
    
protected:
    void load ();
    void save ();

    // it was a vector, but CI tests & valgrind complain with:
    // Syscall param write(buf) points to uninitialised byte(s)
    //    by 0x43E9FF: SpiffsMock::save() (spiffs_mock.cpp:116)
    //    = if (::write(fs, &m_fs[0], m_fs_size) != (ssize_t)m_fs_size)
    // so switched to a regular array
    // and that bug is still here
    // XXXWIPTODO

    uint8_t* m_fs;
    size_t m_fs_size;
    bool m_storage;
};

#define SPIFFS_MOCK_DECLARE(size_kb, block_kb, page_b, storage) SpiffsMock spiffs_mock(size_kb * 1024, block_kb * 1024, page_b, storage)
#define SPIFFS_MOCK_RESET() spiffs_mock.reset()


#endif /* spiffs_mock_hpp */
