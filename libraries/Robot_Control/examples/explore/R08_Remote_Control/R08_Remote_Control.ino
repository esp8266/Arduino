/* 08 Remote Control

 If you connect a IR receiver to the robot,
 you can control it like a RC car.
 Using the remote control comes with sensor
 pack, You can make the robot move around
 without even touching it!

 Circuit:
 * Arduino Robot
 * Connect the IRreceiver to D2
 * Remote control from Robot sensor pack

 based on the IRremote library
 by Ken Shirriff
 http://arcfn.com

 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles

 This example is in the public domain
 */

// include the necessary libraries
#include <IRremote.h>
#include <IRremoteTools.h>
#include <ArduinoRobot.h>

// Define a few commands from your remote control
#define IR_CODE_FORWARD 284154405
#define IR_CODE_BACKWARDS 284113605
#define IR_CODE_TURN_LEFT 284129925
#define IR_CODE_TURN_RIGHT 284127885
#define IR_CODE_CONTINUE -1

boolean isActing = false; //If the robot is executing command from remote
long timer;
const long TIME_OUT = 150;

void setup() {
  // initialize the Robot, SD card, display, and speaker
  Serial.begin(9600);
  Robot.begin();
  Robot.beginTFT();
  Robot.beginSD();

  // print some text to the screen
  beginIRremote(); // Start the receiver
}

void loop() {
  // if there is an IR command, process it
  if (IRrecived()) {
    processResult();
    resumeIRremote(); // resume receiver
  }

  //If the robot does not receive any command, stop it
  if (isActing && (millis() - timer >= TIME_OUT)) {
    Robot.motorsStop();
    isActing = false;
  }
}
void processResult() {
  unsigned long res = getIRresult();
  switch (res) {
    case IR_CODE_FORWARD:
      changeAction(1, 1); //Move the robot forward
      break;
    case IR_CODE_BACKWARDS:
      changeAction(-1, -1); //Move the robot backwards
      break;
    case IR_CODE_TURN_LEFT:
      changeAction(-0.5, 0.5); //Turn the robot left
      break;
    case IR_CODE_TURN_RIGHT:
      changeAction(0.5, -0.5); //Turn the robot Right
      break;
    case IR_CODE_CONTINUE:
      timer = millis(); //Continue the last action, reset timer
      break;
  }
}
void changeAction(float directionLeft, float directionRight) {
  Robot.motorsWrite(255 * directionLeft, 255 * directionRight);
  timer = millis();
  isActing = true;
}

