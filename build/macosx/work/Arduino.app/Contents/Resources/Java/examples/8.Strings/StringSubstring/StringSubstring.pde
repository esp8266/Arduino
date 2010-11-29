/*
  String substring()
 
 Examples of how to use substring in a String
 
 created 27 July 2010
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/StringSubstring
 
 This example code is in the public domain.
 */

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nString  substring():");
}

void loop() {
  // Set up a String:
  String stringOne = "Content-Type: text/html";
  Serial.println(stringOne);
  
  // substring(index) looks for the substring from the index position to the end:
  if (stringOne.substring(19) == "html") {
    Serial.println("It's an html file"); 
  } 
  // you can also look for a substring in the middle of a string:
  if (stringOne.substring(14,18) == "text") {
    Serial.println("It's a text-based file"); 
  } 

  // do nothing while true:
  while(true);
}