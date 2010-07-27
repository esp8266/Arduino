/*
  String Case changes
 
 Examples of how to change the case of a string
 
 created 27 July 2010
 by Tom Igoe
 
 This example code is in the public domain.
 */

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nString  case changes:");
}

void loop() {
  // toUpperCase() changes all letters to upper case:
  String stringOne = "<html><head><body>";
  Serial.println(stringOne.toUpperCase());
  
 // toLowerCase() changes all letters to lower case:  
  String stringTwo = "</BODY></HTML>";
  Serial.println(stringTwo.toLowerCase());
  
  // do nothing while true:
  while(true);
}

















