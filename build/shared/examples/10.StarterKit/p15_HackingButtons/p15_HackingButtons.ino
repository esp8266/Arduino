/*
  Arduino Starter Kit example
 Project 15  - Hacking Buttons

 This sketch is written to accompany Project 15 in the
 Arduino Starter Kit

 Parts required:
 batery powered component
 220 ohm resistor
 4N35 optocoupler

 Created 18 September 2012
 by Scott Fitzgerald

 http://www.arduino.cc/starterKit

 This example code is part of the public domain
 */

const int optoPin = 2; // the pin the optocoupler is connected to

void setup() {
  // make the pin with the optocoupler an output
  pinMode(optoPin, OUTPUT);
}

void loop() {
  digitalWrite(optoPin, HIGH);  // pull pin 2 HIGH, activating the optocoupler

  delay(15); // give the optocoupler a moment to activate

  digitalWrite(optoPin, LOW);  // pull pin 2 low until you're ready to activate again
  delay(21000);                // wait for 21 seconds
}


