/*

 Esplora TFT Temperature Display

 This example for the Arduino TFT screen is for use
 with an Arduino Esplora.

 This example reads the temperature of the Esplora's
 on board thermisistor and displays it on an attached
 LCD screen, updating every second.

 This example code is in the public domain.

 Created 15 April 2013 by Scott Fitzgerald

 http://arduino.cc/en/Tutorial/EsploraTFTTemp

 */

// include the necessary libraries
#include <Esplora.h>
#include <TFT.h>            // Arduino LCD library
#include <SPI.h>

char tempPrintout[3];  // array to hold the temperature data

void setup() {

  // Put this line at the beginning of every sketch that uses the GLCD
  EsploraTFT.begin();

  // clear the screen with a black background
  EsploraTFT.background(0, 0, 0);

  // set the text color to magenta
  EsploraTFT.stroke(200, 20, 180);
  // set the text to size 2
  EsploraTFT.setTextSize(2);
  // start the text at the top left of the screen
  // this text is going to remain static
  EsploraTFT.text("Degrees in C :\n ", 0, 0);

  // set the text in the loop to size 5
  EsploraTFT.setTextSize(5);
}

void loop() {

  // read the temperature  in Celcius and store it in a String
  String temperature = String(Esplora.readTemperature(DEGREES_C));

  // convert the string to a char array
  temperature.toCharArray(tempPrintout, 3);

  // set the text color to white
  EsploraTFT.stroke(255, 255, 255);
  // print the temperature one line below the static text
  EsploraTFT.text(tempPrintout, 0, 30);

  delay(1000);
  // erase the text for the next loop
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.text(tempPrintout, 0, 30);
}
