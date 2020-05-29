/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis
  Modified for ESP8266 platform by Ivan Grokhotkov, 2014-2015.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif


//                d1-mini  GPIO
static const uint8_t D0  = 16; // (no intr, internal-pull-down-not-up)
static const uint8_t D1  = 5;  //           i2c-sclk
static const uint8_t D2  = 4;  //           i2c-sda
static const uint8_t D3  = 0;  // 10k-up                       (boot:flash=up,uart=down)
static const uint8_t D4  = 2;  // 10k-up LED         i2so-ws   (boot:up)   uart1-tx
static const uint8_t D5  = 14; //           spi-sclk i2si-ws
static const uint8_t D6  = 12; //           spi-miso i2si-data
static const uint8_t D7  = 13; //           spi-mosi i2si-bck              uart0-swapped-rx
static const uint8_t D8  = 15; // 10k-down  spi-ss   i2so-bck  (boot:down) uart0-swapped-tx
static const uint8_t RX  = 3;  // uart0-rx           i2so-data
static const uint8_t TX  = 1;  // uart0-tx

// https://www.wemos.cc/en/latest/d1/d1_mini.html

#include "../generic/common.h"

#endif /* Pins_Arduino_h */
