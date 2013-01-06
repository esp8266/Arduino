/*
  Esplora Led/Microphone

  This simple sketch reads the microphone, light sensor, and slider.
  Then it uses those readings to set the brightness of red, green and blue
  channels of the RGB LED. The red channel will change with the loudness
  "heared" by the microphone, the green channel changes as the
  amount of light in the room and the blue channel will change
  with the position of the slider.

  Created on 22 november 2012
  By Enrico Gueli <enrico.gueli@gmail.com>
  Modified 24 Nov 2012
  by Tom Igoe
*/

#include <Esplora.h>

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
}

int lowLight = 400;   // the light sensor reading when it's covered
int highLight = 1023; // the maximum light sensor reading 
int minGreen = 0;     // minimum brightness of the green LED
int maxGreen = 100;   // maximum brightness of the green LED

void loop() {
  // read the sensors into variables:
  int mic = Esplora.readMicrophone();
  int light = Esplora.readLightSensor();
  int slider = Esplora.readSlider();
  
  // convert the sensor readings to light levels:
  byte red   = constrain(mic, 0, 255);
  byte green = constrain(
                  map(light, lowLight, highLight, minGreen, maxGreen),
                  0, 255);
  byte blue  = slider/4;

  // print the light levels (to see what's going on):
  Serial.print(red);
  Serial.print(' ');
  Serial.print(green);
  Serial.print(' ');
  Serial.println(blue);

  // write the light levels to the LED. 
  // note that the green value is always 0:
  Esplora.writeRGB(red, green, blue);
  
  // add a delay to keep the LED from flickering:
  delay(10); 
}
