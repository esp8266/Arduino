/*
  Arduino Starter Kit example
 Project 8  - Digital Hourglass
 
 This sketch is written to accompany Project 8 in the
 Arduino Starter Kit
 
 Parts required:
 10 kilohm resistor
 six 220 ohm resistors
 six LEDs
 tilt switch
 
 Created 13 September 2012
 by Scott Fitzgerald
 
 http://arduino.cc/starterKit
 
 This example code is part of the public domain 
 */

// named constant for the switch pin
const int switchPin = 8;

unsigned long previousTime = 0; // store the last time an LED was updated
int switchState = 0; // the current switch state
int prevSwitchState = 0; // the previous switch state
int led = 2; // a variable to refer to the LEDs

// 600000 = 10 minutes in milliseconds
long interval = 600000; // interval at which to light the next LED 

void setup() {
  // set the LED pins as outputs
  for(int x = 2;x<8;x++){
    pinMode(x, OUTPUT);
  }
  // set the tilt switch pin as input 
  pinMode(switchPin, INPUT);
}

void loop(){ 
  // store the time since the Arduino started running in a variable 
  unsigned long currentTime = millis(); 

  // compare the current time to the previous time an LED turned on
  // if it is greater than your interval, run the if statement
  if(currentTime - previousTime > interval) {
    // save the current time as the last time you changed an LED 
    previousTime = currentTime; 
    // Turn the LED on
    digitalWrite(led, HIGH);
    // increment the led variable
    // in 10 minutes the next LED will light up   
    led++; 
    
    if(led == 7){
      // the hour is up
    }
  }

  // read the switch value
  switchState = digitalRead(switchPin); 
  
  // if the switch has changed
  if(switchState != prevSwitchState){
    // turn all the LEDs low
    for(int x = 2;x<8;x++){    
      digitalWrite(x, LOW);
    }  
    
    // reset the LED variable to the first one
    led = 2;
    
    //reset the timer
    previousTime = currentTime;
  }
  // set the previous switch state to the current state
  prevSwitchState = switchState;
}
