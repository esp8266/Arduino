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

#define PIN_WIRE_SDA (2)
#define PIN_WIRE_SCL (0)

static const uint8_t P0   = 2;
static const uint8_t P1   = 5;
static const uint8_t P2   = 0;
static const uint8_t P3   = 3;
static const uint8_t P4   = 1;
static const uint8_t P5   = 4;
static const uint8_t P6   = 15;
static const uint8_t P7   = 13;
static const uint8_t P8   = 12;
static const uint8_t P9   = 14;
static const uint8_t P10  = 16;
static const uint8_t P11  = 17;

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#define LED_BUILTIN 5

#include "../generic/common.h"

#endif /* Pins_Arduino_h */
