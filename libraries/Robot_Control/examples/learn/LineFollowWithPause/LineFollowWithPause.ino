/*
 Line Following with Pause

 As the robot has two processors, one to command the motors and one to
 take care of the screen and user input, it is possible to write
 programs that put one part of the robot to do something and get the
 other half to control it.

 This example shows how the Control Board assigns the Motor one to
 follow a line, but asks it to stop every 3 seconds.

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

  // initialize the screen
  Robot.beginTFT();

  // get some time to place the robot on the ground
  delay(3000);

  // set the robot in line following mode
  Robot.setMode(MODE_LINE_FOLLOW);
}

void loop() {
  // tell the robot to take a break and stop
  Robot.pauseMode(true);
  Robot.debugPrint('p');
  delay(3000);

  // tell the robot to move on
  Robot.pauseMode(false);
  Robot.debugPrint('>');
  delay(3000);
}
