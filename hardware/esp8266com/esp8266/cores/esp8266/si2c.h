/* 
  si2c.h - Software I2C library for esp8266

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
#ifndef SI2C_h
#define SI2C_h
#include "Arduino.h"

void twi_init(uint8_t sda, uint8_t scl);
void twi_stop(void);
void twi_setClock(uint32_t freq);
uint8_t twi_writeTo(uint8_t address, uint8_t * buf, uint32_t len, uint8_t sendStop);
uint8_t twi_readFrom(uint8_t address, uint8_t * buf, uint32_t len, uint8_t sendStop);

#endif