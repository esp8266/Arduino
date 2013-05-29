/* 6 Wheel Calibration

 Use this sketch to calibrate the wheels in your robot. 
 Your robot should drive as straight as possible when 
 putting both motors at the same speed.

 Run the software and follow the on-screen instructions. 
 Use the trimmer on the motor board to make sure the 
 robot is working at its best!
 Circuit:
 * Arduino Robot
 
 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles
 
 This example is in the public domain
 */

#include <ArduinoRobot.h> // inport the robot librsry
// import the utility library
// a description of its funtionality is below
#include <utility/RobotTextManager.h> 

// arrays to hold the text for instructions
char script1[] ="Wheel Calibration";
char script2[] ="1. Put Robot on a\n flat surface";
char script3[] ="2. Adjust speed with the knob on top";
char script4[] ="3. If robot goes\n straight, it's done";
char script5[] ="4. Use screwdriver\n on the bottom trim";
char script6[] ="- Robot turns left,\n screw it clockwise;";
char script7[] ="- Turns right, screw it ct-colockwise;";
char script8[] ="5. Repeat 4 until\n going straight";

int speedRobot;  //robot speed
int calibrationValue; //value for calibrate difference between wheels

void setup(){
  //necessary initialization sequence
  Robot.begin();
  Robot.beginTFT();
  Robot.beginSD();
  
  // left and top margin for displaying text
  // see below for a description of this
  textManager.setMargin(5,5);
  // write all instructions at once
  writeAllscript();
  
}
void loop(){
  //Control the robot's speed with knob on top
  int speedRobot=map(Robot.knobRead(),0,1023,-255,255);
  Robot.motorsWrite(speedRobot,speedRobot);

  //read value of the pot on motor baord,to clibrate the wheels
  int calibrationValue=map(Robot.trimRead(),0,1023,-30,30);
  // print the values to the screen
  Robot.debugPrint(calibrationValue,110,145);
  delay(40);

}

void writeAllscript(){
  //prints 8 scripts one after another
  textManager.writeText(0,0,script1);
  textManager.writeText(1,0,script2);
  textManager.writeText(3,0,script3);
  textManager.writeText(5,0,script4);
  textManager.writeText(7,0,script5);
  textManager.writeText(9,0,script6);
  textManager.writeText(11,0,script7);
  textManager.writeText(13,0,script8);
}

/**
textManager mostly contains helper functions for 
R06_Wheel_Calibration and R01_Hello_User.

  textManager.setMargin(margin_left, margin_top):
    Configure the left and top margin for text
    display. The margins will be used by 
    textManager.writeText().
    Parameters:
      margin_left, margin_top: int, the margin values
      from the top and left side of the screen.
    Returns:
      none
    
  textManager.writeText(line,column,text):
    Display text on the specific line and column. 
    It's different from Robot.text() which
    uses pixels for positioning the text.
    Parameters:
      line:int, which line is the text displayed. Each line
        is 10px high.
      column:int,  which column is the text displayed. Each
        column is 8px wide.
      text:a char array(string) of the text to be displayed.
    Returns:
      none
*/
