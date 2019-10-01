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
/*
  Modified by Doit.am team
  www.doit.am
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

// All digital pins are PWM capable
static const uint8_t D0   = 0;
static const uint8_t D1   = 1; //TX0
static const uint8_t D2   = 2;
static const uint8_t D3   = 3; //RX0
static const uint8_t D4   = 4;
static const uint8_t D5   = 5;
static const uint8_t D12   = 12; // MISO
static const uint8_t D13   = 13; // MOSI
static const uint8_t D14   = 14; // SCK
static const uint8_t D15   = 15; // SS(SDA)
static const uint8_t D16   = 16; // LED_BUILTIN
static const uint8_t RX0   = 3;
static const uint8_t TX0   = 1;

#define LED_BUILTIN 16

#include "../generic/common.h"

#endif /* Pins_Arduino_h */
