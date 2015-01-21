/* 
  i2c.h - esp8266 i2c bit-banging library

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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

#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void i2c_init(int sda_pin, int scl_pin);
void i2c_freq(int freq_hz);
void i2c_release();
void i2c_start();
void i2c_stop();
void i2c_set_ack(int ack);
int i2c_get_ack();
uint8_t i2c_read(void);
void i2c_write(uint8_t val);


size_t i2c_master_read_from(int address, uint8_t* data, size_t size, bool sendStop);
size_t i2c_master_write_to(int address, const uint8_t* data, size_t size, bool sendStop);

// todo: implement i2c slave functions
//
// void i2c_slave_setAddress(uint8_t);
// int  i2c_slave_transmit(const uint8_t* data, size_t size);
// void i2c_slave_attach_rx_callback( void (*)(uint8_t*, int) );
// void i2c_slave_attach_tx_callback( void (*)(void) );

#endif//I2C_H
