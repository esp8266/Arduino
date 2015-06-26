/*

 Arduino TFT Bitmap Logo example

 This example reads an image file from a micro-SD card
 and draws it on the screen, at random locations.

 In this sketch, the Arduino logo is read from a micro-SD card.
 There is a .bmp file included with this sketch.
 - open the sketch folder (Ctrl-K or Cmd-K)
 - copy the "arduino.bmp" file to a micro-SD
 - put the SD into the SD slot of the Arduino TFT module.

 This example code is in the public domain.

 Created 19 April 2013 by Enrico Gueli

 http://www.arduino.cc/en/Tutorial/TFTBitmapLogo

 */

// include the necessary libraries
#include <SPI.h>
#include <SD.h>
#include <TFT.h>  // Arduino LCD library

// pin definition for the Uno
#define sd_cs  4
#define lcd_cs 10
#define dc     9
#define rst    8

// pin definition for the Leonardo
//#define sd_cs  8
//#define lcd_cs 7
//#define dc     0
//#define rst    1

TFT TFTscreen = TFT(lcd_cs, dc, rst);

// this variable represents the image to be drawn on screen
PImage logo;


void setup() {
  // initialize the GLCD and show a message
  // asking the user to open the serial line
  TFTscreen.begin();
  TFTscreen.background(255, 255, 255);

  TFTscreen.stroke(0, 0, 255);
  TFTscreen.println();
  TFTscreen.println(F("Arduino TFT Bitmap Example"));
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.println(F("Open serial monitor"));
  TFTscreen.println(F("to run the sketch"));

  // initialize the serial port: it will be used to
  // print some diagnostic info
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial line to be ready
  }

  // clear the GLCD screen before starting
  TFTscreen.background(255, 255, 255);

  // try to access the SD card. If that fails (e.g.
  // no card present), the setup process will stop.
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(sd_cs)) {
    Serial.println(F("failed!"));
    return;
  }
  Serial.println(F("OK!"));

  // initialize and clear the GLCD screen
  TFTscreen.begin();
  TFTscreen.background(255, 255, 255);

  // now that the SD card can be access, try to load the
  // image file.
  logo = TFTscreen.loadImage("arduino.bmp");
  if (!logo.isValid()) {
    Serial.println(F("error while loading arduino.bmp"));
  }
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
  int x = random(TFTscreen.width() - logo.width());
  int y = random(TFTscreen.height() - logo.height());

  // draw the image to the screen
  TFTscreen.image(logo, x, y);

  // wait a little bit before drawing again
  delay(1500);
}
