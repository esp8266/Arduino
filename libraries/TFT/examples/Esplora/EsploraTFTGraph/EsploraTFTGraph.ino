/*

 Esplora TFT Graph

 This example for the Esplora with an Arduino TFT reads
 the value of the light sensor, and graphs the values on
 the screen.

 This example code is in the public domain.

 Created 15 April 2013 by Scott Fitzgerald

 http://www.arduino.cc/en/Tutorial/EsploraTFTGraph

 */

#include <Esplora.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// position of the line on screen
int xPos = 0;

void setup() {

  // initialize the screen
  EsploraTFT.begin();

  // clear the screen with a nice color
  EsploraTFT.background(250, 16, 200);
}

void loop() {

  // read the sensor value
  int sensor = Esplora.readLightSensor();
  // map the sensor value to the height of the screen
  int graphHeight = map(sensor, 0, 1023, 0, EsploraTFT.height());

  // draw the line in a pretty color
  EsploraTFT.stroke(250, 180, 10);
  EsploraTFT.line(xPos, EsploraTFT.height() - graphHeight, xPos, EsploraTFT.height());

  // if the graph reaches the edge of the screen
  // erase it and start over from the other side
  if (xPos >= 160) {
    xPos = 0;
    EsploraTFT.background(250, 16, 200);
  }
  else {
    // increment the horizontal position:
    xPos++;
  }

  delay(16);
}
