/*
 Clean EEPROM
 
 This example erases the user information stored on the
 external EEPROM memory chip on your robot.

 BEWARE, this will erase the following information:
 - your name
 - your robots name given by you
 - your city and country if you configured them via software

 EEPROMs shouldn't be rewritten too often, therefore the
 code runs only during setup and not inside loop.
 
 Circuit:
 * Arduino Robot
 
 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles
 
 This example is in the public domain
 */

#include <ArduinoRobot.h>

void setup(){
  // initialize the robot
  Robot.begin();

  // write empty strings for the different fields
  Robot.userNameWrite("");
  Robot.robotNameWrite("");
  Robot.cityNameWrite("");
  Robot.countryNameWrite(""); 
}

void loop(){
  // do nothing  
}
