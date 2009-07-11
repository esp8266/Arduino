/*
  Conditionals - while statement
 
 This example demonstrates the use of  while() statements.
 
 It reads the state of a potentiometer (an analog input) and blinks an LED
 while the LED remains above a certain threshold level. It prints the analog value
 only if it's below the threshold.
 
 This example uses principles explained in the BlinkWithoutDelay  example as well.
 
 The circuit:
 * potentiometer connected to analog pin 0.
 Center pin of the potentiometer goes to the analog pin.
 side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 13 to ground
 
 * Note: On most Arduino boards, there is already an LED on the board
 connected to pin 13, so you don't need any extra components for this example.
 
 created 17 Jan 2009
 by Tom Igoe
 
 */

#define ledPin 13      // the pin for the LED
#define analogPin 0    // the analog pin that the potentiometer is attached to

#include "WProgram.h"
void setup();
void loop();
int threshold = 400;   // an arbitrary threshold level that's in the range of the analog input
int ledState = LOW;    // the state of the LED
int lastBlinkTime = 0; // last time the LED changed
int blinkDelay = 500;  // how long to hold between changes of the LED
  int analogValue;     // variable to hold the value of the analog input
void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize serial communications:
  Serial.begin(9600);
}

void loop() {
  // read the value of the potentiometer:
  analogValue = analogRead(analogPin);

  // if the analog value is high enough, turn on the LED:
  while (analogValue > threshold) {
    // if enough time has passed since the last change of the LED,
    // then change it.  Note you're using the technique from BlinkWithoutDelay
    // here so that the while loop doesn't delay the rest of the program:
    
    if (millis() - lastBlinkTime > blinkDelay) {
      // if the ledState is high, this makes it low, and vice versa:
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      
      // save the last time the LED changed in a variable:
      lastBlinkTime = millis();
    }
      // while you're in the while loop, you have to read the 
      // input again:
       analogValue = analogRead(analogPin);
  }

  // if you're below the threshold, print the analog value:
  Serial.println(analogValue, DEC);
  // turn the LED off:
   digitalWrite(ledPin, LOW);

}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

