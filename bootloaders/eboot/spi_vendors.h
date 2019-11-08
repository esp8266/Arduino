/*
 spi_vendors.h - Vendor IDs for SPI chips
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

#ifndef SPI_VENDORS_H
#define SPI_VENDORS_H

// Vendor IDs taken from Flashrom project
// https://review.coreboot.org/cgit/flashrom.git/tree/flashchips.h?h=1.0.x
// Moved here from ../../cores/esp8266/Esp.h
typedef enum {
    SPI_FLASH_VENDOR_ALLIANCE    = 0x52,    /* Alliance Semiconductor */
    SPI_FLASH_VENDOR_AMD         = 0x01,    /* AMD */
    SPI_FLASH_VENDOR_AMIC        = 0x37,    /* AMIC */
    SPI_FLASH_VENDOR_ATMEL       = 0x1F,    /* Atmel (now used by Adesto) */
    SPI_FLASH_VENDOR_BRIGHT      = 0xAD,    /* Bright Microelectronics */
    SPI_FLASH_VENDOR_CATALYST    = 0x31,    /* Catalyst */
    SPI_FLASH_VENDOR_EON         = 0x1C,    /* EON Silicon Devices, missing 0x7F prefix */
    SPI_FLASH_VENDOR_ESMT        = 0x8C,    /* Elite Semiconductor Memory Technology (ESMT) / EFST Elite Flash Storage */
    SPI_FLASH_VENDOR_EXCEL       = 0x4A,    /* ESI, missing 0x7F prefix */
    SPI_FLASH_VENDOR_FIDELIX     = 0xF8,    /* Fidelix */
    SPI_FLASH_VENDOR_FUJITSU     = 0x04,    /* Fujitsu */
    SPI_FLASH_VENDOR_GIGADEVICE  = 0xC8,    /* GigaDevice */
    SPI_FLASH_VENDOR_HYUNDAI     = 0xAD,    /* Hyundai */
    SPI_FLASH_VENDOR_INTEL       = 0x89,    /* Intel */
    SPI_FLASH_VENDOR_ISSI        = 0xD5,    /* ISSI Integrated Silicon Solutions, see also PMC. */
    SPI_FLASH_VENDOR_MACRONIX    = 0xC2,    /* Macronix (MX) */
    SPI_FLASH_VENDOR_NANTRONICS  = 0xD5,    /* Nantronics, missing prefix */
    SPI_FLASH_VENDOR_PMC         = 0x9D,    /* PMC, missing 0x7F prefix */
    SPI_FLASH_VENDOR_PUYA        = 0x85,    /* Puya semiconductor (shanghai) co. ltd */
    SPI_FLASH_VENDOR_SANYO       = 0x62,    /* Sanyo */
    SPI_FLASH_VENDOR_SHARP       = 0xB0,    /* Sharp */
    SPI_FLASH_VENDOR_SPANSION    = 0x01,    /* Spansion, same ID as AMD */
    SPI_FLASH_VENDOR_SST         = 0xBF,    /* SST */
    SPI_FLASH_VENDOR_ST          = 0x20,    /* ST / SGS/Thomson / Numonyx (later acquired by Micron) */
    SPI_FLASH_VENDOR_SYNCMOS_MVC = 0x40,    /* SyncMOS (SM) and Mosel Vitelic Corporation (MVC) */
    SPI_FLASH_VENDOR_TENX        = 0x5E,    /* Tenx Technologies */
    SPI_FLASH_VENDOR_TI          = 0x97,    /* Texas Instruments */
    SPI_FLASH_VENDOR_TI_OLD      = 0x01,    /* TI chips from last century */
    SPI_FLASH_VENDOR_WINBOND     = 0xDA,    /* Winbond */
    SPI_FLASH_VENDOR_WINBOND_NEX = 0xEF,    /* Winbond (ex Nexcom) serial flashes */
    SPI_FLASH_VENDOR_XMC         = 0x20,    /* Wuhan Xinxin Semiconductor Manufacturing Corp */

    SPI_FLASH_VENDOR_UNKNOWN     = 0xFF
} SPI_FLASH_VENDOR_t;

#endif // __SPI_VENDORS_H__
