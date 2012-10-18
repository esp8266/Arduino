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
#include <board_init.h>
#include <nvram.h>


static struct nvram {
        uint8_t read;
        void *data;
        uint32_t len;
        uint16_t off;
} PRIV;

int nvram_init(void)
{
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
    
        at45dbx_init(spiOptions, FPBA_HZ);
        return 0;
}


/**
 * Invoked by at45dbx driver
 *
 */
void at45dbx_read_multiple_sector_callback(const void *psector)
{
        struct nvram *priv = &PRIV;
        const uint8_t *buf = psector;

        if (!priv->read)
                return;

        memcpy(priv->data, buf + priv->off, priv->len);
}


/**
 * Invoked by at45dbx driver
 *
 */
void at45dbx_write_multiple_sector_callback(void *psector)
{
        struct nvram *priv = &PRIV;
        uint8_t *buf = psector;
        memcpy(buf + priv->off, priv->data, priv->len);
}


/**
 * Write/read any number bytes into any offset of nor flash by taking care
 * of cases where the length is not aligned to the sector size or where
 * the addr is not aligned to the sector offsets.
 *
 */
static int nvram_rw(uint32_t addr, void *data, uint16_t len, int write)
{
        struct nvram *priv = &PRIV;
        priv->read = write ? 0 : 1;
        
        while (len) {
                uint32_t sector = addr / AT45DBX_SECTOR_SIZE;
                priv->data = data;
                priv->off = addr % AT45DBX_SECTOR_SIZE;
                priv->len = AT45DBX_SECTOR_SIZE;

                if (len < AT45DBX_SECTOR_SIZE)
                        priv->len = len;

                if (priv->len > AT45DBX_SECTOR_SIZE - priv->off)
                        priv->len = AT45DBX_SECTOR_SIZE - priv->off;

                at45dbx_read_open(sector);
                at45dbx_read_multiple_sector(1);
                at45dbx_read_close();

                if (write) {
                        at45dbx_write_open(sector);
                        at45dbx_write_multiple_sector(1);
                        at45dbx_write_close();
                }
                
                data += priv->len;
                len -= priv->len;
                addr += priv->len;
        }
        
        return 0;
}

/**
 * Write any number bytes into any offset of nor flash.
 *
 */
int nvram_write(uint32_t addr, const void *data, uint32_t len)
{
        return nvram_rw(addr, (void *) data, len, 1);
}


/**
 * Read any number bytes into any offset of nor flash.
 *
 */
int nvram_read(uint32_t addr, void *data, uint32_t len)
{
        return nvram_rw(addr, data, len, 0);
}
