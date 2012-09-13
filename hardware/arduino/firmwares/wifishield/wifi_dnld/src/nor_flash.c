/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <string.h>
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "spi.h"
#include "conf_at45dbx.h"
#include "at45dbx.h"
#include "clocks.h"


void flash_init(void)
{
        static const gpio_map_t AT45DBX_SPI_GPIO_MAP = {
                { AT45DBX_SPI_SCK_PIN, AT45DBX_SPI_SCK_FUNCTION },
                { AT45DBX_SPI_MISO_PIN, AT45DBX_SPI_MISO_FUNCTION },
                { AT45DBX_SPI_MOSI_PIN, AT45DBX_SPI_MOSI_FUNCTION },
                { AT45DBX_SPI_NPCS2_PIN, AT45DBX_SPI_NPCS2_FUNCTION },
        };
        
        spi_options_t spiOptions = {
                .reg          = AT45DBX_SPI_FIRST_NPCS,
                .baudrate     = AT45DBX_SPI_MASTER_SPEED,
                .bits         = AT45DBX_SPI_BITS,
                .spck_delay   = 0,
                .trans_delay  = 0,
                .stay_act     = 1,
                .spi_mode     = 0,
                .modfdis      = 1
        };
    
        gpio_enable_module(AT45DBX_SPI_GPIO_MAP,
                           sizeof(AT45DBX_SPI_GPIO_MAP) /
                           sizeof(AT45DBX_SPI_GPIO_MAP[0]));
        
        spi_initMaster(AT45DBX_SPI, &spiOptions);

        spi_selectionMode(AT45DBX_SPI, 0, 0, 0);
        spi_enable(AT45DBX_SPI);

        /* put up flash reset pin */
        gpio_set_gpio_pin(AT45DBX_CHIP_RESET);

        at45dbx_init(spiOptions, FPBA_HZ);
}

void flash_write(U32 addr, const U8* buf, U32 len)
{
        U32 sector = addr / AT45DBX_SECTOR_SIZE;
        U32 i;
        Assert(addr % AT45DBX_SECTOR_SIZE == 0);
        
        at45dbx_write_open(sector);
        for (i = 0; i < len; i++)
                at45dbx_write_byte(buf[i]);
        at45dbx_write_close();
}

void flash_read(U32 addr, U8* buf, U32 len)
{
        U32 sector = addr / AT45DBX_SECTOR_SIZE;
        U32 i;
        Assert(addr % AT45DBX_SECTOR_SIZE == 0);
        
        at45dbx_read_open(sector);
        for (i = 0; i < len; i++)
                buf[i] = at45dbx_read_byte();
        at45dbx_read_close();
}
