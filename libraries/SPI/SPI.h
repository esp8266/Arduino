/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <stdio.h>
#include <WProgram.h>
#include <avr/pgmspace.h>

enum SPIClockDivider {
  SPI_ClkDiv_4,
  SPI_ClkDiv_16,
  SPI_ClkDiv_64,
  SPI_ClkDiv_128,
};

enum SPIDataOrder {
  SPI_DataOrder_LSB,
  SPI_DataOrder_MSB,
};

enum SPIMode {
  SPI_Mode_SampleRising,
  SPI_Mode_SetupRising,
  SPI_Mode_SampleFalling,
  SPI_Mode_SetupFalling,
};

class SPIClass {
public:
  SPIClass();

  inline static byte transfer(byte _data);

  // SPI Configuration methods

  inline static void attachInterrupt();
  inline static void detachInterrupt(); // Default

  inline static void begin(); // Default
  inline static void end();

  static void setDataOrder(SPIDataOrder);

  static void setSPIMode(SPIMode);

  static void setClockDivider(SPIClockDivider);
  inline static void doubleSpeed();
  inline static void noDoubleSpeed(); // Default
};

extern SPIClass SPI;

byte SPIClass::transfer(byte _data) {
  SPDR = _data;
  while (!(SPSR & _BV(SPIF)))
    ;
  return SPDR;
}

void SPIClass::attachInterrupt() {
  SPCR |= _BV(SPIE);
}

void SPIClass::detachInterrupt() {
  SPCR &= ~_BV(SPIE);
}

void SPIClass::begin() {
  SPCR |= _BV(SPE);
}

void SPIClass::end() {
  SPCR &= ~_BV(SPE);
}

void SPIClass::doubleSpeed() {
  SPSR |= _BV(SPI2X);
}

void SPIClass::noDoubleSpeed() {
  SPSR &= ~_BV(SPI2X);
}

#endif
