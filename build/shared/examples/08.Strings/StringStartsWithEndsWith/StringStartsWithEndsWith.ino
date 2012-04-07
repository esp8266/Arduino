/*
  String startWith() and endsWith()
 
 Examples of how to use startsWith() and endsWith() in a String
 
 created 27 July 2010
 modified 2 Apr 2012
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/StringStartsWithEndsWith
 
 This example code is in the public domain.
 */

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // send an intro:
  Serial.println("\n\nString startsWith() and endsWith():");
  Serial.println();
}

void loop() {
  // startsWith() checks to see if a String starts with a particular substring:
  String stringOne = "HTTP/1.1 200 OK";
  Serial.println(stringOne);
  if (stringOne.startsWith("HTTP/1.1")) {
    Serial.println("Server's using http version 1.1"); 
  } 

  // you can also look for startsWith() at an offset position in the string:
  stringOne = "HTTP/1.1 200 OK";
  if (stringOne.startsWith("200 OK", 9)) {
    Serial.println("Got an OK from the server"); 
  } 

  // endsWith() checks to see if a String ends with a particular character:
  String sensorReading = "sensor = ";
  sensorReading += analogRead(A0);
  Serial.print (sensorReading);
  if (sensorReading.endsWith(0)) {
    Serial.println(". This reading is divisible by ten"); 
  } 
  else {
    Serial.println(". This reading is not divisible by ten"); 

  }

  // do nothing while true:
  while(true);
}
