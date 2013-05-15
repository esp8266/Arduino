/* Motor Board IR Array Test
   
   This example of the Arduno robot's motor board returns the 
   values read fron the 5 infrared sendors on the bottom of 
   the robot.

*/
// include the motor board header 
#include <ArduinoRobotMotorBoard.h>

String bar; // string for storing the informaton

void setup(){
  // start serial communication
  Serial.begin(9600);
  // initialize the library
  RobotMotor.begin();
}
void loop(){
  bar=String(""); // empty the string
  // read the sensors and add them to the string
  bar=bar+RobotMotor.readIR(0)+' '+RobotMotor.readIR(1)+' '+RobotMotor.readIR(2)+' '+RobotMotor.readIR(3)+' '+RobotMotor.readIR(4);
  // print out the values
  Serial.println(bar);
  delay(100);
}
