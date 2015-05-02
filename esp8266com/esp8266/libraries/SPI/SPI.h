/*
  SPI.h - SPI library for esp8266

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
#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>
#include <stdlib.h>

#define FCPU80 80000000L

#if F_CPU == FCPU80
#define SPI_CLOCK_DIV80M  0x80000000 //80 MHz
#define SPI_CLOCK_DIV40M  0x00001001 //40 MHz
#define SPI_CLOCK_DIV20M  0x00041001 //20 MHz
#define SPI_CLOCK_DIV16M  0x000fffc0 //16 MHz
#define SPI_CLOCK_DIV10M  0x000c1001 //10 MHz
#define SPI_CLOCK_DIV2    0x00101001 //8 MHz
#define SPI_CLOCK_DIV5M   0x001c1001 //5 MHz
#define SPI_CLOCK_DIV4    0x00241001 //4 MHz
#define SPI_CLOCK_DIV8    0x004c1001 //2 MHz
#define SPI_CLOCK_DIV16   0x009c1001 //1 MHz
#define SPI_CLOCK_DIV32   0x013c1001 //500 KHz
#define SPI_CLOCK_DIV64   0x027c1001 //250 KHz
#define SPI_CLOCK_DIV128  0x04fc1001 //125 KHz
#else
#define SPI_CLOCK_DIV160M 0x80000000 //160 MHz
#define SPI_CLOCK_DIV80M  0x00001001 //80 MHz
#define SPI_CLOCK_DIV40M  0x00041001 //40 MHz
#define SPI_CLOCK_DIV32M  0x000fffc0 //32 MHz
#define SPI_CLOCK_DIV20M  0x000c1001 //20 MHz
#define SPI_CLOCK_DIV16M  0x00101001 //16 MHz
#define SPI_CLOCK_DIV10M  0x001c1001 //10 MHz
#define SPI_CLOCK_DIV2    0x00241001 //8 MHz
#define SPI_CLOCK_DIV4    0x004c1001 //4 MHz
#define SPI_CLOCK_DIV8    0x009c1001 //2 MHz
#define SPI_CLOCK_DIV16   0x013c1001 //1 MHz
#define SPI_CLOCK_DIV32   0x027c1001 //500 KHz
#define SPI_CLOCK_DIV64   0x04fc1001 //250 KHz
#endif

const uint8_t SPI_MODE0 = 0x00;
const uint8_t SPI_MODE1 = 0x04;
const uint8_t SPI_MODE2 = 0x08;
const uint8_t SPI_MODE3 = 0x0C;

class SPISettings {
public:
  SPISettings() :_clock(SPI_CLOCK_DIV16), _bitOrder(LSBFIRST), _dataMode(SPI_MODE0){}
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) :_clock(clock), _bitOrder(bitOrder), _dataMode(dataMode){}
  uint32_t _clock;
  uint8_t  _bitOrder;
  uint8_t  _dataMode;
};

class SPIClass {
public:
  SPIClass();
  void begin();
  void end();
  void setBitOrder(uint8_t bitOrder);
  void setDataMode(uint8_t dataMode);
  void setClockDivider(uint32_t clockDiv);
  void beginTransaction(SPISettings settings);
  uint8_t transfer(uint8_t data);
  void endTransaction(void);
};

extern SPIClass SPI;

#endif