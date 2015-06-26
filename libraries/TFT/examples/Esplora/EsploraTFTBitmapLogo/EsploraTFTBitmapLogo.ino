/*

 Esplora TFT Bitmap Logos

 This example for the Arduino TFT screen is for use
 with an Arduino Esplora.

 This example reads an image file from a micro-SD card
 and draws it on the screen, at random locations.

 There is a .bmp file included with this sketch.
 - open the sketch folder (Ctrl-K or Cmd-K)
 - copy the "arduino.bmp" file to a micro-SD
 - put the SD into the SD slot of the Arduino LCD module.

 This example code is in the public domain.

 Created 19 April 2013 by Enrico Gueli

 http://www.arduino.cc/en/Tutorial/EsploraTFTBitmapLogo

 */

// include the necessary libraries
#include <Esplora.h>
#include <SPI.h>
#include <SD.h>
#include <TFT.h>            // Arduino LCD library

// the Esplora pin connected to the chip select line for SD card
#define SD_CS    8

// this variable represents the image to be drawn on screen
PImage logo;

void setup() {
  // initialize the GLCD and show a message
  // asking the user to open the serial line
  EsploraTFT.begin();
  EsploraTFT.background(255, 255, 255);

  EsploraTFT.stroke(0, 0, 255);
  EsploraTFT.println();
  EsploraTFT.println(F("Arduino LCD Bitmap Example"));
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.println(F("Open serial monitor"));
  EsploraTFT.println(F("to run the sketch"));

  // initialize the serial port: it will be used to
  // print some diagnostic info
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial monitor to be open
  }

  // try to access the SD card. If that fails (e.g.
  // no card present), the Esplora's LED will turn red.
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("failed!"));
    Esplora.writeRed(255);
    return;
  }
  Serial.println("OK!");

  // clear the GLCD screen before starting
  EsploraTFT.background(255, 255, 255);

  // now that the SD card can be access, try to load the
  // image file. The Esplora LED will turn green or red if
  // the loading went OK or not.
  Esplora.writeRGB(0, 0, 0);
  logo = EsploraTFT.loadImage("arduino.bmp");
  if (logo.isValid()) {
    Esplora.writeGreen(255);
  }
  else
    Esplora.writeRed(255);

}

void loop() {
  // don't do anything if the image wasn't loaded correctly.
  if (logo.isValid() == false) {
    return;
  }

  Serial.println(F("drawing image"));

  // get a random location where to draw the image.
  // To avoid the image to be draw outside the screen,
  // take into account the image size.
  int x = random(EsploraTFT.width() - logo.width());
  int y = random(EsploraTFT.height() - logo.height());

  // draw the image to the screen
  EsploraTFT.image(logo, x, y);

  // wait a little bit before drawing again
  delay(1500);
}
