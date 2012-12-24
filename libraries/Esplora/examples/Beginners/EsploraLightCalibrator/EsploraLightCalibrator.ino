/*
  Esplora Led calibration
 
 This  sketch shows you how to read and calibrate the light sensor.
 Because light levels vary from one location to another, you need to calibrate the 
 sensor for each location. To do this, you read the sensor for a few seconds,
 and save the highest and lowest readings as maximum and minimum.  
 Then, when you're using the sensor's reading (for example, to set the brightness
 of the LED), you map the sensor's reading to a range between the minimum
 and the maximum.
 
 Created on 22 Dec 2012
 by Tom Igoe
 
 This example is in the public domain.
 */

#include <Esplora.h>

// variables:
int lightMin = 1023;        // minimum sensor value
int lightMax = 0;           // maximum sensor value
boolean calibrated = false;  // whether the sensor's been calibrated yet

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);

  // print an intial message
  Serial.println("To calibrate the light sensor, press and hold Switch 1");
}

void loop() {
  // if switch 1 is pressed, go to the calibration function again:
  if (Esplora.readButton(1) == LOW) {
    calibrate();
  }
  // read the sensor into a variable:
  int light = Esplora.readLightSensor();

  // map the light level to a brightness level for the LED
  // using the calibration min and max:
  int brightness = map(light, lightMin, lightMax, 0, 255);
  // limit the brightness to a range from 0 to 255:
  brightness = constrain(brightness, 0, 255);
  // write the brightness to the blue LED. 
  Esplora.writeBlue(brightness);
  
  // if the calibration's been done, show the sensor and brightness
  // levels in the serial monitor:
  if (calibrated == true) {
    // print the light sensor levels and the LED levels (to see what's going on):
    Serial.print("light sensor level: ");
    Serial.print(light);
    Serial.print(" blue brightness: ");
    Serial.println(brightness);
  }
  // add a delay to keep the LED from flickering:
  delay(10); 
}

void calibrate() {
  // tell the user what do to using the serial monitor:
  Serial.println("While holding switch 1, shine a light on the light sensor, then cover it.");

  // calibrate while switch 1 is pressed:
  while(Esplora.readButton(1) == LOW) {
    // read the sensor value: 
    int light  = Esplora.readLightSensor();

    // record the maximum sensor value:
    if (light > lightMax) {
      lightMax = light;
    }

    // record the minimum sensor value:
    if (light < lightMin) {
      lightMin = light;
    }
    // note that you're calibrated, for future reference:
    calibrated = true;
  }
}








