#ifndef flash_hal_h
#define flash_hal_h

/*
 flash_hal.h - API for accessing raw flash for filesystems
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.

 This code was influenced by NodeMCU and Sming libraries, and first version of
 Arduino wrapper written by Hristo Gochkov.

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

#ifdef __cplusplus
extern "C" {
#endif

#if AUTOFLASHSIZE

//#warning auto flash size
extern uintptr_t EEPROM_start;
extern uintptr_t FS_start;
extern uintptr_t FS_end;
extern uint16_t FS_page;
extern uint16_t FS_block;

#include <FlashMap.h>
extern uint32_t spi_flash_get_id (void); // <user_interface.h>
#define FLASHMAPCONFIG(conf) \
  { \
    static const flash_map_s flashdesc[] PROGMEM = conf; \
    uint32_t flash_chip_size_kb = 1 << (((spi_flash_get_id() >> 16) & 0xff) - 10); \
    for (size_t i = 0; i < sizeof(flashdesc) / sizeof(flashdesc[0]); i++) \
      if (pgm_read_word(&flashdesc[i].flash_size_kb) == flash_chip_size_kb) \
      { \
        EEPROM_start = (uintptr_t)pgm_read_dword(&flashdesc[i].eeprom_start); \
        FS_start = (uintptr_t)pgm_read_dword(&flashdesc[i].fs_start); \
        FS_end = (uintptr_t)pgm_read_dword(&flashdesc[i].fs_end); \
        FS_block = pgm_read_word(&flashdesc[i].fs_block_size); \
        FS_page = pgm_read_word(&flashdesc[i].fs_page_size); \
      } \
  }

#else // !AUTOFLASHSIZE

//#warning no auto flash size
extern "C" uint32_t _FS_start;
extern "C" uint32_t _FS_end;
extern "C" uint32_t _FS_page;
extern "C" uint32_t _FS_block;
extern uint32_t _EEPROM_start;
#define EEPROM_start ((uintptr_t)&_EEPROM_start)
#define FS_start ((uintptr_t)&_FS_start)
#define FS_end ((uintptr_t)&_FS_end)
#define FS_page ((uintptr_t)&_FS_page)
#define FS_block ((uintptr_t)&_FS_block)

#endif // AUTOFLASHSIZE

#define FS_PHYS_ADDR ((uint32_t) (FS_start) - 0x40200000)
#define FS_PHYS_SIZE ((uint32_t) (FS_end) - (uint32_t) (FS_start))
#define FS_PHYS_PAGE ((uint32_t) FS_page)
#define FS_PHYS_BLOCK ((uint32_t) FS_block)

// Return values of the following functions
#define FLASH_HAL_OK          (0)
#define FLASH_HAL_READ_ERROR  (-1)
#define FLASH_HAL_WRITE_ERROR (-2)
#define FLASH_HAL_ERASE_ERROR (-3)

extern int32_t flash_hal_write(uint32_t addr, uint32_t size, const uint8_t *src);
extern int32_t flash_hal_erase(uint32_t addr, uint32_t size);
extern int32_t flash_hal_read(uint32_t addr, uint32_t size, uint8_t *dst);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !defined(flash_hal_h)
