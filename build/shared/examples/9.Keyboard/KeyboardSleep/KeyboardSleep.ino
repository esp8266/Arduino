/*
  Keyboard wake-on-interrupt
 
 This sketch demonstrates the Keyboard library.
 
 With most operating systems, when the computer is in sleep mode,
 tapping the space bar will wake it up. This sketch puts your 
 computer to sleep, waits ten seconds, then wakes it up.
 
 
 Circuit:
 * Arduino Leonardo
 * wire to connect D12 to ground.
 
 created 5 Mar 2012
 by Tom Igoe
 
 This example is in the public domain
 
 http://www.arduino.cc/en/Tutorial/KeyboardSleep
 */

// give the operating systems numbers:
#define OSX 0
#define WIN7 1
#define WINXP 2
#define UBUNTU 3

int plastform = OSX;    // change this to match your platform

void setup() {
  // make pin 12 an input and turn on the 
  // pullup resistor so it goes high unless
  // connected to ground:
  pinMode(12, INPUT_PULLUP);
}

void loop() {
  while (digitalRead(12) == HIGH) {
    // do nothing until pin 12 goes low
    delay(500);
  }

  //Put the computer to sleep:
  switch (platform) {
  case OSX:
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_F12); 
    Keyboard.releaseAll();
    break;
  case WIN7:
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_RIGHT_ARROW);
    Keyboard.press(KEY_RETURN); 
    Keyboard.releaseAll();
    break;
  case WINXP:
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_DELETE); 
    delay(1000);
    Keyboard.type('S');
    Keyboard.type('H');
    break;
  case UBUNTU:
    // ubuntu keystrokes?

    break;
  }
  // wait ten seconds, then wake the computer back up:
  delay(10000);
  Keyboard.type(' ');

  // wait forever:
  while(true);
}



