/*
  Esplora Sound Sensor

 This  sketch shows you how to read  the microphone sensor. The microphone
will range from 0 (total silence) to 1023 (really loud).
 When you're using the sensor's reading (for example, to set the brightness
 of the LED), you map the sensor's reading to a range between the minimum
 and the maximum.

 Created on 22 Dec 2012
 by Tom Igoe

 This example is in the public domain.
 */

#include <Esplora.h>

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
}

void loop() {
  // read the sensor into a variable:
  int loudness = Esplora.readMicrophone();

  // map the sound level to a brightness level for the LED:
  int brightness = map(loudness, 0, 1023, 0, 255);
  // write the brightness to the green LED:
  Esplora.writeGreen(brightness);


  // print the microphone levels and the LED levels (to see what's going on):
  Serial.print("sound level: ");
  Serial.print(loudness);
  Serial.print(" Green brightness: ");
  Serial.println(brightness);
  // add a delay to keep the LED from flickering:
  delay(10);
}

