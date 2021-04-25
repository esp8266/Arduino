/*
  ESP8266 forced light sleep mode example

  Copyright (c) 2021 Dirk O. Kaar. All rights reserved.
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

#include <Schedule.h>
#include <PolledTimeout.h>

#define WAKE_UP_PIN 0  // D3/GPIO0, can also force a serial flash upload with RESET
// you can use any GPIO for WAKE_UP_PIN except for D0/GPIO16 as it doesn't support interrupts

void IRAM_ATTR wakeupPinIsr() {
  // For edge-triggered IRQ.
  schedule_function([]() {
    Serial.println("GPIO went from HI to LO");
  });
}

void IRAM_ATTR wakeupPinIsrWE() {
  // Wakeup IRQs are level-triggered only.
  schedule_function([]() {
    Serial.println("GPIO wakeup IRQ");
  });
  wakeupPinIsr();
  // return to falling edge IRQ, otherwise level-triggered IRQ
  // keeps triggering this ISR back-to-back, consuming nearly all CPU time.
  attachInterrupt(WAKE_UP_PIN, wakeupPinIsr, FALLING);
}

void wakeupCallback() {
  schedule_function([]() {
    Serial.println("wakeup callback was performed");
  });
}

void setup() {
  Serial.begin(74880);
  while (!Serial);
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);  // activity and status indicator
  digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
  pinMode(WAKE_UP_PIN, INPUT_PULLUP);  // polled to advance tests, interrupt for Forced Light Sleep
  attachInterrupt(WAKE_UP_PIN, wakeupPinIsr, FALLING);
}

using oneShotYieldMs = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
oneShotYieldMs gotoSleep(2000);

void loop() {
  if (gotoSleep && ESP.forcedLightSleepBegin(10 * 1000 * 1000, wakeupCallback)) {
    // No new timers, no delay(), between forcedLightSleepBegin() and forcedLightSleepEnd().
    // Only ONLOW_WE or ONHIGH_WE interrupts work, no edge, that's an SDK or CPU limitation.
    // If the GPIO is in the state that will cause a wakeup on attaching the interrupt,
    // it cannot trigger a wakeup later, but any sleep duration will be honored.
    bool wakeupPinIsHigh = digitalRead(WAKE_UP_PIN);
    // the GPIO might still bounce to LOW between both digital reads, disabling wakeup
    if (wakeupPinIsHigh) {
      attachInterrupt(WAKE_UP_PIN, wakeupPinIsrWE, ONLOW_WE);
    }
    wakeupPinIsHigh &= digitalRead(WAKE_UP_PIN);
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off so they know the CPU isn't running
    ESP.forcedLightSleepEnd(!wakeupPinIsHigh);
    digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
    if (wakeupPinIsHigh) {
      gotoSleep.reset();
    }
  }
}
