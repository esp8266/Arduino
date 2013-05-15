/* Robot Compass

 The robot has an on-board compass module, with
 which it can tell the direction the robot is 
 facing. This sketch will make sure the robot 
 goes towards a certain direction. 
 
 Beware, magnets will interfere with the compass
 readings.

 Circuit:
 * Arduino Robot
 
 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles
 
 This example is in the public domain
 */

// include the robot library
#include <ArduinoRobot.h>

int speedLeft;
int speedRight;
int compassValue;
int direc = 180;  //Direction the robot is heading

void setup() {
  // initialize the modules
  Robot.begin();
  Robot.beginTFT();
  Robot.beginSD();
  Robot.displayLogos();
}

void loop() { 
  // read the compass orientation
  compassValue = Robot.compassRead();
  
  // how many degrees are we off
  int diff = compassValue-direc;
  
  // modify degress 
  if(diff > 180)
    diff = -360+diff;
  else if(diff < -180)
    diff = 360+diff;
    
  // Make the robot turn to its proper orientation
  diff = map(diff, -180, 180, -255, 255);
  
  if(diff > 0) {
    // keep the right wheel spinning, 
    // change the speed of the left wheel 
    speedLeft = 255-diff;
    speedRight = 255;
  } else {
    // keep the right left spinning,
    // change the speed of the left wheel   
    speedLeft = 255;
    speedRight = 255+diff;
  }
  // write out to the motors
  Robot.motorsWrite(speedLeft, speedRight);

  // draw the orientation on the screen
  Robot.drawCompass(compassValue);
}
