/*
 IR array

 Read the analog value of the IR sensors at the
 bottom of the robot. The also-called line following
 sensors are a series of pairs of IR sender/receiver
 used to detect how dark it is underneath the robot.

 The information coming from the sensor array is stored
 into the Robot.IRarray[] and updated using the Robot.updateIR()
 method.

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

  // initialize the serial port
  Serial.begin(9600);
}

void loop() {
  // store the sensor information into the array
  Robot.updateIR();

  // iterate the array and print the data to the Serial port
  for (int i = 0; i < 5; i++) {
    Serial.print(Robot.IRarray[i]);
    Serial.print(" ");
  }
  Serial.println("");
}
