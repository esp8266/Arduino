/* 
 core_esp8266_eboot_command.c - interface to the eboot bootloader

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

#include <stddef.h>
#include <stdbool.h>
#include "eboot_command.h"

extern "C" {

static uint32_t crc_update(uint32_t crc, const uint8_t *data, size_t length)
{
    uint32_t i;
    bool bit;
    uint8_t c;
    
    while (length--) {
        c = *data++;
        for (i = 0x80; i > 0; i >>= 1) {
            bit = crc & 0x80000000;
            if (c & i) {
                bit = !bit;
            }
            crc <<= 1;
            if (bit) {
                crc ^= 0x04c11db7;
            }
        }
    }
    return crc;
}

static uint32_t eboot_command_calculate_crc32(const struct eboot_command* cmd)
{
    return crc_update(0xffffffff, (const uint8_t*) cmd, 
                      offsetof(struct eboot_command, crc32));
}

int eboot_command_read(struct eboot_command* cmd)
{
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    uint32_t* dst = (uint32_t *) cmd;
    for (uint32_t i = 0; i < dw_count; ++i) {
        dst[i] = RTC_MEM[i];
    }

    uint32_t crc32 = eboot_command_calculate_crc32(cmd);
    if ((cmd->magic & EBOOT_MAGIC_MASK) != EBOOT_MAGIC ||
        cmd->crc32 != crc32) {
        return 1;
    }

    return 0;
}

void eboot_command_write(struct eboot_command* cmd)
{
    cmd->magic = EBOOT_MAGIC;
    cmd->crc32 = eboot_command_calculate_crc32(cmd);

    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    const uint32_t* src = (const uint32_t *) cmd;
    for (uint32_t i = 0; i < dw_count; ++i) {
        RTC_MEM[i] = src[i];
    }
}

void eboot_command_clear()
{
    RTC_MEM[offsetof(struct eboot_command, magic) / sizeof(uint32_t)] = 0;
    RTC_MEM[offsetof(struct eboot_command, crc32) / sizeof(uint32_t)] = 0;
}

};
