/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"

SPIClass SPI;

SPIClass::SPIClass()
{
#if defined(__AVR_ATmega1280__)
  // PBx
  const static byte SS   = 0;
  const static byte MOSI = 2;
  const static byte MISO = 3;
  const static byte SCK  = 1;
#else
  // PBx
  const static byte SS   = 2;
  const static byte MOSI = 3;
  const static byte MISO = 4;
  const static byte SCK  = 5;
#endif

  // Set direction register for SCK and MOSI pin.
  // MISO pin automatically overrides to INPUT.
  // When the SS pin is set as OUTPUT, it can be used as
  // a general purpose output port (it doesn't influence
  // SPI operations).

  DDRB  |= _BV(SCK) | _BV(MOSI) | _BV(SS);
  PORTB &= ~(_BV(SCK) | _BV(MOSI) | _BV(SS));
  SPCR  = _BV(SPE) | _BV(MSTR);
  
  // Warning: if the SS pin ever becomes a LOW INPUT then SPI 
  // automatically switches to Slave, so the data direction of 
  // the SS pin MUST be kept as OUTPUT.
  SPCR  = _BV(SPE) | _BV(MSTR);
}

void SPIClass::setDataOrder(SPIDataOrder _d)
{
  if (_d == SPI_DataOrder_LSB)
    SPCR |= _BV(DORD); // LSB
  else
    SPCR &= ~_BV(DORD); // MSB
}

void SPIClass::setSPIMode(SPIMode _d)
{
  switch (_d) {
    case SPI_Mode_SampleRising:
      SPCR &= ~(_BV(CPOL) | _BV(CPHA));  
      break;
    case SPI_Mode_SetupRising:
      SPCR &= ~_BV(CPOL);
      SPCR |= _BV(CPHA);
      break;
    case SPI_Mode_SampleFalling:
      SPCR &= ~_BV(CPHA);
      SPCR |= _BV(CPOL);
      break;
    case SPI_Mode_SetupFalling:
      SPCR |= _BV(CPOL) | _BV(CPHA);
      break;
  }
}

void SPIClass::setClockDivider(SPIClockDivider _d)
{
  switch (_d) {
    case SPI_ClkDiv_4:
      SPCR &= ~(_BV(SPR1) | _BV(SPR0));
      break;
    case SPI_ClkDiv_16:
      SPCR &= ~_BV(SPR1);
      SPCR |= _BV(SPR0);
      break;
    case SPI_ClkDiv_64:
      SPCR |= _BV(SPR1);
      SPCR &= ~_BV(SPR0);
      break;
    case SPI_ClkDiv_128:
      SPCR |= _BV(SPR1) | _BV(SPR0);
      break;
  }
}

