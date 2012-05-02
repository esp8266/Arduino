/*
  Copyright (c) 2012 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "variant.h"
#include <stdio.h>

// set pin numbers for the five buttons:
const int upButton = 2;
const int downButton = 3;
const int leftButton = 4;
const int rightButton = 5;
const int mouseButton = 6;

int range = 5;              // output range of X or Y movement; affects movement speed
int responseDelay = 10;     // response delay of the mouse, in ms


void setup() {
  // initialize the buttons' inputs:
 /* pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(mouseButton, INPUT);*/
  // initialize mouse control:
  Mouse.begin();
}

void loop() {
  // read the buttons:
 /* int upState = digitalRead(upButton);
  int downState = digitalRead(downButton);
  int rightState = digitalRead(rightButton);
  int leftState = digitalRead(leftButton);
  int clickState = digitalRead(mouseButton);

  // calculate the movement distance based on the button states:
  int  xDistance = (leftState - rightState)*range;
  int  yDistance = (upState - downState)*range;

  // if X or Y is non-zero, move:
  if ((xDistance != 0) || (yDistance != 0)) {
    Mouse.move(xDistance, yDistance, 0);
  }

  // if the mouse button is pressed:
  if (clickState == HIGH) {
    // if the mouse is not pressed, press it:
    if (!Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.press(MOUSE_LEFT);
    }
  }
  // else the mouse button is not pressed:
  else {
    // if the mouse is pressed, release it:
    if (Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.release(MOUSE_LEFT);
    }
  }*/
Mouse.move(10, 10, 0);
  // a delay so the mouse doesn't move too fast:
  delay(responseDelay);
}



//#include "USBAPI.h"



// Cet exemple fonctionne !!! ;-)
/*void setup() {
	// initialize the digital pin as an output.
	// Pin 13 has an LED connected on most Arduino boards:
	//pinMode(13, OUTPUT);
	Serial1.begin(115200) ;
	printf("USB test starting...\r\n");
}

void loop() {
	//digitalWrite(13, HIGH);   // set the LED on
	//delay(1000);              // wait for a second
	//digitalWrite(13, LOW);    // set the LED off
	delay(1000);              // wait for a second
	printf("loop...\r\n");
}*/

