/* Copyright (c) 2015 Ivan Grokhotkov. All rights reserved. 
 * This file is part of eboot bootloader.
 *
 * Redistribution and use is permitted according to the conditions of the
 * 3-clause BSD license to be found in the LICENSE file.
 */

#ifndef FLASH_H
#define FLASH_H


/* The geometry defines are placed in the sdk. The .h was factored out for reuse by eboot here.
 * Beware: this means that eboot has an external dependency.
 * The following .h is placed in tools/sdk/includes
 */
#include <spi_flash_geometry.h>

#define APP_START_OFFSET 0x1000

int SPIEraseBlock(uint32_t block);
int SPIEraseSector(uint32_t sector);
int SPIRead(uint32_t addr, void *dest, size_t size);
int SPIWrite(uint32_t addr, void *src, size_t size);
int SPIEraseAreaEx(const uint32_t start, const uint32_t size);


typedef struct {
    unsigned char       magic;
    unsigned char       num_segments;

    /* SPI Flash Interface (0 = QIO, 1 = QOUT, 2 = DIO, 0x3 = DOUT) */
    unsigned char       flash_mode;

    /* High four bits: 0 = 512K, 1 = 256K, 2 = 1M, 3 = 2M, 4 = 4M, 8 = 8M, 9 = 16M
       Low four bits:  0 = 40MHz, 1= 26MHz, 2 = 20MHz, 0xf = 80MHz */
    unsigned char		flash_size_freq;

    uint32_t            entry;
} image_header_t;


typedef struct {
    uint32_t            address;
    uint32_t            size;
} section_header_t;



#endif //FLASH_H
