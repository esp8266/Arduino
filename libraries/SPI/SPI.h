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

#define SPI_HAS_TRANSACTION 1

// This defines are not representing the real Divider of the ESP8266
// the Defines match to an AVR Arduino on 16MHz for better compatibility
#define SPI_CLOCK_DIV2 		0x00101001 //8 MHz
#define SPI_CLOCK_DIV4 		0x00241001 //4 MHz
#define SPI_CLOCK_DIV8 		0x004c1001 //2 MHz
#define SPI_CLOCK_DIV16 	0x009c1001 //1 MHz
#define SPI_CLOCK_DIV32 	0x013c1001 //500 KHz
#define SPI_CLOCK_DIV64 	0x027c1001 //250 KHz
#define SPI_CLOCK_DIV128 	0x04fc1001 //125 KHz

const uint8_t SPI_MODE0 = 0x00; ///<  CPOL: 0  CPHA: 0
const uint8_t SPI_MODE1 = 0x01; ///<  CPOL: 0  CPHA: 1
const uint8_t SPI_MODE2 = 0x10; ///<  CPOL: 1  CPHA: 0
const uint8_t SPI_MODE3 = 0x11; ///<  CPOL: 1  CPHA: 1

class SPISettings {
public:
  SPISettings() :_clock(1000000), _bitOrder(LSBFIRST), _dataMode(SPI_MODE0){}
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) :_clock(clock), _bitOrder(bitOrder), _dataMode(dataMode){}
  uint32_t _clock;
  uint8_t  _bitOrder;
  uint8_t  _dataMode;
};

class SPIClass {
public:
  SPIClass();
  bool pins(int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
  void begin();
  void end();
  void setHwCs(bool use);
  void setBitOrder(uint8_t bitOrder);  
  void setDataMode(uint8_t dataMode);
  void setFrequency(uint32_t freq);
  void setClockDivider(uint32_t clockDiv);
  void beginTransaction(const SPISettings& settings);
  uint8_t transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, uint16_t count);
  void write(uint8_t data);
  void write16(uint16_t data);
  void write16(uint16_t data, bool msb);
  void write32(uint32_t data);
  void write32(uint32_t data, bool msb);
  void writeBytes(const uint8_t * data, uint32_t size);
  void writePattern(const uint8_t * data, uint8_t size, uint32_t repeat);
  void transferBytes(const uint8_t * out, uint8_t * in, uint32_t size);
  void endTransaction(void);
private:
  bool useHwCs;
  uint8_t pinSet;
  void writeBytes_(const uint8_t * data, uint8_t size);
  void transferBytes_(const uint8_t * out, uint8_t * in, uint8_t size);
  void transferBytesAligned_(const uint8_t * out, uint8_t * in, uint8_t size);
  inline void setDataBits(uint16_t bits);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPI)
extern SPIClass SPI;
#endif

#endif
