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
#define LED_BUILTIN 1
#endif

// routed
//  on
// ESP01  GPIO
//        16  (no intr, internal-pull-down-not-up)
//         5                   i2c-sclk
//         4                   i2c-sda
//   X     0                                      (boot:flash=up,uart=down)
//   X     2  uart1-tx                  i2so-ws   (boot:up)
//        14                   spi-sclk i2si-ws
//        12                   spi-miso i2si-data
//        13  uart0-swapped-rx spi-mosi i2si-bck
//        15  uart0-swapped-tx spi-ss   i2so-bck  (boot:down)
//   X     3  uart0-rx                  i2so-data
//   X     1  uart0-tx

#include "common.h"

#endif /* Pins_Arduino_h */
