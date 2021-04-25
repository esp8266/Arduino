/*
  ESP8266 forced modem sleep mode example

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

using oneShotYieldMs = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
oneShotYieldMs gotoSleep(2000);

void wakeupCallback() {
  digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
  schedule_function([]() {
    Serial.println("wakeup callback was performed");
    gotoSleep.reset(2000);
  });
}

void setup() {
  Serial.begin(74880);
  while (!Serial);
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);  // activity and status indicator
  digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
}

void loop() {
  if (gotoSleep && ESP.forcedModemSleep(10 * 1000 * 1000, wakeupCallback)) {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off so they know the modem isn't running
    gotoSleep.resetToNeverExpires();
  }
}
