/*
  String substring()
 
 Examples of how to use substring in a String
 
 created 27 July 2010, 
 modified 2 Apr 2012
 by Zach Eveland
 
 http://arduino.cc/en/Tutorial/StringSubstring
 
 This example code is in the public domain.
 */

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // send an intro:
  Serial.println("\n\nString  substring():");
  Serial.println();
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
