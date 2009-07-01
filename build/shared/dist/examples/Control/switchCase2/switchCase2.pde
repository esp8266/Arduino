/*
  Switch statement  with serial input
 
 Demonstrates the use of a switch statement.  The switch
 statement allows you to choose from among a set of discrete values
 of a variable.  It's like a series of if statements.
 
 To see this sketch in action, open the Serial monitor and send any character.
 The characters a, b, c, d, and e, will turn on LEDs.  Any other character will turn
 the LEDs off.
 
 The circuit:
 * 5 LEDs attached to digital pins 2 through 6
 
 created 1 Jul 2009
 by Tom Igoe 
 
 http://www.arduino.cc/en/Tutorial/SwitchCase2
 */

// these constants won't change:
const int sensorMin = 0;      // sensor minimum, discovered through experiment
const int sensorMax = 600;    // sensor maximum, discovered through experiment

void setup() {
  // initialize serial communication:
  Serial.begin(9600);  
}

void loop() {
  // read the sensor:
  int sensorReading = analogRead(0);
  // map the sensor range to a range of four options:
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);

  // do something different depending on the 
  // range value:
  switch (range) {
  case 0:    // your hand is on the sensor
    Serial.println("dark");
    break;
  case 1:    // your hand is close to the sensor
    Serial.println("dim");
    break;
  case 2:    // your hand is a few inches from the sensor
    Serial.println("medium");
    break;
  case 3:    // your hand is nowhere near the sensor
    Serial.println("bright");
    break;
  } 

}





