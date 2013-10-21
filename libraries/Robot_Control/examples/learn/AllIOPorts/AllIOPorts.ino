/*
 All IO Ports

 This example goes through all the IO ports on your robot and
 reads/writes from/to them. Uncomment the different lines inside
 the loop to test the different possibilities.

 The M inputs on the Control Board are multiplexed and therefore
 it is not recommended to use them as outputs. The D pins on the
 Control Board as well as the D pins on the Motor Board go directly
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
uint8_t arr[]  = { M0, M1, M2, M3, M4, M5, M6, M7 };
uint8_t arr2[] = { D0, D1, D2, D3, D4, D5 };
uint8_t arr3[] = { D7, D8, D9, D10 };

void setup() {
  // initialize the robot
  Robot.begin();

  // open the serial port to send the information of what you are reading
  Serial.begin(9600);
}

void loop() {
  // read all the D inputs at the Motor Board as analog
  //analogReadB_Ds();

  // read all the D inputs at the Motor Board as digital
  //digitalReadB_Ds();

  // read all the M inputs at the Control Board as analog
  //analogReadMs();

  // read all the M inputs at the Control Board as digital
  //digitalReadMs();

  // read all the D inputs at the Control Board as analog
  analogReadT_Ds();

  // read all the D inputs at the Control Board as digital
  //digitalReadT_Ds();

  // write all the D outputs at the Motor Board as digital
  //digitalWriteB_Ds();

  // write all the D outputs at the Control Board as digital
  //digitalWriteT_Ds();
  delay(40);
}

// read all M inputs on the Control Board as analog inputs
void analogReadMs() {
  for (int i = 0; i < 8; i++) {
    Serial.print(Robot.analogRead(arr[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// read all M inputs on the Control Board as digital inputs
void digitalReadMs() {
  for (int i = 0; i < 8; i++) {
    Serial.print(Robot.digitalRead(arr[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// read all D inputs on the Control Board as analog inputs
void analogReadT_Ds() {
  for (int i = 0; i < 6; i++) {
    Serial.print(Robot.analogRead(arr2[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// read all D inputs on the Control Board as digital inputs
void digitalReadT_Ds() {
  for (int i = 0; i < 6; i++) {
    Serial.print(Robot.digitalRead(arr2[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// write all D outputs on the Control Board as digital outputs
void digitalWriteT_Ds() {
  // turn all the pins on
  for (int i = 0; i < 6; i++) {
    Robot.digitalWrite(arr2[i], HIGH);
  }
  delay(500);

  // turn all the pins off
  for (int i = 0; i < 6; i++) {
    Robot.digitalWrite(arr2[i], LOW);
  }
  delay(500);
}

// write all D outputs on the Motor Board as digital outputs
void digitalWriteB_Ds() {
  // turn all the pins on
  for (int i = 0; i < 4; i++) {
    Robot.digitalWrite(arr3[i], HIGH);
  }
  delay(500);

  // turn all the pins off
  for (int i = 0; i < 4; i++) {
    Robot.digitalWrite(arr3[i], LOW);
  }
  delay(500);
}

// read all D inputs on the Motor Board as analog inputs
void analogReadB_Ds() {
  for (int i = 0; i < 4; i++) {
    Serial.print(Robot.analogRead(arr3[i]));
    Serial.print(",");
  }
  Serial.println("");
}

// read all D inputs on the Motor Board as digital inputs
void digitalReadB_Ds() {
  for (int i = 0; i < 4; i++) {
    Serial.print(Robot.digitalRead(arr3[i]));
    Serial.print(",");
  }
  Serial.println("");
}
