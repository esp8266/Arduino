/* Copyright (c) 2015 Ivan Grokhotkov. All rights reserved. 
 * This file is part of eboot bootloader.
 *
 * Redistribution and use is permitted according to the conditions of the
 * 3-clause BSD license to be found in the LICENSE file.
 */
 
#ifndef EBOOT_COMMAND_H
#define EBOOT_COMMAND_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define RTC_MEM ((volatile uint32_t*)0x60001200)

enum action_t {
    ACTION_COPY_RAW = 0x00000001,
    ACTION_LOAD_APP = 0xffffffff
};

#define EBOOT_MAGIC 	 0xeb001000
#define EBOOT_MAGIC_MASK 0xfffff000

typedef struct eboot_command {
    uint32_t magic;
    enum action_t action;
    uint32_t args[29];
    uint32_t crc32;
} eboot_command_t;


#if defined (EBOOT_ENABLE_FLASH_STORAGE)
// Magic for version 1 corresponds to AR01
#define EBOOT_INDEX_MAGIC_V1 0x31305241
#define EBOOT_COMMAND_MAX_COUNT 32


#define FLASH_SECTOR_SIZE 0x1000


/* A command is ready to be actioned if it is both
 * Not free (SLOT_FREE == 0) AND
 * pending (PENDING == 1)
*/

// Is the slot available for new command data (1 == Yes; 0 == It's been used, so No)
#define EBOOT_CMD_FLAG_SLOT_FREE 	(1 << 0)
// Has the command been actioned (1 == No, needs to be actioned; 0 == It's been completed)
#define EBOOT_CMD_FLAG_PENDING 		(1 << 1)

typedef struct eboot_flash_command {
	uint32_t				flags;
	eboot_command_t		    cmd;
} eboot_flash_command_t;

typedef struct eboot_index {
	uint32_t 				version;
	uint32_t 				magic;
	eboot_flash_command_t  *commands;
} eboot_index_t;

#endif // EBOOT_ENABLE_FLASH_STORAGE

int eboot_command_read(struct eboot_command* cmd);
void eboot_command_write(struct eboot_command* cmd);
void eboot_command_clear();

#endif //EBOOT_COMMAND_H
