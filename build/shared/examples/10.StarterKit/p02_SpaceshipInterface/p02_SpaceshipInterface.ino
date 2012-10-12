/*
  Arduino Starter Kit example
 Project 2  - Spaceship Interface
 
 This sketch is written to accompany Project 2 in the
 Arduino Starter Kit
 
 Parts required:
 1 green LED 
 2 red LEDs
 pushbutton
 10 kilohm resistor
 3 220 ohm resistors
 
 Created 13 September 2012
 by Scott Fitzgerald
 
 http://arduino.cc/starterKit

 This example code is part of the public domain 
*/

// Create a global variable to hold the 
// state of the switch. This variable is persistent 
// throughout the program. Whenever you refer to 
// switchState, you’re talking about the number it holds
int switchstate = 0;

void setup(){
  // declare the LED pins as outputs 
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);

  // declare the switch pin as an input   
  pinMode(2,INPUT);
}

void loop(){

  // read the value of the switch
  // digitalRead() checks to see if there is voltage
  // on the pin or not  
  switchstate = digitalRead(2);

  // if the button is not pressed
  // blink the red LEDs  
  if (switchstate == LOW) {
    digitalWrite(3, HIGH); // turn the green LED on pin 3 on
    digitalWrite(4, LOW);  // turn the red LED on pin 4 off
    digitalWrite(5, LOW);  // turn the red LED on pin 5 off
  }
  // this else is part of the above if() statement. 
  // if the switch is not LOW (the button is pressed)
  // the code below will run  
  else {
    digitalWrite(3, LOW);  // turn the green LED on pin 3 off
    digitalWrite(4, LOW);  // turn the red LED on pin 4 off
    digitalWrite(5, HIGH); // turn the red LED on pin 5 on
    // wait for a quarter second before changing the light
    delay(250);
    digitalWrite(4, HIGH); // turn the red LED on pin 4 on
    digitalWrite(5, LOW);  // turn the red LED on pin 5 off
    // wait for a quarter second before changing the light
    delay(250);
  }
}

