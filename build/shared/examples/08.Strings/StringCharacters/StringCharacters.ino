/*
  String charAt() and setCharAt()
 
 Examples of how to get and set characters of a String
 
 created 27 July 2010
 modified 2 Apr 2012
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/StringCharacters
 
 This example code is in the public domain.
 */

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("\n\nString  charAt() and setCharAt():");
}

void loop() {
  // make a string to report a sensor reading:
  String reportString = "SensorReading: 456";
  Serial.println(reportString);

  // the reading's most significant digit is at position 15 in the reportString:
  char mostSignificantDigit = reportString.charAt(15);

  String message = "Most significant digit of the sensor reading is: "; 
  Serial.println(message + mostSignificantDigit);

  // add blank space:
  Serial.println();

  // you can alo set the character of a string. Change the : to a = character
  reportString.setCharAt(13, '='); 
  Serial.println(reportString);

  // do nothing while true:
  while(true);
}

