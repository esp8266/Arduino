/*
  Esplora Pong

 This  sketch connects serially to a Processing sketch to control a Pong game.
 It sends the position of the slider and the states of three pushbuttons to the
 Processing sketch serially, separated by commas. The Processing sketch uses that
 data to control the graphics in the sketch.

 The slider sets a paddle's height
 Switch 1 is resets the game
 Switch 2 resets the ball to the center
 Switch 3 reverses the players

 You can play this game with one or two Esploras.

 Created on 22 Dec 2012
 by Tom Igoe

 This example is in the public domain.
 */

#include <Esplora.h>

void setup() {
  Serial.begin(9600);     // initialize serial communication
}

void loop() {
  // read the slider and three of the buttons
  int slider = Esplora.readSlider();
  int resetButton = Esplora.readButton(SWITCH_1);
  int serveButton = Esplora.readButton(SWITCH_3);
  int switchPlayerButton = Esplora.readButton(SWITCH_4);

  Serial.print(slider);                // print the slider value
  Serial.print(",");                   // add a comma
  Serial.print(resetButton);           // print the reset button value
  Serial.print(",");                   // add another comma
  Serial.print(serveButton);           // print the serve button value
  Serial.print(",");                   // add another comma
  Serial.println(switchPlayerButton);  // print the last button with a newline
  delay(10);                           // delay before sending the next set
}

