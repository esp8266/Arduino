/*
 * Smoothing
 * David A. Mellis <dam@mellis.org>
 *
 * Reads repeatedly from an analog input, calculating a running average
 * and printing it to the computer. 
 *
 * http://www.arduino.cc/en/Tutorial/Smoothing
 */

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a #define rather than a normal variable lets
// use this value to determine the size of the readings array.
#define NUMREADINGS 10

int readings[NUMREADINGS];                // the readings from the analog input
int index = 0;                            // the index of the current reading
int total = 0;                            // the running total
int average = 0;                          // the average

int inputPin = 0;

void setup()
{
  Serial.begin(9600);                     // initialize serial communication with computer
  for (int i = 0; i < NUMREADINGS; i++)
    readings[i] = 0;                      // initialize all the readings to 0
}

void loop()
{
  total -= readings[index];               // subtract the last reading
  readings[index] = analogRead(inputPin); // read from the sensor
  total += readings[index];               // add the reading to the total
  index = (index + 1);                    // advance to the next index
  
  if (index >= NUMREADINGS)               // if we're at the end of the array...
    index = 0;                            // ...wrap around to the beginning
  
  average = total / NUMREADINGS;          // calculate the average
  Serial.println(average);                // send it to the computer (as ASCII digits)
}
