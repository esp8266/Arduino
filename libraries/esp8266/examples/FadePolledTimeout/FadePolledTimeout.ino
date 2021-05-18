/*
  ESP8266 LED fade with polledTimeout and locked phase PWM

  Modified from an BlinkPolledTimeout.ino,
  Copyright (c) 2018 Daniel Salazar. All rights reserved.
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


  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

#include <PolledTimeout.h>

PolledTimeout::periodicFastUs stepPeriod(50000);

// the setup function runs only once at start
void setup() {
  Serial.begin(115200);
  Serial.println();

  // This next line will cause the code to use the Phase-Locked waveform generator
  // instead of the default PWM-Locked one.  Comment it out to try the default version.
  // For more information on choosing between the two options, see the following pull requests:
  // Phase-Locked generator: https://github.com/esp8266/Arduino/pull/7022
  // PWM-Locked generator:   https://github.com/esp8266/Arduino/pull/7231
  enablePhaseLockedWaveform();

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  analogWriteRange(1000);

  using PolledTimeout::oneShotMs; //import the type to the local namespace

  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level

  oneShotMs timeoutOn(2000);
  while (!timeoutOn) {
    yield();
  }

  stepPeriod.reset();
}


void loop() {
  static int val = 0;
  static int delta = 100;
  if (stepPeriod) {
    val += delta;
    if (val < 0) {
      val = 100;
      delta = 100;
    } else if (val > 1000) {
      val = 900;
      delta = -100;
    }
    analogWrite(LED_BUILTIN, val);
  }
}
