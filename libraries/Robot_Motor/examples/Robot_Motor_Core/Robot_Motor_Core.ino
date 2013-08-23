/* Motor Core

  This code for the Arduino Robot's motor board
  is the stock firmware. program the motor board with 
  this sketch whenever you want to return the motor
  board to its default state.
  
*/

#include <ArduinoRobotMotorBoard.h>

void setup(){
  RobotMotor.begin();
}
void loop(){
  RobotMotor.parseCommand();
  RobotMotor.process();
}
