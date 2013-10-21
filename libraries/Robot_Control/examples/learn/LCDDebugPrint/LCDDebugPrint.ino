/*
 LCD Debug Print

 Use the Robot's library function debugPrint() to
 quickly send a sensor reading to the robot's creen.

 Circuit:
 * Arduino Robot

 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles

 This example is in the public domain
 */

#include <ArduinoRobot.h>

int value;

void setup() {
  // initialize the robot
  Robot.begin();

  // initialize the screen
  Robot.beginTFT();
}
void loop() {
  // read a value
  value = analogRead(A4);

  // send the value to the screen
  Robot.debugPrint(value);

  delay(40);
}
