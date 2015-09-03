/* 
  twi.h - Software I2C library for esp8266

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

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct {
    unsigned char dcount, sda, scl;
} TwiConfig;

void twi_init(TwiConfig* twi, unsigned char sda, unsigned char scl);
void twi_stop(TwiConfig* twi);
void twi_setClock(TwiConfig* twi, unsigned int freq);
uint8_t twi_writeTo(TwiConfig* twi, unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop);
uint8_t twi_readFrom(TwiConfig* twi, unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop);

#ifdef __cplusplus
}
#endif

#endif