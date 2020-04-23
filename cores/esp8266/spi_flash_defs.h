/*
 spi_flash_defs.h - SPI Flash chip commands and status registers
 Copyright (c) 2019 Mike Nix. All rights reserved.
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

#ifndef SPI_FLASH_DEFS_H
#define SPI_FLASH_DEFS_H

// Flash chip Status Register 3: Vendor XMC Output Drive levels
#define SPI_FLASH_SR3_XMC_DRV_25  1
#define SPI_FLASH_SR3_XMC_DRV_50  0
#define SPI_FLASH_SR3_XMC_DRV_75  2
#define SPI_FLASH_SR3_XMC_DRV_100 3

#define SPI_FLASH_SR3_XMC_DRV_S   5
#define SPI_FLASH_SR3_XMC_DRV_MASK 0x03

// Flash Chip commands
#define SPI_FLASH_CMD_RSR1  0x05  //Read Flash Status Register...
#define SPI_FLASH_CMD_RSR2  0x35
#define SPI_FLASH_CMD_RSR3  0x15
#define SPI_FLASH_CMD_WSR1  0x01  //Write Flash Status Register...
#define SPI_FLASH_CMD_WSR2  0x31
#define SPI_FLASH_CMD_WSR3  0x11
#define SPI_FLASH_CMD_WEVSR 0x50  //Write Enable Volatile Status Registers
#define SPI_FLASH_CMD_WREN  0x06  //Write Enable
#define SPI_FLASH_CMD_WRDI  0x04  //Write Disable

#endif // SPI_FLASH_DEFS_H
