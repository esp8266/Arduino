/*
 littlefs_mock.h - LittleFS HAL mock for host side testing
 Copyright © 2019 Earle F. Philhower, III

 Based on spiffs_mock:
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

#ifndef littlefs_mock_hpp
#define littlefs_mock_hpp

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <FS.h>
#include "flash_hal_mock.h"

#define DEFAULT_LITTLEFS_FILE_NAME "littlefs.bin"

class LittleFSMock
{
public:
    LittleFSMock(ssize_t fs_size, size_t fs_block, size_t fs_page, const String& storage = emptyString);
    void reset();
    ~LittleFSMock();

protected:
    void                 load();
    void                 save();

    std::vector<uint8_t> m_fs;
    String               m_storage;
    bool                 m_overwrite;
};

#define LITTLEFS_MOCK_DECLARE(size_kb, block_kb, page_b, storage) LittleFSMock littlefs_mock(size_kb * 1024, block_kb * 1024, page_b, storage)
#define LITTLEFS_MOCK_RESET() littlefs_mock.reset()

#endif /* littlefs_mock_hpp */
