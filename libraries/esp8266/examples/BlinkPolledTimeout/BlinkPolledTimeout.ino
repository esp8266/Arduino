/*
  ESP8266 Blink with polledTimeout by Daniel Salazar

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

void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
}

void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
}

void ledToggle() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // Change the state of the LED
}


esp8266::polledTimeout::periodicFastUs halfPeriod(500000); //use fully qualified type and avoid importing all ::esp8266 namespace to the global namespace

// the setup function runs only once at start
void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.printf("periodic/oneShotMs::timeMax()     = %u ms\n", (uint32_t)esp8266::polledTimeout::periodicMs::timeMax());
  Serial.printf("periodic/oneShotFastMs::timeMax() = %u ms\n", (uint32_t)esp8266::polledTimeout::periodicFastMs::timeMax());
  Serial.printf("periodic/oneShotFastUs::timeMax() = %u us\n", (uint32_t)esp8266::polledTimeout::periodicFastUs::timeMax());
  Serial.printf("periodic/oneShotFastNs::timeMax() = %u ns\n", (uint32_t)esp8266::polledTimeout::periodicFastNs::timeMax());

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  using esp8266::polledTimeout::oneShotMs; //import the type to the local namespace

  //STEP1; turn the led ON
  ledOn();

  //STEP2: wait for ON timeout
  oneShotMs timeoutOn(2000);
  while (!timeoutOn) {
    yield();
  }

  //STEP3: turn the led OFF
  ledOff();

  //STEP4: wait for OFF timeout to assure the led is kept off for this time before exiting setup
  oneShotMs timeoutOff(2000);
  while (!timeoutOff) {
    yield();
  }

  //Done with STEPs, do other stuff
  halfPeriod.reset(); //halfPeriod is global, so it gets inited on sketch start. Clear it here to make it ready for loop, where it's actually used.
}


// the loop function runs over and over again forever
void loop() {
  if (halfPeriod) {
    ledToggle();
  }
}
