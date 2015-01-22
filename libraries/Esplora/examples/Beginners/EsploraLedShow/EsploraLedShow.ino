/*
  Esplora LED Show

  Makes the RGB LED bright and glow as the joystick or the
  slider are moved.

  Created on 22 november 2012
  By Enrico Gueli <enrico.gueli@gmail.com>
  Modified 22 Dec 2012
  by Tom Igoe
*/
#include <Esplora.h>

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
}

void loop() {
  // read the sensors into variables:
  int xAxis = Esplora.readJoystickX();
  int yAxis = Esplora.readJoystickY();
  int slider = Esplora.readSlider();

  // convert the sensor readings to light levels:
  byte red   = map(xAxis, -512, 512, 0, 255);
  byte green = map(yAxis, -512, 512, 0, 255);
  byte blue  = slider / 4;

  // print the light levels:
  Serial.print(red);
  Serial.print(' ');
  Serial.print(green);
  Serial.print(' ');
  Serial.println(blue);

  // write the light levels to the LED.
  Esplora.writeRGB(red, green, blue);

  // add a delay to keep the LED from flickering:
  delay(10);
}
