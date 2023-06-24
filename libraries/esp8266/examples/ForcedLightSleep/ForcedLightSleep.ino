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
    detachInterrupt(WAKE_UP_PIN);
    schedule_function([]() {
        Serial.println("GPIO went from HI to LO");
        });
}

void IRAM_ATTR wakeupPinIsrWE() {
    // Wakeup IRQs are available as level-triggered only.
    detachInterrupt(WAKE_UP_PIN);
    schedule_function([]() {
        Serial.println("GPIO wakeup IRQ");
        });
    wakeupPinIsr();
    // reattach falling edge IRQ in loop
}

void wakeupCallback() {
    schedule_function([]() {
        Serial.println("wakeup callback was performed");
        });
    // return to falling edge IRQ, otherwise level-triggered IRQ with wakeup
    // would get called unexpectedly while awake.
    attachInterrupt(WAKE_UP_PIN, wakeupPinIsr, FALLING);
}

void setup() {
    Serial.begin(74880);
    while (!Serial)
        ;
    delay(100);
    pinMode(LED_BUILTIN, OUTPUT);        // activity and status indicator
    digitalWrite(LED_BUILTIN, LOW);      // turn on the LED
    pinMode(WAKE_UP_PIN, INPUT_PULLUP);  // polled to advance tests, interrupt for Forced Light Sleep
    attachInterrupt(WAKE_UP_PIN, wakeupPinIsr, FALLING);
}

using oneShotYieldMs = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
oneShotYieldMs gotoSleep(2000);

void loop() {
    if (gotoSleep) {
        // No new timers, no delay(), while RAII ForcedLightSleepToken exists.
        // Only ONLOW_WE or ONHIGH_WE interrupts work, no edge, that's an SDK or CPU limitation.
        // If the GPIO is in the state that will cause a wakeup on attaching the interrupt,
        // it cannot trigger a wakeup later, but any sleep duration will be honored.
        bool wakeupPinIsHigh = digitalRead(WAKE_UP_PIN);
        {
            ESPForcedLightSleepToken token(10 * 1000 * 1000, wakeupCallback);
            if (token) { // if true, run user code to set up forced light sleep details
                // debouncing the wake up pin
                delayMicroseconds(5000);
                wakeupPinIsHigh &= digitalRead(WAKE_UP_PIN);
                delayMicroseconds(5000);
                wakeupPinIsHigh &= digitalRead(WAKE_UP_PIN);
                // the GPIO might still bounce to LOW after this but before sleep is full engaged,
                // disabling wakeup after all
                if (wakeupPinIsHigh) {
                    attachInterrupt(WAKE_UP_PIN, wakeupPinIsrWE, ONLOW_WE);
                }
                digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off so they know the CPU isn't running
                if (!wakeupPinIsHigh) token.cancel();
            }
            // RAII token gets destructed, going to sleep if all went well
        }
        digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
        // retry immediately if the GPIO was found not ready for entering sleep
        if (wakeupPinIsHigh) {
            gotoSleep.reset();
        }
        // restore falling edge IRQ
        attachInterrupt(WAKE_UP_PIN, wakeupPinIsr, FALLING);
    }
}
