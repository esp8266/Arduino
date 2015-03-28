/* 
  i2c.cpp - esp8266 i2c bit-banging library

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

#include <Arduino.h>
#include "i2c.h"

extern "C" {
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "debug.h"
}

static uint8_t s_sda_pin = 0;
static uint8_t s_scl_pin = 2;
static uint32_t s_i2c_delay = 5;

static inline void i2c_digital_write(int pin, int val)
{
	uint32_t mask = 1 << pin;
    if (val)
      GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, mask);
    else
      GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, mask);
}

static inline void i2c_set(int sda, int scl)
{
	i2c_digital_write(s_sda_pin, sda);
	i2c_digital_write(s_scl_pin, scl);
}

static inline void i2c_set_sda(int sda)
{
	i2c_digital_write(s_sda_pin, sda);
}

static inline void i2c_set_scl(int scl)
{
	i2c_digital_write(s_scl_pin, scl);
}

static inline uint8_t i2c_get_sda()
{
	return GPIO_INPUT_GET(GPIO_ID_PIN(s_sda_pin));
}

static inline uint8_t i2c_get_scl()
{
	return GPIO_INPUT_GET(GPIO_ID_PIN(s_scl_pin));
}


static inline void i2c_wait()
{
	delayMicroseconds(s_i2c_delay);
}

void i2c_freq(int freq_hz)
{
	s_i2c_delay = 1000000 / freq_hz / 4 - 1;
	if (s_i2c_delay < 0)
		s_i2c_delay = 0;
}

void i2c_init(int sda_pin, int scl_pin)
{
	s_sda_pin = sda_pin;
	s_scl_pin = scl_pin;
	pinMode(ESP_PINS_OFFSET + sda_pin, OUTPUT_OPEN_DRAIN);
	pinMode(ESP_PINS_OFFSET + scl_pin, OUTPUT_OPEN_DRAIN);
	i2c_set(1, 1);
	i2c_wait();
}


void i2c_release()
{
	pinMode(ESP_PINS_OFFSET + s_sda_pin, INPUT);
	pinMode(ESP_PINS_OFFSET + s_scl_pin, INPUT);
}

void i2c_start()
{
	i2c_set(1, 1);
	i2c_wait();
	i2c_set_sda(0);
	i2c_wait();
	i2c_wait();
	i2c_set_scl(0);
	i2c_wait();
}

void i2c_stop()
{
	i2c_wait();
    i2c_set_scl(1);
    i2c_wait();
    i2c_set_sda(1);
    i2c_wait();
}

void i2c_set_ack(int ack)
{
    i2c_set_sda(!ack);
    i2c_wait();
    i2c_set_scl(1);
    i2c_wait();
    i2c_set_scl(0);
    i2c_wait();
    i2c_set_sda(0);
    i2c_wait();
}

int i2c_get_ack()
{
	i2c_set_scl(1);
	i2c_wait();
	int result = i2c_get_sda();
	i2c_set_scl(0);
	i2c_wait();
	i2c_set_sda(0);
	i2c_wait();
	return result;
}

uint8_t i2c_read(void)
{
	uint8_t result = 0;
	for (int i = 7; i >= 0; --i)
	{
		i2c_wait();
		i2c_set_scl(1);
		i2c_wait();
		result <<= 1;
		result |= i2c_get_sda();
		i2c_set_scl(0);
	}
	return result;
}


void i2c_write(uint8_t val)
{
	for (int i = 7; i >= 0; --i)
	{
		i2c_set_sda((val >> i) & 1);
		i2c_wait();
		i2c_set_scl(1);
		i2c_wait();
		i2c_set_scl(0);
	}
	i2c_wait();
	i2c_set_sda(1);
}

size_t i2c_master_read_from(int address, uint8_t* data, size_t size, bool sendStop)
{
	i2c_start();
	i2c_write(address << 1 | 1);
	int ack = i2c_get_ack();
	uint8_t* end = data + size;
	for (;data != end; ++data )
	{
		i2c_set_sda(1);
		pinMode(ESP_PINS_OFFSET + s_sda_pin, INPUT);
		*data = i2c_read();
		pinMode(ESP_PINS_OFFSET + s_sda_pin, OUTPUT_OPEN_DRAIN);
		if (data == end - 1)
			i2c_set_ack(0);
		else
			i2c_set_ack(1);
	}
	if (sendStop)
		i2c_stop();
	return size;
}

size_t i2c_master_write_to(int address, const uint8_t* data, size_t size, bool sendStop)
{
	i2c_start();
	i2c_write(address << 1);
	int ack = i2c_get_ack();
	const uint8_t* end = data + size;
	for (;data != end; ++data )
	{
		i2c_write(*data);
		ack = i2c_get_ack();
	}
	if (sendStop)
		i2c_stop();
	return size;
}

// some stubs for libraries that use private twi.h interface
extern "C"
{
  void twi_init(void) { }

  void twi_setAddress(uint8_t) { }

  uint8_t twi_readFrom(uint8_t addr, uint8_t* data, uint8_t size, uint8_t sendStop)
  {
  	return i2c_master_read_from(addr, data, size, sendStop);
  }

  uint8_t twi_writeTo(uint8_t addr, uint8_t* data, uint8_t size, uint8_t wait, uint8_t sendStop)
  {
  	return i2c_master_write_to(addr, data, size, sendStop);
  }

  uint8_t twi_transmit(const uint8_t* data, uint8_t length)
  {

  }


  void twi_attachSlaveRxEvent( void (*)(uint8_t*, int) ) { }
  void twi_attachSlaveTxEvent( void (*)(void) ) { }
  void twi_reply(uint8_t) { }
  void twi_stop(void) 
  {
  	i2c_stop();
  }

  void twi_releaseBus(void)
  {
  	i2c_set(1, 1);
  }
}

