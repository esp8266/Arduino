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
  Serial.begin(9600); 
}

void loop() {
  int slider = Esplora.readSlider();
  int resetButton = Esplora.readButton(1);
  int serveButton = Esplora.readButton(3);
  int switchPlayerButton = Esplora.readButton(4);

  Serial.print(slider);
  Serial.print(",");
  Serial.print(resetButton);
  Serial.print(",");
  Serial.print(serveButton);
  Serial.print(",");
  Serial.println(switchPlayerButton);
  delay(10);
}

