/*
  SPI.cpp - SPI library for esp8266

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

#include "SPI.h"

SPIClass SPI;

SPIClass::SPIClass(){}

void SPIClass::begin(){
  pinMode(SCK, SPECIAL);
  pinMode(MISO, SPECIAL);
  pinMode(MOSI, SPECIAL);

  GPMUX = 0x105;
  SPI1C = 0;
  SPI1CLK = SPI_CLOCK_DIV16;//1MHz
  SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
  SPI1U1 = (7 << SPILMOSI) | (7 << SPILMISO);
  SPI1C1 = 0;
}

void SPIClass::end() {
  pinMode(SCK, INPUT);
  pinMode(MISO, INPUT);
  pinMode(MOSI, INPUT);
}

void SPIClass::beginTransaction(SPISettings settings) {
  setClockDivider(settings._clock);
  setBitOrder(settings._bitOrder);
  setDataMode(settings._dataMode);
}

void SPIClass::endTransaction() {}

void SPIClass::setDataMode(uint8_t dataMode) {

}

void SPIClass::setBitOrder(uint8_t bitOrder) {
  if (bitOrder == MSBFIRST){
    SPI1C &= ~(SPICWBO | SPICRBO);
  } else {
    SPI1C |= (SPICWBO | SPICRBO);
  }
}

void SPIClass::setClockDivider(uint32_t clockDiv) {
  SPI1CLK = clockDiv;
}

uint8_t SPIClass::transfer(uint8_t data) {
  while(SPI1CMD & SPIBUSY);
  SPI1W0 = data;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY);
  return (uint8_t)(SPI1W0 & 0xff);
}
