/*
 Compass

 Try the compass both on the robot's TFT
 and through the serial port.

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

  // initialize the robot's screen
  Robot.beginTFT();

  // initialize the serial port
  Serial.begin(9600);
}

void loop() {
  // read the compass
  int compass = Robot.compassRead();

  // print out the sensor's value
  Serial.println(compass);

  // show the value on the robot's screen
  Robot.drawCompass(compass);
}

