/*
  Esplora Temperature Sensor

 This  sketch shows you how to read the Esplora's temperature sensor
 You can read the temperature sensor in Farhenheit or Celsius.

 Created on 22 Dec 2012
 by Tom Igoe

 This example is in the public domain.
 */
#include <Esplora.h>

void setup()
{
  Serial.begin(9600);      // initialize serial communications with your computer
}

void loop()
{
  // read the temperature sensor in Celsius, then Fahrenheit:
  int celsius = Esplora.readTemperature(DEGREES_C);
  int fahrenheit = Esplora.readTemperature(DEGREES_F);

  // print the results:
  Serial.print("Temperature is: ");
  Serial.print(celsius);
  Serial.print(" degrees Celsius, or ");
  Serial.print(fahrenheit);
  Serial.println(" degrees Fahrenheit.");
  Serial.println("     Fahrenheit = (9/5 * Celsius) + 32");

  // wait a second before reading again:
  delay(1000);
}


