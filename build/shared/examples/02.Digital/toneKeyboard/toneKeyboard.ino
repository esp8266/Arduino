/*
  keyboard

 Plays a pitch that changes based on a changing analog input

 circuit:
 * 3 force-sensing resistors from +5V to analog in 0 through 5
 * 3 10K resistors from analog in 0 through 5 to ground
 * 8-ohm speaker on digital pin 8

 created 21 Jan 2010
 modified 9 Apr 2012
 by Tom Igoe

This example code is in the public domain.

 http://arduino.cc/en/Tutorial/Tone3

 */

#include "pitches.h"

const int threshold = 10;    // minimum reading of the sensors that generates a note

// notes to play, corresponding to the 3 sensors:
int notes[] = {
  NOTE_A4, NOTE_B4, NOTE_C3
};

void setup() {

}

void loop() {
  for (int thisSensor = 0; thisSensor < 3; thisSensor++) {
    // get a sensor reading:
    int sensorReading = analogRead(thisSensor);

    // if the sensor is pressed hard enough:
    if (sensorReading > threshold) {
      // play the note corresponding to this sensor:
      tone(8, notes[thisSensor], 20);
    }
  }
}
