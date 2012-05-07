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

/*
void setup() {
	Mouse.begin();
}

void loop() {
	Mouse.move(10, 0, 0);
	delay(1000);
}
*/


/*
void setup() {
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
}
*/

void setup() {
	Serial.begin(57600);
}

void loop() {

	if (Serial.available() > 0)
	{
		char inChar = Serial.read();
		Serial.print(inChar);
		Serial1.print(inChar);
	}
	delay(10);
}

