/*
 LCD Write Text
 
 Use the Robot's library function text() to
 print out text to the robot's screen. Take
 into account that you need to erase the
 information before continuing writing.
 
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
}
void loop() {
  Robot.stroke(0, 0, 0);              // choose the color black
  Robot.text("Hello World", 0, 0);  // print the text
  delay(2000);
  Robot.stroke(255, 255, 255);        // choose the color white
  Robot.text("Hello World", 0, 0);  // writing text in the same color as the BG erases the text!
  
  Robot.stroke(0, 0, 0);              // choose the color black
  Robot.text("I am a robot", 0, 0); // print the text
  delay(3000);
  Robot.stroke(255, 255, 255);         // choose the color black
  Robot.text("I am a robot", 0, 0);  // print the text
}
