/*
 spiffs_hal.cpp - SPI read/write/erase functions for SPIFFS.
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

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

#include <Arduino.h>
#include <stdlib.h>
#include <algorithm>
#include "debug.h"
#include "flash_hal.h"

extern "C" {
#include "c_types.h"
#include "spi_flash.h"
}

int32_t flash_hal_read(uint32_t addr, uint32_t size, uint8_t *dst) {
    optimistic_yield(10000);

    uint32_t result = FLASH_HAL_OK;
    // We use flashRead overload that handles proper alignment
    if (ESP.flashRead(addr, dst, size)) {
        return FLASH_HAL_OK;
    } else {
        return FLASH_HAL_READ_ERROR;
    }
}

int32_t flash_hal_write(uint32_t addr, uint32_t size, const uint8_t *src) {
    optimistic_yield(10000);

    // We use flashWrite overload that handles proper alignment
    if (ESP.flashWrite(addr, src, size)) {
        return FLASH_HAL_OK;
    } else {
        return FLASH_HAL_WRITE_ERROR;
    }
}

int32_t flash_hal_erase(uint32_t addr, uint32_t size) {
    if ((size & (SPI_FLASH_SEC_SIZE - 1)) != 0 ||
        (addr & (SPI_FLASH_SEC_SIZE - 1)) != 0) {
        DEBUGV("_spif_erase called with addr=%x, size=%d\r\n", addr, size);
        abort();
    }
    const uint32_t sector = addr / SPI_FLASH_SEC_SIZE;
    const uint32_t sectorCount = size / SPI_FLASH_SEC_SIZE;
    for (uint32_t i = 0; i < sectorCount; ++i) {
        optimistic_yield(10000);
        if (!ESP.flashEraseSector(sector + i)) {
            DEBUGV("_spif_erase addr=%x size=%d i=%d\r\n", addr, size, i);
            return FLASH_HAL_ERASE_ERROR;
        }
    }
    return FLASH_HAL_OK;
}
