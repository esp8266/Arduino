/*

 TFT EtchASketch

 This example for the Arduino screen draws a white point
 on the GLCD based on the values of 2 potentiometers.
 To clear the screen, press a button attached to pin 2.

 This example code is in the public domain.

 Created 15 April 2013 by Scott Fitzgerald

 http://arduino.cc/en/Tutorial/TFTEtchASketch

 */

#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

// pin definition for the Leonardo
// #define cs   7
// #define dc   0
// #define rst  1

TFT TFTscreen = TFT(cs, dc, rst);

// initial position of the cursor
int xPos = TFTscreen.width() / 2;
int yPos = TFTscreen.height() / 2;

// pin the erase switch is connected to
int erasePin = 2;

void setup() {
  // declare inputs
  pinMode(erasePin, INPUT);
  // initialize the screen
  TFTscreen.begin();
  // make the background black
  TFTscreen.background(0, 0, 0);
}

void loop()
{
  // read the potentiometers on A0 and A1
  int xValue = analogRead(A0);
  int yValue = analogRead(A1);

  // map the values and update the position
  xPos = xPos + (map(xValue, 0, 1023, 2, -2));
  yPos = yPos + (map(yValue, 0, 1023, -2, 2));

  // don't let the point go past the screen edges
  if (xPos > 159) {
    (xPos = 159);
  }

  if (xPos < 0) {
    (xPos = 0);
  }
  if (yPos > 127) {
    (yPos = 127);
  }

  if (yPos < 0) {
    (yPos = 0);
  }

  // draw the point
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.point(xPos, yPos);

  // read the value of the pin, and erase the screen if pressed
  if (digitalRead(erasePin) == HIGH) {
    TFTscreen.background(0, 0, 0);
  }

  delay(33);
}
