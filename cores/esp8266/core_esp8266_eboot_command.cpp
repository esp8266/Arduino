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
#include "coredecls.h"
#include <stdint.h>
#include "eboot_command.h"
#include "flash_utils.h"
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"

extern "C" {

static uint32_t eboot_command_calculate_crc32(const struct eboot_command* cmd)
{
    return crc32((const uint8_t*) cmd, offsetof(struct eboot_command, crc32));
}

#if defined (EBOOT_ENABLE_FLASH_STORAGE)

uint32_t eboot_get_flash_block_size(void) {
    return 2 * 4 * 1024;
}

uint32_t eboot_read_flash_index(eboot_index_t *eboot_index)
{
    if (spi_flash_read(0 + APP_START_OFFSET + sizeof(image_header_t) + sizeof(section_header_t), (uint32_t *) &eboot_index, sizeof(eboot_index)) != 0) {
        return 0;
    }

    if (eboot_index->magic != EBOOT_INDEX_MAGIC_V1) {
        return 0;
    }

    return 1;
}

eboot_flash_command_t *commandAddress(void) {
  eboot_index_t eboot_index;
  const uint32_t addr = 0 + APP_START_OFFSET + sizeof(image_header_t) + sizeof(section_header_t);
  ets_printf("commandAddress: %p\n",  (void *) addr);
  if (spi_flash_read(addr, (uint32_t *)&eboot_index, sizeof(eboot_index)) != 0) {
    ets_printf("failed to read SPI\n");
    return NULL;
  }
  if (eboot_index.magic != EBOOT_INDEX_MAGIC_V1) {
    ets_printf("failed to find MAGIC value (had %x)\n", eboot_index.magic);
    return NULL;
  } else {
    return eboot_index.commands;
  }
}

bool readBootCommand(int cmd, eboot_flash_command_t *dst) {
  eboot_flash_command_t *cmds = commandAddress();
  if (cmds == 0) {
    return 0;
  }
  ets_printf("Reading command %d from flash: %p\n", cmd, (void *) cmds);
  if (cmd >= EBOOT_COMMAND_MAX_COUNT) {
    return 0;
  }
  uint32_t addr = (uint32_t) cmds - 0x40200000;
  addr += cmd * sizeof(*dst);
  ets_printf("Reading command %d from flash @ %p\n", cmd, (void *) addr);
  if (spi_flash_read(addr, (uint32_t *)dst, sizeof(*dst))) {
    return 0;
  }
  return 1;
}

bool writeBootCommand(int cmd, eboot_flash_command_t *dst) {
  eboot_flash_command_t *cmds = commandAddress();
  ets_printf("!Writing command %d to flash: %p\n", cmd, (void *) cmds);
  if (cmd >= EBOOT_COMMAND_MAX_COUNT) {
    return 0;
  }
  uint32_t addr = (uint32_t)cmds - 0x40200000;
  addr += cmd * sizeof(*dst);
  ets_printf("Writing command %d to flash @ %p\n", cmd, (void *) addr);
  if (spi_flash_write(addr, (uint32_t *)dst, sizeof(*dst))) {
    return 0;
  }
  return 1;
}

bool eraseBootCommandBlock(void) {
  eboot_flash_command_t *cmds = commandAddress();
  uint32_t addr = (uint32_t)cmds - 0x40200000;
  ets_printf("Erasing command block at %p\n", (void *) addr);
  if (spi_flash_erase_sector(addr / FLASH_SECTOR_SIZE)) {
    return 0;
  }
  return 1;
}

uint32_t eboot_command_read_from_flash(eboot_flash_command_t *cmd)
{
    eboot_index_t eboot_index;
    eboot_flash_command_t *flash_command;
    uint32_t i;
   const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);

    if (!eboot_read_flash_index(&eboot_index)) {
        return 0;
    } else {
        flash_command = eboot_index.commands;
        for (i = 0; i < EBOOT_COMMAND_MAX_COUNT; i++) {
            if (((flash_command->flags & EBOOT_CMD_FLAG_SLOT_FREE) == 0) &&
                ((flash_command->flags & EBOOT_CMD_FLAG_PENDING) == EBOOT_CMD_FLAG_PENDING)) {
                // This is a valid command waiting to be actioned, or should be. The CRC check will determine if it's actually valid
                uint32_t* dst = (uint32_t *) cmd;
                uint32_t* src = (uint32_t *) &(flash_command->cmd);
                for (uint32_t i = 0; i < dw_count; ++i) {
                    dst[i] = src[i];
                }
                return 1;
            }
        }
    }
    return 0;
}

