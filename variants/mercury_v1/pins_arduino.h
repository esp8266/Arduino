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

#include "../generic/common.h"

#define LED_BUILTIN 0
#define BUILTIN_LED LED_BUILTIN

#define A0 (17)

static const uint8_t D0   = 0;
static const uint8_t D1   = 12;
static const uint8_t D2   = 4;
static const uint8_t D3   = 16;
static const uint8_t D4   = 5;
static const uint8_t D5   = 13;
static const uint8_t D6   = 15;
static const uint8_t D7   = 2;
static const uint8_t D8   = 14;
static const uint8_t D9   = 9;
static const uint8_t D10  = 10;

#define PIN_WIRE_SDA (2)
#define PIN_WIRE_SCL (14)

// Brushed DC Motors
#define MOTOR_1_DIR (16)
#define MOTOR_1_PWM (12)
#define MOTOR_2_DIR (5)
#define MOTOR_2_PWM (4)

//Ultrasonic Sensor
static const uint8_t USST = D7;
static const uint8_t USSE = D8;

//Servo
static const uint8_t SERVO1 = D4;
static const uint8_t SERVO2 = D6;
static const uint8_t SERVO3 = D3;
static const uint8_t SERVO4 = D5;

//IR
static const uint8_t IR1 = D9;
static const uint8_t IR2 = D10;

#endif /* Pins_Arduino_h */
