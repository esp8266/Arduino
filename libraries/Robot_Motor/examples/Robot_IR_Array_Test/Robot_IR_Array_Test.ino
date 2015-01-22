/* Motor Board IR Array Test

   This example of the Arduno robot's motor board returns the
   values read fron the 5 infrared sendors on the bottom of
   the robot.

*/
// include the motor board header
#include <ArduinoRobotMotorBoard.h>

String bar; // string for storing the informaton

void setup() {
  // start serial communication
  Serial.begin(9600);
  // initialize the library
  RobotMotor.begin();
}
void loop() {
  bar = String(""); // empty the string
  // read the sensors and add them to the string
  bar = bar + RobotMotor.IRread(1) + ' ' + RobotMotor.IRread(2) + ' ' + RobotMotor.IRread(3) + ' ' + RobotMotor.IRread(4) + ' ' + RobotMotor.IRread(5);
  // print out the values
  Serial.println(bar);
  delay(100);
}