uint32_t eboot_command_write_to_flash(struct eboot_command *cmd)
{
    eboot_flash_command_t flash_command;
    uint32_t i, *dst, *src = (uint32_t *)cmd;
    int32_t target_command_slot = -1;
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    ets_printf("Writing command to flash: %p\n", (void *) cmd);

    for (i = 0; i < EBOOT_COMMAND_MAX_COUNT; i++) {
        ets_printf("Read bootCommand %d, flags: %x\n", i, flash_command.flags);
        if (readBootCommand(i, &flash_command) == 0) {
            ets_printf("Reading bootCommand %d failed!\n", i);
        } else {
            ets_printf("Read bootCommand %d, flags: %x\n", i, flash_command.flags);
            if (((flash_command.flags & EBOOT_CMD_FLAG_SLOT_FREE) == EBOOT_CMD_FLAG_SLOT_FREE) &&
                ((flash_command.flags & EBOOT_CMD_FLAG_PENDING) == EBOOT_CMD_FLAG_PENDING) && 
                (flash_command.cmd.magic == 0)
                ) {
                target_command_slot = i;
                break;
            }
        }
    }
    if (target_command_slot == -1) {
        // We didn't find a free slot. Assume this is due to the slots being all used,
        //  so erase the storage page
        if (eraseBootCommandBlock() != 1) {
            return 0;
        }
        // Block is now clear, so we can use the first slot
        target_command_slot = 0;
        // And reinitialise our block to a blank one.
        readBootCommand(target_command_slot, &flash_command);
    }
    dst = (uint32_t *)&flash_command.cmd;
    for (uint32_t j = 0; j < dw_count; ++j) {
        dst[j] = src[j];
    }
    flash_command.flags &= ~EBOOT_CMD_FLAG_SLOT_FREE;
    ets_printf("Writing command %d to flash: %p\n", target_command_slot, (void *) cmd);
    writeBootCommand(target_command_slot, &flash_command);
    return 1;
}

#endif // EBOOT_ENABLE_FLASH_STORAGE

int eboot_command_read_from_rtc(struct eboot_command *cmd)
{
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    uint32_t* dst = (uint32_t *) cmd;
    for (uint32_t i = 0; i < dw_count; ++i) {
        dst[i] = RTC_MEM[i];
    }

    return 0;
}

void eboot_command_write_to_rtc(struct eboot_command *cmd)
{
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    const uint32_t* src = (const uint32_t *) cmd;
    for (uint32_t i = 0; i < dw_count; ++i) {
        RTC_MEM[i] = src[i];
    }
}

void eboot_command_clear_rtc(void)
{
    RTC_MEM[offsetof(struct eboot_command, magic) / sizeof(uint32_t)] = 0;
    RTC_MEM[offsetof(struct eboot_command, crc32) / sizeof(uint32_t)] = 0;
}

int eboot_command_read(struct eboot_command* cmd)
{
    uint32_t have_command = 0;
#if defined (EBOOT_ENABLE_FLASH_STORAGE)
    // have_command = eboot_command_read_from_flash(cmd);
#endif // EBOOT_ENABLE_FLASH_STORAGE
    if (have_command == 0) {
        eboot_command_read_from_rtc(cmd);
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
    uint32_t saved = 0;
    cmd->magic = EBOOT_MAGIC;
    cmd->crc32 = eboot_command_calculate_crc32(cmd);
    ets_printf("Writing command: %p\n", (void *) cmd);
#if defined (EBOOT_ENABLE_FLASH_STORAGE)
    saved = eboot_command_write_to_flash(cmd);
#endif // EBOOT_ENABLE_FLASH_STORAGE
    if (saved == 0) {
        eboot_command_write_to_rtc(cmd);
    }
}

void eboot_command_clear()
{
#if defined (EBOOT_ENABLE_FLASH_STORAGE)
#endif // EBOOT_ENABLE_FLASH_STORAGE
    eboot_command_clear_rtc();
}

};
