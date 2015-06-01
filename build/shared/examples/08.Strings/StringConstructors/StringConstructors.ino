/*
   String constructors

 Examples of how  to create strings from other data types

 created 27 July 2010
 modified 30 Aug 2011
 by Tom Igoe

 http://www.arduino.cc/en/Tutorial/StringConstructors

 This example code is in the public domain.
 */

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // send an intro:
  Serial.println("\n\nString Constructors:");
  Serial.println();
}

void loop() {
  // using a constant String:
  String stringOne = "Hello String";
  Serial.println(stringOne);      // prints "Hello String"

  // converting a constant char into a String:
  stringOne =  String('a');
  Serial.println(stringOne);       // prints "a"

  // converting a constant string into a String object:
  String stringTwo =  String("This is a string");
  Serial.println(stringTwo);      // prints "This is a string"

  // concatenating two strings:
  stringOne =  String(stringTwo + " with more");
  // prints "This is a string with more":
  Serial.println(stringOne);

  // using a constant integer:
  stringOne =  String(13);
  Serial.println(stringOne);      // prints "13"

  // using an int and a base:
  stringOne =  String(analogRead(A0), DEC);
  // prints "453" or whatever the value of analogRead(A0) is
  Serial.println(stringOne);

  // using an int and a base (hexadecimal):
  stringOne =  String(45, HEX);
  // prints "2d", which is the hexadecimal version of decimal 45:
  Serial.println(stringOne);

  // using an int and a base (binary)
  stringOne =  String(255, BIN);
  // prints "11111111" which is the binary value of 255
  Serial.println(stringOne);

  // using a long and a base:
  stringOne =  String(millis(), DEC);
  // prints "123456" or whatever the value of millis() is:
  Serial.println(stringOne);

  //using a float and the right decimal places:
  stringOne = String(5.698, 3);
  Serial.println(stringOne);

  //using a float and less decimal places to use rounding:
  stringOne = String(5.698, 2);
  Serial.println(stringOne);

  // do nothing while true:
  while (true);

}
