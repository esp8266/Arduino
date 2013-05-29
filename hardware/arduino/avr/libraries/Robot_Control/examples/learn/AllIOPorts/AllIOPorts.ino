/*
 All IO Ports
 
 This example goes through all the IO ports on your robot and
 reads/writes from/to them. Uncomment the different lines inside 
 the loop to test the different possibilities.

 The TK inputs on the Control Board are multiplexed and therefore 
 it is not recommended to use them as outputs. The TKD pins on the
 Control Board as well as the TK pins on the Motor Board go directly
 to the microcontroller and therefore can be used both as inputs 
 and outputs.
 
 Circuit:
 * Arduino Robot
 
 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles
 
 This example is in the public domain
 */

#include <ArduinoRobot.h>

// use arrays to store the names of the pins to be read
uint8_t arr[]  = { TK0, TK1, TK2, TK3, TK4, TK5, TK6, TK7 };
uint8_t arr2[] = { TKD0, TKD1, TKD2, TKD3, TKD4, TKD5 };
uint8_t arr3[] = { B_TK1, B_TK2, B_TK3, B_TK4 };

void setup(){
  // initialize the robot
  Robot.begin();

  // open the serial port to send the information of what you are reading
  Serial.begin(9600);
}

void loop(){
  // read all the TK inputs at the Motor Board as analog 
  analogReadB_TKs();

  // read all the TK inputs at the Motor Board as digital
  //digitalReadB_TKs();

  // read all the TK inputs at the Control Board as analog
  //analogReadTKs();

  // read all the TK inputs at the Control Board as digital
  //digitalReadTKs();

  // read all the TKD inputs at the Control Board as analog
  //analogReadTKDs();

  // read all the TKD inputs at the Control Board as digital
  //digitalReadTKDs();

  // write all the TK outputs at the Motor Board as digital
  //digitalWriteB_TKs();

  // write all the TKD outputs at the Control Board as digital
  //digitalWriteTKDs();
  delay(5);  
}

// read all TK inputs on the Control Board as analog inputs
void analogReadTKs() {
  for(int i=0;i<8;i++) {
    Serial.print(Robot.analogRead(arr[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// read all TK inputs on the Control Board as digital inputs
void digitalReadTKs() {
  for(int i=0;i<8;i++) {
    Serial.print(Robot.digitalRead(arr[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// read all TKD inputs on the Control Board as analog inputs
void analogReadTKDs() {
  for(int i=0; i<6; i++) {
    Serial.print(Robot.analogRead(arr2[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// read all TKD inputs on the Control Board as digital inputs
void digitalReadTKDs() {
  for(int i=0; i<6; i++) {
    Serial.print(Robot.digitalRead(arr2[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// write all TKD outputs on the Control Board as digital outputs
void digitalWriteTKDs() {
  // turn all the pins on
  for(int i=0; i<6; i++) {
    Robot.digitalWrite(arr2[i], HIGH);
  }
  delay(500);

  // turn all the pins off
  for(int i=0; i<6; i++){
    Robot.digitalWrite(arr2[i], LOW);
  }
  delay(500);
}

// write all TK outputs on the Motor Board as digital outputs
void digitalWriteB_TKs() {
  // turn all the pins on
  for(int i=0; i<4; i++) {
    Robot.digitalWrite(arr3[i], HIGH);
  }
  delay(500);

  // turn all the pins off
  for(int i=0; i<4; i++) {
    Robot.digitalWrite(arr3[i], LOW);
  }
  delay(500);
}

// read all TK inputs on the Motor Board as analog inputs
void analogReadB_TKs() {
  for(int i=0; i<4; i++) {
    Serial.print(Robot.analogRead(arr3[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// read all TKD inputs on the Motor Board as digital inputs
void digitalReadB_TKs() {
  for(int i=0; i<4; i++) {
    Serial.print(Robot.digitalRead(arr3[i]));
    Serial.print(",");
  }
  Serial.println("");
}
