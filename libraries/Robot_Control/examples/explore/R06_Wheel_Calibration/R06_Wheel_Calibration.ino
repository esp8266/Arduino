/* 6 Wheel Calibration
*
* Use this sketch to calibrate the wheels in your robot.
* Your robot should drive as straight as possible when
* putting both motors at the same speed.
*
* Run the software and follow the on-screen instructions.
* Use the trimmer on the bottom board to make sure the
* robot is working at its best!
*
* (c) 2013 X. Yang
*/
#include "scripts_library.h"

#include <ArduinoRobot.h>
#include <Wire.h>
#include <SPI.h>

void setup() {
  Serial.begin(9600);
  Robot.begin();
  Robot.beginTFT();
  Robot.beginSD();

  Robot.setTextWrap(false);
  Robot.displayLogos();

  writeAllScripts();

}
void loop() {
  int val = map(Robot.knobRead(), 0, 1023, -255, 255);
  Serial.println(val);
  Robot.motorsWrite(val, val);

  int WC = map(Robot.trimRead(), 0, 1023, -20, 20);
  Robot.debugPrint(WC, 108, 149);
  delay(40);

}
