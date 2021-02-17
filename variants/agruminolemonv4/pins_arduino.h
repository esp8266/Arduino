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

#define LED_BUILTIN 2

// PINOUT Agrumino Implemented
#define PIN_SDA 2 // [X] BOOT: Must be HIGH at boot
#define PIN_SCL 14 // [X]
#define PIN_PUMP 12 // [X]
#define PIN_BTN_S1 4 // [X] Same as Internal WT8266 LED
#define PIN_USB_DETECT 5 // [X]
#define PIN_MOSFET 15 // [X] BOOT: Must be LOW at boot
#define PIN_BATT_STAT 13 // [X]
#define PIN_LEVEL 0 // [ ] BOOT: HIGH for Running and LOW for Program

static constexpr uint8_t D0   = 16;
static constexpr uint8_t RX   = 3;
static constexpr uint8_t TX   = 1;

#define PIN_WIRE_SDA PIN_SDA
#define PIN_WIRE_SCL PIN_SCL

static constexpr uint8_t SDA = PIN_WIRE_SDA;
static constexpr uint8_t SCL = PIN_WIRE_SCL;

#include "../generic/common.h"

#endif /* Pins_Arduino_h */
