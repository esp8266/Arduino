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

extern "C"
{
    uint32_t mock_spiffs_phys_addr = 0;
    uint32_t mock_spiffs_phys_size = 0;
    uint32_t mock_spiffs_phys_page = 0;
    uint32_t mock_spiffs_phys_block = 0;
    uint8_t* mock_spiffs_phys_data = nullptr;
}

FS SPIFFS(nullptr);



SpiffsMock::SpiffsMock(size_t fs_size, size_t fs_block, size_t fs_page)
{
    m_fs.resize(fs_size, 0xff);
    mock_spiffs_phys_addr  = 0;
    mock_spiffs_phys_size  = fs_size;
    mock_spiffs_phys_page  = fs_page;
    mock_spiffs_phys_block = fs_block;
    mock_spiffs_phys_data  = m_fs.data();
    SPIFFS = FS(FSImplPtr(new SPIFFSImpl(0, fs_size, fs_page, fs_block, 5)));
}
    
SpiffsMock::~SpiffsMock()
{
    mock_spiffs_phys_addr  = 0;
    mock_spiffs_phys_size  = 0;
    mock_spiffs_phys_page  = 0;
    mock_spiffs_phys_block = 0;
    mock_spiffs_phys_data  = nullptr;
    SPIFFS = FS(FSImplPtr(nullptr));
}

/*
 spi_flash_read function requires flash address to be aligned on word boundary.
 We take care of this by reading first and last words separately and memcpy
 relevant bytes into result buffer.
 
 alignment:       012301230123012301230123
 bytes requested: -------***********------
 read directly:   --------xxxxxxxx--------
 read pre:        ----aaaa----------------
 read post:       ----------------bbbb----
 alignedBegin:            ^
 alignedEnd:                      ^
 */

int32_t spiffs_hal_read(uint32_t addr, uint32_t size, uint8_t *dst) {
    memcpy(dst, mock_spiffs_phys_data + addr, size);
    return SPIFFS_OK;
}

/*
 Like spi_flash_read, spi_flash_write has a requirement for flash address to be
 aligned. However it also requires RAM address to be aligned as it reads data
 in 32-bit words. Flash address (mis-)alignment is handled much the same way
 as for reads, but for RAM alignment we have to copy data into a temporary
 buffer. The size of this buffer is a tradeoff between number of writes required
 and amount of stack required. This is chosen to be 512 bytes here, but might
 be adjusted in the future if there are good reasons to do so.
 */

int32_t spiffs_hal_write(uint32_t addr, uint32_t size, uint8_t *src) {
    memcpy(mock_spiffs_phys_data + addr, src, size);
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
        memset(mock_spiffs_phys_data + (sector + i) * FLASH_SECTOR_SIZE, 0xff, FLASH_SECTOR_SIZE);
    }
    return SPIFFS_OK;
}
