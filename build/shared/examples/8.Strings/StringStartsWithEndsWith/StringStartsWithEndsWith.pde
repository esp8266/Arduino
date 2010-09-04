/*
  String startWith() and endsWith()
 
 Examples of how to use startsWith() and endsWith() in a String
 
 created 27 July 2010
 modified 4 Sep 2010
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/StringStartsWithEndsWith
 
 This example code is in the public domain.
 */

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nString startsWith() and endsWith():");

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