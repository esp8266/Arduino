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
#include "HardwareSerial.h"
typedef struct {
        uint32_t divider;
        uint32_t regValue;
} spiClockDiv_t;

static const spiClockDiv_t spiClockDiv[] = {
    { 0, (0x80000000) },    ///< @80Mhz = 80 MHz       @160Mhz = 160 MHz
    { 2, (0x00001001) },    ///< @80Mhz = 40 MHz       @160Mhz = 80 MHz
    { 4, (0x00041001) },    ///< @80Mhz = 20 MHz       @160Mhz = 40 MHz
    { 6, (0x000fffc0) },    ///< @80Mhz = 16 MHz       @160Mhz = 32 MHz
    { 8, (0x000c1001) },    ///< @80Mhz = 10 MHz       @160Mhz = 20 MHz
    { 10, (0x00101001) },   ///< @80Mhz = 8 MHz        @160Mhz = 16 MHz
    { 16, (0x001c1001) },   ///< @80Mhz = 5 MHz        @160Mhz = 10 MHz
    { 20, (0x00241001) },   ///< @80Mhz = 4 MHz        @160Mhz = 8 MHz
    { 40, (0x004c1001) },   ///< @80Mhz = 2 MHz        @160Mhz = 4 MHz
    { 80, (0x009c1001) },   ///< @80Mhz = 1 MHz        @160Mhz = 2 MHz
    { 160, (0x013c1001) },  ///< @80Mhz = 500 KHz      @160Mhz = 1 MHz
    { 320, (0x027c1001) },  ///< @80Mhz = 250 KHz      @160Mhz = 500 KHz
    { 640, (0x04fc1001) }   ///< @80Mhz = 125 KHz      @160Mhz = 250 KHz
};

static const uint8_t spiClockDiv_count = (sizeof(spiClockDiv) / sizeof(spiClockDiv_t));

SPIClass SPI;

SPIClass::SPIClass() {
}

void SPIClass::begin() {
    pinMode(SCK, SPECIAL);  ///< GPIO14
    pinMode(MISO, SPECIAL); ///< GPIO12
    pinMode(MOSI, SPECIAL); ///< GPIO13

    GPMUX = 0x105; // note crash if SPI flash Frequency < 40MHz
    SPI1C = 0;
    setFrequency(1000000); ///< 1Mhz
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
    setFrequency(settings._clock);
    setBitOrder(settings._bitOrder);
    setDataMode(settings._dataMode);
}

void SPIClass::endTransaction() {
}

void SPIClass::setDataMode(uint8_t dataMode) {

/**
    SPI_MODE0 0x00 - CPOL: 0  CPHA: 0
    SPI_MODE1 0x01 - CPOL: 0  CPHA: 1
    SPI_MODE2 0x10 - CPOL: 1  CPHA: 0
    SPI_MODE3 0x11 - CPOL: 1  CPHA: 1
*/

    bool CPOL = (dataMode&0x10); ///< CPOL (Clock Polarity)
    bool CPHA = (dataMode&0x01); ///< CPHA (Clock Phase)

    if(CPHA) {
        SPI1U |= (SPIUSME);
    } else {
        SPI1U &= ~(SPIUSME);
    }

    if(CPOL) {
        //todo How set CPOL???
    }

}

void SPIClass::setBitOrder(uint8_t bitOrder) {
    if(bitOrder == MSBFIRST) {
        SPI1C &= ~(SPICWBO | SPICRBO);
    } else {
        SPI1C |= (SPICWBO | SPICRBO);
    }
}

void SPIClass::setFrequency(uint32_t freq) {
    uint8_t i = 0;
    // find the best match
    if(freq < F_CPU) {
        for(i = 1; i < (spiClockDiv_count-1); i++) {
            if(freq >= (F_CPU/spiClockDiv[i].divider)) {
                break;
            }
        }
    }
    setClockDivider(spiClockDiv[i].regValue);
}

void SPIClass::setClockDivider(uint32_t clockDiv) {
    SPI1CLK = clockDiv;
}

uint8_t SPIClass::transfer(uint8_t data) {
    while(SPI1CMD & SPIBUSY);
    SPI1W0 = data;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY);
    return (uint8_t) (SPI1W0 & 0xff);
}

uint16_t SPIClass::transfer16(uint16_t data) {
    union {
            uint16_t val;
            struct {
                    uint8_t lsb;
                    uint8_t msb;
            };
    } in, out;
    in.val = data;

    if((SPI1C & (SPICWBO | SPICRBO))) {
        //MSBFIRST
        out.msb = transfer(in.msb);
        out.lsb = transfer(in.lsb);
    } else {
        //LSBFIRST
        out.lsb = transfer(in.lsb);
        out.msb = transfer(in.msb);
    }
    return out.val;
}

