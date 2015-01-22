/*
  Arduino Starter Kit example
 Project 11  - Crystal Ball

 This sketch is written to accompany Project 11 in the
 Arduino Starter Kit

 Parts required:
 220 ohm resistor
 10 kilohm resistor
 10 kilohm potentiometer
 16x2 LCD screen
 tilt switch


 Created 13 September 2012
 by Scott Fitzgerald

 http://arduino.cc/starterKit

 This example code is part of the public domain
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// set up a constant for the tilt switchPin
const int switchPin = 6;

// variable to hold the value of the switchPin
int switchState = 0;

// variable to hold previous value of the switchpin
int prevSwitchState = 0;

// a variable to choose which reply from the crystal ball
int reply;

void setup() {
  // set up the number of columns and rows on the LCD
  lcd.begin(16, 2);

  // set up the switch pin as an input
  pinMode(switchPin, INPUT);

  // Print a message to the LCD.
  lcd.print("Ask the");
  // set the cursor to column 0, line 1
  // line 1 is the second row, since counting begins with 0
  lcd.setCursor(0, 1);
  // print to the second line
  lcd.print("Crystal Ball!");
}

void loop() {
  // check the status of the switch
  switchState = digitalRead(switchPin);

  // compare the switchState to its previous state
  if (switchState != prevSwitchState) {
    // if the state has changed from HIGH to LOW
    // you know that the ball has been tilted from
    // one direction to the other
    if (switchState == LOW) {
      // randomly chose a reply
      reply = random(8);
      // clean up the screen before printing a new reply
      lcd.clear();
      // set the cursor to column 0, line 0
      lcd.setCursor(0, 0);
      // print some text
      lcd.print("the ball says:");
      // move the cursor to the second line
      lcd.setCursor(0, 1);

      // choose a saying to print baed on the value in reply
      switch (reply) {
        case 0:
          lcd.print("Yes");
          break;

        case 1:
          lcd.print("Most likely");
          break;

        case 2:
          lcd.print("Certainly");
          break;

        case 3:
          lcd.print("Outlook good");
          break;

        case 4:
          lcd.print("Unsure");
          break;

        case 5:
          lcd.print("Ask again");
          break;

        case 6:
          lcd.print("Doubtful");
          break;

        case 7:
          lcd.print("No");
          break;
      }
    }
  }
  // save the current switch state as the last state
  prevSwitchState = switchState;
}

