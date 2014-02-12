/*
 Keyboard Test

 Check how the robot's keyboard works. This example
 sends the data about the key pressed through the
 serial port.

 All the buttons on the Control Board are tied up to a
 single analog input pin, in this way it is possible to multiplex a
 whole series of buttons on one single pin.

 It is possible to recalibrate the thresholds of the buttons using
 the Robot.keyboardCalibrate() function, that takes a 5 ints long
 array as parameter

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
  // initialize the serial port
  Serial.begin(9600);
}

void loop() {
  // print out the keyboard readings
  Serial.println(Robot.keyboardRead());
  delay(100);
}
