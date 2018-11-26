/*
  ESP8266 Blink by Daniel Salazar
  Copyright 2018

  Blink the blue LED on the ESP module using polledTimeout class
  This example code is in the public domain

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



esp8266::polledTimeoutPeriodic halfPeriod(500); //use fully qualified type and avoid importing all ::esp8266 namespace to the global namespace

// the setup function runs only once at start
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  using esp8266::polledTimeoutOneShot; //import the type to the local namespace

  //STEP1; turn the led ON
  ledOn();

  //STEP2: wait for ON timeout
  polledTimeoutOneShot timeoutOn(2000);
  while(!timeoutOn)
    yield();

  //STEP3: turn the led OFF
  ledOff();

  //STEP4: wait for OFF timeout to assure the led is kept off for this time before exiting setup
  polledTimeoutOneShot timeoutOff(2000);
  while(!timeoutOff)
    yield();

  //Done with STEPs, do other stuff
  halfPeriod.reset(); //halfPeriod is global, so it gets inited on sketch start. Clear it here to make it ready for loop, where it's actually used.
}


// the loop function runs over and over again forever
void loop() {
  if(halfPeriod)
    ledToggle();
}
