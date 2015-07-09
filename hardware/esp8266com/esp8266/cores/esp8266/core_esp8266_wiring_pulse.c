/* 
  pulse.c - wiring pulseIn implementation for esp8266

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
#include "wiring_private.h"
#include "pins_arduino.h"

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
  pinMode(pin, INPUT);
  uint32_t start = micros();
  while(digitalRead(pin) == state && (micros() - start) < timeout);
  while(digitalRead(pin) != state && (micros() - start) < timeout);
  start = micros();
  while(digitalRead(pin) == state && (micros() - start) < timeout);
  return micros() - start;
}
