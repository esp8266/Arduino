/*
 LCD Print

 Print the reading from a sensor to the screen.

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

  // initialize the robot's screen
  Robot.beginTFT();
}

void loop() {
  // read a analog port
  value = Robot.analogRead(TK4);

  // write the sensor value on the screen
  Robot.stroke(0, 255, 0);
  Robot.textSize(1);
  Robot.text(value, 0, 0);

  delay(500);

  // erase the previous text on the screen
  Robot.stroke(255, 255, 255);
  Robot.textSize(1);
  Robot.text(value, 0, 0);
}
