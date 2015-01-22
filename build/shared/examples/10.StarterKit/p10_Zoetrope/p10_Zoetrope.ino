/*
  Arduino Starter Kit example
 Project 10  - Zoetrope

 This sketch is written to accompany Project 10 in the
 Arduino Starter Kit

 Parts required:
 two 10 kilohm resistors
 2 momentary pushbuttons
 one 10 kilohm potentiometer
 motor
 9V battery
 H-Bridge

 Created 13 September 2012
 by Scott Fitzgerald
 Thanks to Federico Vanzati for improvements

 http://arduino.cc/starterKit

 This example code is part of the public domain
 */

const int controlPin1 = 2; // connected to pin 7 on the H-bridge
const int controlPin2 = 3; // connected to pin 2 on the H-bridge
const int enablePin = 9;   // connected to pin 1 on the H-bridge
const int directionSwitchPin = 4;  // connected to the switch for direction
const int onOffSwitchStateSwitchPin = 5; // connected to the switch for turning the motor on and off
const int potPin = A0;  // connected to the potentiometer's output

// create some variables to hold values from your inputs
int onOffSwitchState = 0;  // current state of the On/Off switch
int previousOnOffSwitchState = 0; // previous position of the on/off switch
int directionSwitchState = 0;  // current state of the direction switch
int previousDirectionSwitchState = 0;  // previous state of the direction switch

int motorEnabled = 0; // Turns the motor on/off
int motorSpeed = 0; // speed of the motor
int motorDirection = 1; // current direction of the motor

void setup() {
  // intialize the inputs and outputs
  pinMode(directionSwitchPin, INPUT);
  pinMode(onOffSwitchStateSwitchPin, INPUT);
  pinMode(controlPin1, OUTPUT);
  pinMode(controlPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);

  // pull the enable pin LOW to start
  digitalWrite(enablePin, LOW);
}

void loop() {
  // read the value of the on/off switch
  onOffSwitchState = digitalRead(onOffSwitchStateSwitchPin);
  delay(1);

  // read the value of the direction switch
  directionSwitchState = digitalRead(directionSwitchPin);

  // read the value of the pot and divide by 4 to get
  // a value that can be used for PWM
  motorSpeed = analogRead(potPin) / 4;

  // if the on/off button changed state since the last loop()
  if (onOffSwitchState != previousOnOffSwitchState) {
    // change the value of motorEnabled if pressed
    if (onOffSwitchState == HIGH) {
      motorEnabled = !motorEnabled;
    }
  }

  // if the direction button changed state since the last loop()
  if (directionSwitchState != previousDirectionSwitchState) {
    // change the value of motorDirection if pressed
    if (directionSwitchState == HIGH) {
      motorDirection = !motorDirection;
    }
  }

  // change the direction the motor spins by talking
  // to the control pins on the H-Bridge
  if (motorDirection == 1) {
    digitalWrite(controlPin1, HIGH);
    digitalWrite(controlPin2, LOW);
  }
  else {
    digitalWrite(controlPin1, LOW);
    digitalWrite(controlPin2, HIGH);
  }

  // if the motor is supposed to be on
  if (motorEnabled == 1) {
    // PWM the enable pin to vary the speed
    analogWrite(enablePin, motorSpeed);
  }
  else { // if the motor is not supposed to be on
    //turn the motor off
    analogWrite(enablePin, 0);
  }
  // save the current On/Offswitch state as the previous
  previousDirectionSwitchState = directionSwitchState;
  // save the current switch state as the previous
  previousOnOffSwitchState = onOffSwitchState;
}




