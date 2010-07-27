/*
   String constructors
 
 Examples of how  to create strings from other data types
 
 The circuit:  No external hardware needed.
 
 created 27 July 2010
 by Tom Igoe
 
 This example code is in the public domain.
 
 */

void setup() {
  Serial.begin(9600);
}

void loop() {
  
    String stringOne = "Hello String";     // using a constant String
    Serial.println(stringOne);      // prints "Hello String"

    stringOne =  String('a');        // converting a constant char into a String
    Serial.println(stringOne);       // prints "a"

    String stringTwo =  String("This is a string");  // converting a constant string into a String object
    Serial.println(stringTwo);      // prints "This is a string"

    stringOne =  String(stringTwo + " with more");  // concatenating two strings
    Serial.println(stringOne);      // prints "This is a string with more"

    stringOne =  String(13);        // using a constant integer
    Serial.println(stringOne);      // prints "13"
    
    stringOne =  String(analogRead(0), DEC);  // using an int and a base
    Serial.println(stringOne);      // prints "453" or whatever the value of analogRead(0) is

    stringOne =  String(45, HEX);   // using an int and a base (hexadecimal)
    Serial.println(stringOne);      // prints "2d", which is the hexadecimal version of decimal 45

    stringOne =  String(255, BIN);  // using an int and a base (binary)
    Serial.println(stringOne);      // prints "11111111" which is the binary value of 255


    stringOne =  String(millis(), DEC); // using a long and a base
    Serial.println(stringOne);      // prints "123456" or whatever the value of millis() is

  // do nothing while true:
  while(true);

}








