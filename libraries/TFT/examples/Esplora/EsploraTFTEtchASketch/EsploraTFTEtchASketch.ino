/*

 Esplora TFT EtchASketch

 This example for the Arduino TFT and Esplora draws
 a white line on the screen, based on the position
 of the joystick. To clear the screen, shake the
 Esplora, using the values from the accelerometer.

 This example code is in the public domain.

 Created 15 April 2013 by Scott Fitzgerald

 http://arduino.cc/en/Tutorial/EsploraTFTEtchASketch

 */

#include <Esplora.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// initial position of the cursor
int xPos = EsploraTFT.width() / 2;
int yPos = EsploraTFT.height() / 2;

void setup() {
  // initialize the display
  EsploraTFT.begin();

  // clear the background
  EsploraTFT.background(0, 0, 0);
}

void loop()
{

  int xAxis = Esplora.readJoystickX();    // read the X axis
  int yAxis = Esplora.readJoystickY();    // read the Y axis

  // update the position of the line
  // depending on the position of the joystick
  if (xAxis < 10 && xAxis > -10) {
    xPos = xPos;
  }
  else {
    xPos = xPos + (map(xAxis, -512, 512, 2, -2));
  }
  if (yAxis < 10 && yAxis > -10) {
    yAxis = yAxis;
  }
  else {
    yPos = yPos + (map(yAxis, -512, 512, -2, 2));
  }

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
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.point(xPos, yPos);

  // check the accelerometer values and clear
  // the screen if it is being shaken
  if (abs(Esplora.readAccelerometer(X_AXIS)) > 200 || abs(Esplora.readAccelerometer(Y_AXIS)) > 200) {
    EsploraTFT.background(0, 0, 0);
  }

  delay(33);
}

