/* Copyright (c) 2015 Ivan Grokhotkov. All rights reserved. 
 * This file is part of eboot bootloader.
 *
 * Redistribution and use is permitted according to the conditions of the
 * 3-clause BSD license to be found in the LICENSE file.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "flash.h"


int SPIEraseAreaEx(const uint32_t start, const uint32_t size)
{
    if (start & (FLASH_SECTOR_SIZE - 1) != 0) {
        return 1;
    }

    const uint32_t sectors_per_block = FLASH_BLOCK_SIZE / FLASH_SECTOR_SIZE;
    uint32_t current_sector = start / FLASH_SECTOR_SIZE;
    uint32_t sector_count = (size + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;
    const uint32_t end = current_sector + sector_count;

    for (; current_sector < end && (current_sector & (sectors_per_block-1)); 
        ++current_sector, --sector_count) {
        if (SPIEraseSector(current_sector)) {
            return 2;
        }
    }

    for (;current_sector + sectors_per_block <= end; 
        current_sector += sectors_per_block, 
        sector_count -= sectors_per_block) {
        if (SPIEraseBlock(current_sector / sectors_per_block)) {
            return 3;
        }
    }

    for (; current_sector < end; 
        ++current_sector, --sector_count) {
        if (SPIEraseSector(current_sector)) {
            return 4;
        }
    }

    return 0;
}

