/*
  Analog input, serial output
 
 Reads an analog input pin, prints the results to the serial monitor.
 
 The circuit:

 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 
 created over and over again
 by Tom Igoe and everyone who's ever used Arduino
 
 */
 
 void setup() {
  Serial.begin(9600); 
 }
 
 void loop() {
  // read the analog input into a variable:
   int analogValue = analogRead(0);
   // print the result:
   Serial.println(analogValue);
   // wait 10 milliseconds for the analog-to-digital converter
   // to settle after the last reading:
   delay(10);
 }
