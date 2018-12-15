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
#ifdef ARDUINO
extern "C" uint32_t _FS_start;
extern "C" uint32_t _FS_end;
extern "C" uint32_t _FS_page;
extern "C" uint32_t _FS_block;

#define FS_PHYS_ADDR ((uint32_t) (&_FS_start) - 0x40200000)
#define FS_PHYS_SIZE ((uint32_t) (&_FS_end) - (uint32_t) (&_FS_start))
#define FS_PHYS_PAGE ((uint32_t) &_FS_page)
#define FS_PHYS_BLOCK ((uint32_t) &_FS_block)
#endif

// Return values of the following functions
#define FLASH_HAL_OK          (0)
#define FLASH_HAL_READ_ERROR  (-1)
#define FLASH_HAL_WRITE_ERROR (-2)
#define FLASH_HAL_ERASE_ERROR (-3)

extern int32_t flash_hal_write(uint32_t addr, uint32_t size, const uint8_t *src);
extern int32_t flash_hal_erase(uint32_t addr, uint32_t size);
extern int32_t flash_hal_read(uint32_t addr, uint32_t size, uint8_t *dst);

#endif // !defined(flash_hal_h)
