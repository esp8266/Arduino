/*
  Pitch follower
 
 Plays a pitch that changes based on a changing analog input
 
 circuit:
 * 8-ohm speaker on digital pin 8
 * photoresistor on analog 0 to 5V
 * 4.7K resistor on analog 0 to ground
 
 created 21 Jan 2010
 modified 9 Apr 2012
 by Tom Igoe 

This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/Tone2
 
 */


void setup() {
  // initialize serial communications (for debugging only):
  Serial.begin(9600);
}

void loop() {
  // read the sensor:
  int sensorReading = analogRead(A0);
  // print the sensor reading so you know its range
  Serial.println(sensorReading);
  // map the pitch to the range of the analog input.
  // change the minimum and maximum input numbers below
  // depending on the range your sensor's giving:
  int thisPitch = map(sensorReading, 400, 1000, 100, 1000);

  // play the pitch:
  tone(9, thisPitch, 10);
  delay(1);        // delay in between reads for stability
}






