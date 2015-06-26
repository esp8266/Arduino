/*
  String replace()

 Examples of how to replace characters or substrings of a string

 created 27 July 2010
 modified 2 Apr 2012
 by Tom Igoe

 http://www.arduino.cc/en/Tutorial/StringReplace

 This example code is in the public domain.
 */

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // send an intro:
  Serial.println("\n\nString  replace:\n");
  Serial.println();
}

void loop() {
  String stringOne = "<html><head><body>";
  Serial.println(stringOne);
  // replace() changes all instances of one substring with another:
  // first, make a copy of th original string:
  String stringTwo = stringOne;
  // then perform the replacements:
  stringTwo.replace("<", "</");
  // print the original:
  Serial.println("Original string: " + stringOne);
  // and print the modified string:
  Serial.println("Modified string: " + stringTwo);

  // you can also use replace() on single characters:
  String normalString = "bookkeeper";
  Serial.println("normal: " + normalString);
  String leetString = normalString;
  leetString.replace('o', '0');
  leetString.replace('e', '3');
  Serial.println("l33tspeak: " + leetString);

  // do nothing while true:
  while (true);
}
