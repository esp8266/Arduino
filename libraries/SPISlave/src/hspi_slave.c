/*
  SPISlave library for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
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
#include "hspi_slave.h"
#include "esp8266_peri.h"
#include "ets_sys.h"

static void (*_hspi_slave_rx_data_cb)(void * arg, uint8_t * data, uint8_t len) = NULL;
static void (*_hspi_slave_tx_data_cb)(void * arg) = NULL;
static void (*_hspi_slave_rx_status_cb)(void * arg, uint32_t data) = NULL;
static void (*_hspi_slave_tx_status_cb)(void * arg) = NULL;
static uint8_t _hspi_slave_buffer[33];

void ICACHE_RAM_ATTR _hspi_slave_isr_handler(void *arg)
{
    uint32_t status;
    uint32_t istatus;

    istatus = SPIIR;

    if(istatus & (1 << SPII1)) { //SPI1 ISR
        status = SPI1S;
        SPI1S &= ~(0x3E0);//disable interrupts
        SPI1S |= SPISSRES;//reset
        SPI1S &= ~(0x1F);//clear interrupts
        SPI1S |= (0x3E0);//enable interrupts

        if((status & SPISRBIS) != 0 && (_hspi_slave_tx_data_cb)) {
            _hspi_slave_tx_data_cb(arg);
        }
        if((status & SPISRSIS) != 0 && (_hspi_slave_tx_status_cb)) {
            _hspi_slave_tx_status_cb(arg);
        }
        if((status & SPISWSIS) != 0 && (_hspi_slave_rx_status_cb)) {
            uint32_t s = SPI1WS;
            _hspi_slave_rx_status_cb(arg, s);
        }
        if((status & SPISWBIS) != 0 && (_hspi_slave_rx_data_cb)) {
            uint8_t i;
            uint32_t data;
            uint8_t buffer[33];
            _hspi_slave_buffer[32] = 0;
            for(i=0; i<8; i++) {
                data=SPI1W(i);
                _hspi_slave_buffer[i<<2] = data & 0xff;
                _hspi_slave_buffer[(i<<2)+1] = (data >> 8) & 0xff;
                _hspi_slave_buffer[(i<<2)+2] = (data >> 16) & 0xff;
                _hspi_slave_buffer[(i<<2)+3] = (data >> 24) & 0xff;
            }
            _hspi_slave_rx_data_cb(arg, &_hspi_slave_buffer[0], 32);
        }
    } else if(istatus & (1 << SPII0)) { //SPI0 ISR
        SPI0S &= ~(0x3ff);//clear SPI ISR
    } else if(istatus & (1 << SPII2)) {} //I2S ISR
}

void hspi_slave_begin(uint8_t status_len, void * arg)
{
    status_len &= 7;
    if(status_len > 4) {
        status_len = 4;    //max 32 bits
    }
    if(status_len == 0) {
        status_len = 1;    //min 8 bits
    }

    pinMode(SS, SPECIAL);
    pinMode(SCK, SPECIAL);
    pinMode(MISO, SPECIAL);
    pinMode(MOSI, SPECIAL);

    SPI1S = SPISE | SPISBE | 0x3E0;
    SPI1U = SPIUMISOH | SPIUCOMMAND | SPIUSSE;
    SPI1CLK = 0;
    SPI1U2 = (7 << SPILCOMMAND);
    SPI1S1 = (((status_len * 8) - 1) << SPIS1LSTA) | (0xff << SPIS1LBUF) | (7 << SPIS1LWBA) | (7 << SPIS1LRBA) | SPIS1RSTA;
    SPI1P = (1 << 19);
    SPI1CMD = SPIBUSY;

    ETS_SPI_INTR_ATTACH(_hspi_slave_isr_handler,arg);
    ETS_SPI_INTR_ENABLE();
}

void hspi_slave_setStatus(uint32_t status)
{
    SPI1WS = status;
}

void hspi_slave_setData(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint32_t out = 0;
    uint8_t bi = 0;
    uint8_t wi = 8;

    for(i=0; i<32; i++) {
        out |= (i<len)?(data[i] << (bi * 8)):0;
        bi++;
        bi &= 3;
        if(!bi) {
            SPI1W(wi) = out;
            out = 0;
            wi++;
        }
    }
}

void hspi_slave_onData(void (*rxd_cb)(void *, uint8_t *, uint8_t))
{
    _hspi_slave_rx_data_cb = rxd_cb;
}

void hspi_slave_onDataSent(void (*txd_cb)(void *))
{
    _hspi_slave_tx_data_cb = txd_cb;
}

void hspi_slave_onStatus(void (*rxs_cb)(void *, uint32_t))
{
    _hspi_slave_rx_status_cb = rxs_cb;
}

void hspi_slave_onStatusSent(void (*txs_cb)(void *))
{
    _hspi_slave_tx_status_cb = txs_cb;
}
