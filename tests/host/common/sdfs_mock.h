/*
    sdfs.h - SDFS mock for host side testing
    Copyright (c) 2019 Earle F. Philhower, III

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
*/

#ifndef sdfs_mock_hpp
#define sdfs_mock_hpp

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <FS.h>

class SDFSMock
{
public:
    SDFSMock(ssize_t fs_size, size_t fs_block, size_t fs_page, const String& storage = emptyString)
    {
        (void)fs_size;
        (void)fs_block;
        (void)fs_page;
        (void)storage;
    }
    void reset() { }
    ~SDFSMock() { }
};

extern uint64_t _sdCardSizeB;
extern uint8_t *_sdCard;

#define SDFS_MOCK_DECLARE(size_kb, block_kb, page_b, storage) \
    SDFS.end(); \
    SDFSMock sdfs_mock(size_kb * 1024, block_kb * 1024, page_b, storage); free(_sdCard); \
    _sdCardSizeB = size_kb ? 16 * 1024 * 1024 : 0; \
    if (_sdCardSizeB) _sdCard = (uint8_t*)calloc(_sdCardSizeB, 1); \
    else _sdCard = nullptr; \
    SDFS.setConfig(SDFSConfig().setAutoFormat(true));
#define SDFS_MOCK_RESET() sdfs_mock.reset()

#endif /* spiffs_mock_hpp */
