/*
  Tone.cpp

  A Tone Generator Library for the ESP8266

  Copyright (c) 2016 Ben Pirt. All rights reserved.
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
#include "pins_arduino.h"

#define AVAILABLE_TONE_PINS 1
const uint8_t tone_timers[] = { 1 };
static uint8_t tone_pins[AVAILABLE_TONE_PINS] = { 255, };
static long toggle_counts[AVAILABLE_TONE_PINS] = { 0, };
#define T1INDEX 0

void t1IntHandler();

static int8_t toneBegin(uint8_t _pin) {
  int8_t _index = -1;

  // if we're already using the pin, reuse it.
  for (int i = 0; i < AVAILABLE_TONE_PINS; i++) {
    if (tone_pins[i] == _pin) {
      return i;
    }
  }

  // search for an unused timer.
  for (int i = 0; i < AVAILABLE_TONE_PINS; i++) {
    if (tone_pins[i] == 255) {
      tone_pins[i] = _pin;
      _index = i;
      break;
    }
  }

  return _index;
}

// frequency (in hertz) and duration (in milliseconds).
void tone(uint8_t _pin, unsigned int frequency, unsigned long duration) {
  int8_t _index;

  _index = toneBegin(_pin);

  if (_index >= 0) {
    // Set the pinMode as OUTPUT
    pinMode(_pin, OUTPUT);

    // Calculate the toggle count
    if (duration > 0) {
      toggle_counts[_index] = 2 * frequency * duration / 1000;
    } else {
      toggle_counts[_index] = -1;
    }

    // set up the interrupt frequency
    switch (tone_timers[_index]) {
      case 0:
        // Not currently supported
        break;

      case 1:
        timer1_disable();
        timer1_isr_init();
        timer1_attachInterrupt(t1IntHandler);
        timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
        timer1_write((clockCyclesPerMicrosecond() * 500000) / frequency);
        break;
    }
  }
}

void disableTimer(uint8_t _index) {
  tone_pins[_index] = 255;

  switch (tone_timers[_index]) {
    case 0:
      // Not currently supported
      break;

    case 1:
      timer1_disable();
      break;
  }
}

void noTone(uint8_t _pin) {
  for (int i = 0; i < AVAILABLE_TONE_PINS; i++) {
    if (tone_pins[i] == _pin) {
      tone_pins[i] = 255;
      disableTimer(i);
      break;
    }
  }
  digitalWrite(_pin, LOW);
}

ICACHE_RAM_ATTR void t1IntHandler() {
  if (toggle_counts[T1INDEX] != 0){
    // toggle the pin
    digitalWrite(tone_pins[T1INDEX], toggle_counts[T1INDEX] % 2);
    toggle_counts[T1INDEX]--;
    // handle the case of indefinite duration
    if (toggle_counts[T1INDEX] < -2){
      toggle_counts[T1INDEX] = -1;
    }
  }else{
    disableTimer(T1INDEX);
    digitalWrite(tone_pins[T1INDEX], LOW);
  }
}
