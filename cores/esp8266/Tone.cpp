/*
  Tone.cpp

  A Tone Generator Library for the ESP8266

  Original Copyright (c) 2016 Ben Pirt. All rights reserved.
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

#include "Arduino.h"
#include "core_esp8266_waveform.h"

// Which pins have a tone running on them?
static uint32_t _toneMap = 0;

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration) {
  if (_pin > 16) {
    return;
  }

  if (frequency == 0) {
    noTone(_pin);
    return;
  }

  uint32_t halfCycle = 500000L / frequency;
  if (halfCycle < 100) {
    halfCycle = 100;
  }

  if (startWaveform(_pin, halfCycle, halfCycle, (uint32_t) duration * 1000)) {
    _toneMap |= 1 << _pin;
  }
}

void noTone(uint8_t _pin) {
  if (_pin > 16) {
    return;
  }
  stopWaveform(_pin);
  _toneMap &= ~(1 << _pin);
  digitalWrite(_pin, 0);
}
