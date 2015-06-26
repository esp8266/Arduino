/*

 Esplora TFT Color Picker

 This example for the Esplora with an Arduino TFT reads
 the input of the joystick and slider, using the values
 to change the screen's color.

 This example code is in the public domain.

 Created 15 April 2013 by Scott Fitzgerald

 http://www.arduino.cc/en/Tutorial/TFTColorPicker

 */

#include <Esplora.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

void setup() {
  Serial.begin(9600);

  // initialize the LCD
  EsploraTFT.begin();

  // start out with a white screen
  EsploraTFT.background(255, 255, 255);

}

void loop() {

  // map the values from sensors
  int xValue = map(Esplora.readJoystickX(), -512, 512, 0, 255);  // read the joystick's X position
  int yValue = map(Esplora.readJoystickY(), -512, 512, 0, 255);  // read the joystick's Y position
  int slider = map(Esplora.readSlider(), 0, 1023, 0, 255);   // read the slider's position

  // change the background color based on the mapped values
  EsploraTFT.background(xValue, yValue, slider);

  // print the mapped values to the Serial monitor
  Serial.print("background(");
  Serial.print(xValue);
  Serial.print(" , ");
  Serial.print(yValue);
  Serial.print(" , ");
  Serial.print(slider);
  Serial.println(")");

  delay(33);

}

