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


static void _startTone(uint8_t _pin, uint32_t high, uint32_t low, unsigned long duration) {
  if (_pin > 16) {
    return;
  }

  pinMode(_pin, OUTPUT);

  high = std::max(high, (uint32_t)25);  // 20KHz maximum tone frequency (new),
  low = std::max(low, (uint32_t)25);   // (25us high + 25us low period = 20KHz)

  if (startWaveform(_pin, high, low, (uint32_t) duration * 1000)) {
    _toneMap |= 1 << _pin;
  }
}


void tone(uint8_t _pin, unsigned int frequency, unsigned long duration) {
  if (frequency == 0) {
    noTone(_pin);
  } else {
    uint32_t period = 1000000L / frequency;
    uint32_t high = period / 2;
    uint32_t low = period - high;
    _startTone(_pin, high, low, duration);
  }
}


// Separate tone(float) to hopefully not pull in floating point libs unless
// it's called with a float.
void tone(uint8_t _pin, double frequency, unsigned long duration) {
  if (frequency < 1.0) { // FP means no exact comparisons
    noTone(_pin);
  } else {
    double period = 1000000.0 / frequency;
    uint32_t high = (uint32_t)((period / 2.0) + 0.5);
    uint32_t low = (uint32_t)(period + 0.5) - high;
    _startTone(_pin, high, low, duration);
  }
}


// Fix ambiguous tone() binding when adding in a duration
void tone(uint8_t _pin, int frequency, unsigned long duration) {
  // Call the unsigned int version of the function explicitly
  tone(_pin, (unsigned int)frequency, duration);
}


void noTone(uint8_t _pin) {
  if (_pin > 16) {
    return;
  }
  stopWaveform(_pin);
  _toneMap &= ~(1 << _pin);
  digitalWrite(_pin, 0);
}
