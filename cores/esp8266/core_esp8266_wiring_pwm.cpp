/*
  pwm.c - analogWrite implementation for esp8266

  Use the shared TIMER1 utilities to generate PWM signals

  Original Copyright (c) 2015 Hristo Gochkov. All rights reserved.
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

#include <Arduino.h>
#include "core_esp8266_waveform.h"

extern "C" {

static int32_t analogScale = 255;  // Match upstream default, breaking change from 2.x.x


static uint32_t analogMap = 0;
static uint16_t analogFreq = 1000;

extern void __analogWriteFreq(uint32_t freq) {
  if (freq < 100) {
    analogFreq = 100;
  } else if (freq > 60000) {
    analogFreq = 60000;
  } else {
    analogFreq = freq;
  }
  _setPWMFreq(freq);
}

extern void __analogWrite(uint8_t pin, int val) {
  if (pin > 16) {
    return;
  }
  bool openDrain = false;
  if (analogMap & 1UL << pin) {
    openDrain = GPC(pin) & (1 << GPCD);
  }
  analogWriteMode(pin, val, openDrain);
}

extern void __analogWriteMode(uint8_t pin, int val, bool openDrain) {
  if (pin > 16) {
    return;
  }
  if (val < 0) {
    val = 0;
  } else if (val > analogScale) {
    val = analogScale;
  }

  if (analogMap & 1UL << pin) {
    analogMap &= ~(1 << pin);
    const bool isOpenDrain = GPC(pin) & (1 << GPCD);
    if (isOpenDrain != openDrain) {
      GPC(pin) ^= (1 << GPCD);
    }
  }
  else {
    pinMode(pin, openDrain ? OUTPUT_OPEN_DRAIN : OUTPUT);
  }
  
  // Per the Arduino docs at https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  // val: the duty cycle: between 0 (always off) and 255 (always on).
  // So if val = 0 we have digitalWrite(LOW), if we have val==range we have digitalWrite(HIGH)
  if (_setPWM(pin, val, analogScale)) {
    if (val > 0 && val < analogScale) {
      analogMap |= (1 << pin);
    }
  }
}

extern void __analogWriteRange(uint32_t range) {
  if ((range >= 15) && (range <= 65535)) {
    analogScale = range;
  }
}

extern void __analogWriteResolution(int res) {
  if ((res >= 4) && (res <= 16)) {
    analogScale = (1 << res) - 1;
  }
}

extern void analogWrite(uint8_t pin, int val) __attribute__((weak, alias("__analogWrite")));
extern void analogWriteMode(uint8_t pin, int val, bool openDrain) __attribute__((weak, alias("__analogWriteMode")));
extern void analogWriteFreq(uint32_t freq) __attribute__((weak, alias("__analogWriteFreq")));
extern void analogWriteRange(uint32_t range) __attribute__((weak, alias("__analogWriteRange")));
extern void analogWriteResolution(int res) __attribute__((weak, alias("__analogWriteResolution")));

};
