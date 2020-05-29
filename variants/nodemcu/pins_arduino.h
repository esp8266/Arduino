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
#define LED_BUILTIN_AUX 16

//                nodemcu  GPIO
static const uint8_t D0   = 16; // (no pwm, no intr, internal-pull-down-poosible-no-pullup)
static const uint8_t D1   = 5;  //                i2c-sclk
static const uint8_t D2   = 4;  //                i2c-sda
static const uint8_t D3   = 0;  // 12k pull-up                               (boot:flash=pull-up,uart=down)
static const uint8_t D4   = 2;  // 12k pull-up,   LED,      uart1-TX         (boot:pull-up)
static const uint8_t D5   = 14; //                spi-sclk
static const uint8_t D6   = 12; //                spi-miso
static const uint8_t D7   = 13; //                spi-mosi, uart0-swapped-rx
static const uint8_t D8   = 15; // 12k pull-down, spi-ss,   uart0-swapped-tx (boot:pull-down)
static const uint8_t D9   = 3;  // uart0-rx
static const uint8_t D10  = 1;  // uart0-tx

// https://github.com/nodemcu/nodemcu-devkit-v1.0/blob/master/NODEMCU_DEVKIT_V1.0.PDF

#include "../generic/common.h"

#endif /* Pins_Arduino_h */
