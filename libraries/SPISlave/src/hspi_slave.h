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
#ifndef _HSPISLAVE_H_INCLUDED
#define _HSPISLAVE_H_INCLUDED

#include "Arduino.h"

//Start SPI SLave
void hspi_slave_begin(uint8_t status_len, void * arg);

//End SPI SLave
void hspi_slave_end();

//set the status register so the master can read it
void hspi_slave_setStatus(uint32_t status);

//set the data registers (max 32 bytes at a time)
void hspi_slave_setData(uint8_t *data, uint8_t len);

//set the callbacks
void hspi_slave_onData(void (*rxd_cb)(void *, uint8_t *, uint8_t));
void hspi_slave_onDataSent(void (*txd_cb)(void *));
void hspi_slave_onStatus(void (*rxs_cb)(void *, uint32_t));
void hspi_slave_onStatusSent(void (*txs_cb)(void *));

#endif
