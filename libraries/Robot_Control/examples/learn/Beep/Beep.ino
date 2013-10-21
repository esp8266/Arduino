/*
 Beep

 Test different pre-configured beeps on
 the robot's speaker.

 Possible beeps are:
 - BEEP_SIMPLE
 - BEEP_DOUBLE
 - BEEP_LONG

 Circuit:
 * Arduino Robot

 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles

 This example is in the public domain
 */

#include <ArduinoRobot.h>

void setup() {
  // initialize the robot
  Robot.begin();

  // initialize the sound speaker
  Robot.beginSpeaker();
}
void loop() {
  Robot.beep(BEEP_SIMPLE);
  delay(1000);
  Robot.beep(BEEP_DOUBLE);
  delay(1000);
  Robot.beep(BEEP_LONG);
  delay(1000);
}
