
/*
  Esplora Blink

 This  sketch blinks the Esplora's RGB LED. It goes through
 all three primary colors (red, green, blue), then it
 combines them for secondary colors(yellow, cyan, magenta), then
 it turns on all the colors for white.
 For best results cover the LED with a piece of white paper to see the colors.

 Created on 22 Dec 2012
 by Tom Igoe

 This example is in the public domain.
 */

#include <Esplora.h>


void setup() {
  // There's nothing to set up for this sketch
}

void loop() {
  Esplora.writeRGB(255, 0, 0);  // make the LED red
  delay(1000);                  // wait 1 second
  Esplora.writeRGB(0, 255, 0);  // make the LED green
  delay(1000);                  // wait 1 second
  Esplora.writeRGB(0, 0, 255);  // make the LED blue
  delay(1000);                  // wait 1 second
  Esplora.writeRGB(255, 255, 0); // make the LED yellow
  delay(1000);                  // wait 1 second
  Esplora.writeRGB(0, 255, 255); // make the LED cyan
  delay(1000);                  // wait 1 second
  Esplora.writeRGB(255, 0, 255); // make the LED magenta
  delay(1000);                  // wait 1 second
  Esplora.writeRGB(255, 255, 255); // make the LED white
  delay(1000);                  // wait 1 second

}


