/*
  String length() and trim()
 
 Examples of how to use length() and trim() in a String
 
 created 27 July 2010
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/StringLengthTrim
 
 This example code is in the public domain.
 */

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nString  length() and trim():");
}

void loop() {
  // here's a String with empty spaces at the end (called white space):
  String stringOne = "Hello!       ";
  Serial.print(stringOne);
  Serial.print("<--- end of string. Length: ");
  Serial.println(stringOne.length());

  // trim the white space off the string:
  stringOne = stringOne.trim();
  Serial.print(stringOne);
  Serial.print("<--- end of trimmed string. Length: ");
  Serial.println(stringOne.length());

  // do nothing while true:
  while(true);
}
