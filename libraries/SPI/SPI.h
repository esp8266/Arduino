/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>
#include "include/SPIdef.h"



struct SPISettings 
{
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) 
  : _clock(clock)
  , _bitOrder(bitOrder)
  , _dataMode(dataMode)
  {
  }

  uint32_t _clock;
  uint8_t  _bitOrder;
  uint8_t  _dataMode;
};

class SPIImpl;

class SPIClass 
{
public:
  SPIClass();
  
  void begin();

  // void usingInterrupt(uint8_t interruptNumber);
  // void beginTransaction(SPISettings settings);
  uint8_t transfer(uint8_t data);
  // uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);
  // void endTransaction(void);

  void end();

  void setBitOrder(uint8_t bitOrder);  
  void setDataMode(uint8_t dataMode);
  void setClockDivider(uint8_t clockDiv);

private:
  SPIImpl* _impl;
};

extern SPIClass SPI;

#endif
