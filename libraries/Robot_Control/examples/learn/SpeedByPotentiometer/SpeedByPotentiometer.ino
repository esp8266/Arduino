/*
 Speed by Potentiometer

 Control the robot's speed using the on-board
 potentiometer. The speed will be printed on
 the TFT screen.

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

  // initialize the screen
  Robot.beginTFT();
}

void loop() {
  // read the value of the potentiometer
  int val = map(Robot.knobRead(), 0, 1023, -255, 255);

  // print the value to the TFT screen
  Robot.debugPrint(val);

  // set the same speed on both of the robot's wheels
  Robot.motorsWrite(val, val);
  delay(10);
}
