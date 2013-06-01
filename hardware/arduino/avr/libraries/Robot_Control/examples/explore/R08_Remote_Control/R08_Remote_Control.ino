/* 08 Remote Control

 *******************
 ***
 ***This example code is in an experimental state.
 ***You are welcome to try this with your robot,
 ***and no harm will come to it. We will provide a
 ***detailed description of an updated version of this
 ***in a future update
 ***
 *** For this example to work you need:
 ***
 *** - download and install the IR-Remote library by Ken Shirriff
 ***   to be found at https://github.com/shirriff/Arduino-IRremote
 *** - get a Sony remote control
 ***
 *** This example will be updated soon, come back to the Robot
 *** page on the Arduino server for updates!!
 ***
 *******************
 
 If you connect a IR receiver to the robot, 
 you can control it like you control a TV set. 
 Using a Sony compatiable remote control, 
 map some buttons to different actions. 
 You can make the robot move around without 
 even touching it!

 Circuit: 
 * Arduino Robot
 * Connect the IRreceiver to TDK2
 * Sony compatible remote control

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
#include <ArduinoRobot.h>

// Define a few commands from your remote control
#define IR_CODE_FORWARD 0x2C9B
#define IR_CODE_BACKWARDS 0x6C9B
#define IR_CODE_TURN_LEFT 0xD4B8F
#define IR_CODE_TURN_RIGHT 0x34B8F

int RECV_PIN = TKD2; // the pin the IR receiver is connected to
IRrecv irrecv(RECV_PIN); // an instance of the IR receiver object
decode_results results; // container for received IR codes

void setup() {
  // initialize the Robot, SD card, display, and speaker 
  Robot.begin();
  Robot.beginTFT();
  Robot.beginSD();

  // print some text to the screen
  Robot.stroke(0, 0, 0);
  Robot.text("Remote Control code:", 5, 5);
  Robot.text("Command:", 5, 26);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  // if there is an IR command, process it
  if (irrecv.decode(&results)) {
    processResult();
    irrecv.resume(); // resume receiver
  }
}

void processResult() {
  unsigned long res = results.value;
  // print the value to the screen
  Robot.debugPrint(res, 5, 15);

  if(res == IR_CODE_FORWARD || res == IR_CODE_BACKWARDS || res == IR_CODE_TURN_LEFT || res == IR_CODE_TURN_RIGHT) {
    Robot.fill(255, 255, 255);
    Robot.stroke(255, 255, 255);

    Robot.rect(5, 36, 55, 10);
  }
  switch(results.value){
    case IR_CODE_FORWARD:
      Robot.stroke(0, 0, 0);
      Robot.text("Forward", 5, 36);
      Robot.motorsWrite(255, 255);
      delay(300);
      Robot.motorsStop();
      break;
    case IR_CODE_BACKWARDS:
      Robot.stroke(0, 0, 0);
      Robot.text("Backwards", 5, 36);
      Robot.motorsWrite(-255, -255);
      delay(300);
      Robot.motorsStop();
      break;
    case IR_CODE_TURN_LEFT:
      Robot.stroke(0, 0, 0);
      Robot.text("Left", 5, 36);
      Robot.motorsWrite(-255, 255); 
      delay(100);
      Robot.motorsStop();
      break;
    case IR_CODE_TURN_RIGHT:
      Robot.stroke(0, 0, 0);
      Robot.text("Right", 5, 36);
      Robot.motorsWrite(255, -255); 
      delay(100);
      Robot.motorsStop();
      break;
  }
}

