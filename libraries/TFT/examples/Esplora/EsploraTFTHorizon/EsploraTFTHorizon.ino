/*

 Esplora TFT Horizon

 This example for the Arduino TFT and Esplora draws
 a line on the screen that stays level with the ground
 as you tile the Esplora side to side

 This example code is in the public domain.

 Created 15 April 2013 by Scott Fitzgerald

 http://www.arduino.cc/en/Tutorial/EsploraTFTHorizon

 */

#include <Esplora.h>
#include <TFT.h>            // Arduino LCD library
#include <SPI.h>

// horizontal start and end positions
int yStart = EsploraTFT.height() / 2;
int yEnd = EsploraTFT.height() / 2;

// previous start and end positions
int oldEndY;
int oldStartY;

void setup() {
  // initialize the display
  EsploraTFT.begin();
  // make the background black
  EsploraTFT.background(0, 0, 0);
}

void loop()
{
  // read the x-axis of te accelerometer
  int tilt = Esplora.readAccelerometer(X_AXIS);

  // the values are 100 when tilted to the left
  // and  -100 when tilted to the right
  // map these values to the start and end points
  yStart = map(tilt, -100, 100, EsploraTFT.height(), 0);
  yEnd = map(tilt, -100, 100, 0, EsploraTFT.height());

  // if the previous values are different than the current values
  // erase the previous line
  if (oldStartY != yStart || oldEndY != yEnd) {
    EsploraTFT.stroke(0, 0, 0);
    EsploraTFT.line(0, oldStartY, EsploraTFT.width(), oldEndY);
  }

  // draw the line in magenta
  EsploraTFT.stroke(255, 0, 255);
  EsploraTFT.line(0, yStart, EsploraTFT.width(), yEnd);

  // save the current start and end points
  // to compare int he next loop
  oldStartY = yStart;
  oldEndY = yEnd;
  delay(10);
}
