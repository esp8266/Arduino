/*
  Console Pixel

 An example of using the Arduino board to receive data from the
 Console on the Arduino Yún.  In this case, the Arduino boards turns on an LED when
 it receives the character 'H', and turns off the LED when it
 receives the character 'L'.

 To see the Console, pick your Yún's name and IP address in the Port menu
 then open the Port Monitor. You can also see it by opening a terminal window
 and typing
 ssh root@ yourYunsName.local 'telnet localhost 6571'
 then pressing enter. When prompted for the password, enter it.


 The circuit:
 * LED connected from digital pin 13 to ground

 created 2006
 by David A. Mellis
 modified 25 Jun 2013
 by Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/ConsolePixel

 */

#include <Console.h>

const int ledPin = 13; // the pin that the LED is attached to
char incomingByte;      // a variable to read incoming Console data into

void setup() {
  Bridge.begin();   // Initialize Bridge
  Console.begin();  // Initialize Console

  // Wait for the Console port to connect
  while (!Console);

  Console.println("type H or L to turn pin 13 on or off");

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // see if there's incoming Console data:
  if (Console.available() > 0) {
    // read the oldest byte in the Console buffer:
    incomingByte = Console.read();
    Console.println(incomingByte);
    // if it's a capital H (ASCII 72), turn on the LED:
    if (incomingByte == 'H') {
      digitalWrite(ledPin, HIGH);
    }
    // if it's an L (ASCII 76) turn off the LED:
    if (incomingByte == 'L') {
      digitalWrite(ledPin, LOW);
    }
  }
}

