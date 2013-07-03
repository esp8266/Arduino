/* Robot Line Follow

 This sketch demonstrates the line following capabilities
 of the Arduino Robot. On the floor, place some black 
 electrical tape along the path you wish the robot to follow.
 To indicate a stopping point, place another piece of tape
 perpendicular to the path.

 Circuit:
 * Arduino Robot
 
 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles
 
 This example is in the public domain
 */

#include <ArduinoRobot.h> // include the robot library

long timerOrigin; // used for counting elapsed time

void setup() {
  // initialize the Robot, SD card, display, and speaker 
  Robot.begin();
  Robot.beginTFT();
  Robot.beginSD();
  Robot.beginSpeaker();

  // show the logots on the TFT screen
  Robot.displayLogos();
  
  Robot.drawBMP("lf.bmp", 0, 0); // display background image

  Robot.playFile("chase.sqm");  // play a song from the SD card
  
  // add the instructions
  Robot.text("Line Following\n\n place the robot on\n the track and \n see it run", 5, 5);
  Robot.text("Press the middle\n button to start...", 5, 61);
  Robot.waitContinue();

  // These are some general values that work for line following 
  // uncomment one or the other to see the different behaviors of the robot
  // Robot.lineFollowConfig(11, 5, 50, 10);
  Robot.lineFollowConfig(14, 9, 50, 10);
  
  //set the motor board into line-follow mode
  Robot.setMode(MODE_LINE_FOLLOW);  
  
  // start
  Robot.fill(255, 255, 255);
  Robot.stroke(255, 255, 255);
  Robot.rect(0, 0, 128, 80); // erase the previous text
  Robot.stroke(0, 0, 0);
  Robot.text("Start", 5, 5);
  
  Robot.stroke(0, 0, 0); // choose color for the text
  Robot.text("Time passed:", 5, 21); // write some text to the screen
  
  timerOrigin=millis(); // keep track of the elapsed time
  
  while(!Robot.isActionDone()) {  //wait for the finish signal
    Robot.debugPrint(millis()-timerOrigin, 5, 29);  // show how much time has passed
  }
  
  Robot.stroke(0, 0, 0);  
  Robot.text("Done!", 5, 45);
}
void loop() {
  //nothing here, the program only runs once. Reset the robot 
  //to do it again!
}
