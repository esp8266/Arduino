/*
  Arduino Starter Kit example
 Project 4  - Color Mixing Lamp
 
 This sketch is written to accompany Project 3 in the
 Arduino Starter Kit
 
 Parts required:
 1 RGB LED 
 three 10 kilohm resistors
 3 220 ohm resistors
 3 photoresistors
 red green aand blue colored gels
 
 Created 13 September 2012
 by Scott Fitzgerald
 Thanks to Federico Vanzati for improvements

 http://arduino.cc/starterKit
 
 This example code is part of the public domain 
 */

const int greenLEDPin = 9;    // LED connected to digital pin 9
const int redLEDPin = 10;     // LED connected to digital pin 10
const int blueLEDPin = 11;    // LED connected to digital pin 11

const int redSensorPin = A0;  // pin with the photoresistor with the red gel 
const int greenSensorPin = A1;   // pin with the photoresistor with the green gel 
const int blueSensorPin = A2;   // pin with the photoresistor with the blue gel 

int redValue = 0; // value to write to the red LED
int greenValue = 0; // value to write to the green LED
int blueValue = 0; // value to write to the blue LED

int redSensorValue = 0; // variable to hold the value from the red sensor 
int greenSensorValue = 0; // variable to hold the value from the green sensor 
int blueSensorValue = 0; // variable to hold the value from the blue sensor 

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 

  // set the digital pins as outputs
  pinMode(greenLedPin,OUTPUT);
  pinMode(redLedPin,OUTPUT);
  pinMode(blueLedPin,OUTPUT);
}

void loop() {
  // Read the sensors first:
  
  // read the value from the red-filtered photoresistor:
  redsensorValue = analogRead(redsensorPin);
  // give the ADC a moment to settle
  delay(5);
  // read the value from the green-filtered photoresistor:
  greensensorValue = analogRead(greensensorPin);
  // give the ADC a moment to settle
  delay(5);
  // read the value from the blue-filtered photoresistor:
  bluesensorValue = analogRead(bluesensorPin);  

  // print out the values to the serial monitor  
  Serial.print("raw sensor Values \t red: ");
  Serial.print(redsensorValue);
  Serial.print("\t green: ");
  Serial.print(greensensorValue);
  Serial.print("\t Blue: ");
  Serial.println(bluesensorValue);

  /*
  In order to use the values from the sensor for the LED, 
  you need to do some math. The ADC provides a 10-bit number, 
  but analogWrite() uses 8 bits. You'll want to divide your 
  sensor readings by 4 to keep them in range of the output. 
  */
  redValue = redsensorValue/4;
  greenValue = greensensorValue/4;
  blueValue = bluesensorValue/4;  

  //  print out the mapped values  
  Serial.print("Mapped sensor Values \t red: ");
  Serial.print(redValue);
  Serial.print("\t green: ");
  Serial.print(greenValue);
  Serial.print("\t Blue: ");
  Serial.println(blueValue); 

  /*
  Now that you have a usable value, it's time to PWM the LED.
  */
  analogWrite(redLedPin, redValue);
  analogWrite(greenLedPin, greenValue);
  analogWrite(blueLedPin, blueValue);
}

