/*
 flash_utils.h - Flash access function and data structures
 Copyright (c) 2015 Ivan Grokhotkov.  All right reserved.

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


#ifndef FLASH_UTILS_H
#define FLASH_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

int SPIEraseBlock(uint32_t block);
int SPIEraseSector(uint32_t sector);
int SPIRead(uint32_t addr, void *dest, size_t size);
int SPIWrite(uint32_t addr, void *src, size_t size);
int SPIEraseAreaEx(const uint32_t start, const uint32_t size);

#define FLASH_SECTOR_SIZE 0x1000
#define FLASH_BLOCK_SIZE 0x10000
#define APP_START_OFFSET 0x1000

typedef struct {
    unsigned char       magic;
    unsigned char       num_segments;

    /* SPI Flash Interface (0 = QIO, 1 = QOUT, 2 = DIO, 0x3 = DOUT) */
    unsigned char       flash_mode;

    /* High four bits: 0 = 512K, 1 = 256K, 2 = 1M, 3 = 2M, 4 = 4M, 
       Low four bits:  0 = 40MHz, 1= 26MHz, 2 = 20MHz, 0xf = 80MHz */
    unsigned char		flash_size_freq;

    uint32_t            entry;
} image_header_t;


typedef struct {
    uint32_t            address;
    uint32_t            size;
} section_header_t;

#ifdef __cplusplus
}
#endif


#endif //FLASH_UTILS_H
