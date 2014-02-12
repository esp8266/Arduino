/*
 Turn Test

 Check if the robot turns a certain amount of degrees.

 Circuit:
 * Arduino Robot

 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles

 This example is in the public domain
 */

#include <ArduinoRobot.h>
#include <Wire.h>
#include <SPI.h>

void setup() {
  // initialize the robot
  Robot.begin();
}

void loop() {
  Robot.turn(50); //turn 50 degrees to the right
  Robot.motorsStop();
  delay(1000);

  Robot.turn(-100); //turn 100 degrees to the left
  Robot.motorsStop();
  delay(1000);
}
