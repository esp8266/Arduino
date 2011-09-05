/*
  String replace()
 
 Examples of how to replace characters or substrings of a string
 
 created 27 July 2010
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/StringReplace
 
 This example code is in the public domain. 
 */

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nString  replace:");
}

void loop() {
  String stringOne = "<html><head><body>";
  Serial.println(stringOne);
  // replace() changes all instances of one substring with another:
  String stringTwo = stringOne.replace("<", "</");
  Serial.println(stringTwo);

  // you can also use replace() on single characters:
  String normalString = "bookkeeper";
  Serial.println("normal: " + normalString);
  String leetString = normalString.replace('o', '0');
  leetString = leetString.replace('e', '3');
  Serial.println("l33tspeak: " + leetString);

  // do nothing while true:
  while(true);
}