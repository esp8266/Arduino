/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis
  Modified for ESP8266 WIFIO board by Ivan Grokhotkov, 2015.

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

#define NUM_DIGITAL_PINS  36
#define NUM_ANALOG_INPUTS 1
#define ESP_PINS_OFFSET 20

#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#define PIN_SPI_SS   (12)
#define PIN_SPI_MOSI (13)
#define PIN_SPI_MISO (14)
#define PIN_SPI_SCK  (15)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define PIN_A0 (14)
#define PIN_A1 (15)
#define PIN_A2 (16)
#define PIN_A3 (17)
#define PIN_A4 (18)
#define PIN_A5 (19)
#define PIN_A6 (20)
#define PIN_A7 (21)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;

static const uint8_t E0 = ESP_PINS_OFFSET + 0;
static const uint8_t E1 = ESP_PINS_OFFSET + 1;
static const uint8_t E2 = ESP_PINS_OFFSET + 2;
static const uint8_t E3 = ESP_PINS_OFFSET + 3;
static const uint8_t E4 = ESP_PINS_OFFSET + 4;
static const uint8_t E5 = ESP_PINS_OFFSET + 5;
static const uint8_t E11 = ESP_PINS_OFFSET + 11;
static const uint8_t E12 = ESP_PINS_OFFSET + 12;
static const uint8_t E13 = ESP_PINS_OFFSET + 13;
static const uint8_t E14 = ESP_PINS_OFFSET + 14;
static const uint8_t E15 = ESP_PINS_OFFSET + 15;
static const uint8_t E16 = ESP_PINS_OFFSET + 16;

#define LED_BUILTIN 2

#define SERIAL_PORT_MONITOR        Serial
#define SERIAL_PORT_USBVIRTUAL     Serial
#define SERIAL_PORT_HARDWARE       Serial
#define SERIAL_PORT_HARDWARE_OPEN  Serial

#ifdef LED_BUILTIN
#ifdef __cplusplus
extern "C"
#endif
const int BUILTIN_LED __attribute__((deprecated, weak)) = LED_BUILTIN;
#endif

#endif /* Pins_Arduino_h */
