/*
  Running shell commands using Process class.

 This sketch demonstrate how to run linux shell commands
 using an Arduino Yún. It runs the wifiCheck script on the Linux side
 of the Yún, then uses grep to get just the signal strength line.
 Then it uses parseInt() to read the wifi signal strength as an integer,
 and finally uses that number to fade an LED using analogWrite().

 The circuit:
 * Arduino Yún with LED connected to pin 9

 created 12 Jun 2013
 by Cristian Maglie
 modified 25 June 2013
 by Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/ShellCommands

 */

#include <Process.h>

void setup() {
  Bridge.begin();	// Initialize the Bridge
  Serial.begin(9600);	// Initialize the Serial

  // Wait until a Serial Monitor is connected.
  while (!Serial);
}

void loop() {
  Process p;
  // This command line runs the WifiStatus script, (/usr/bin/pretty-wifi-info.lua), then
  // sends the result to the grep command to look for a line containing the word
  // "Signal:"  the result is passed to this sketch:
  p.runShellCommand("/usr/bin/pretty-wifi-info.lua | grep Signal");

  // do nothing until the process finishes, so you get the whole output:
  while (p.running());

  // Read command output. runShellCommand() should have passed "Signal: xx&":
  while (p.available()) {
    int result = p.parseInt();			// look for an integer
    int signal = map(result, 0, 100, 0, 255);	// map result from 0-100 range to 0-255
    analogWrite(9, signal);			// set the brightness of LED on pin 9
    Serial.println(result);			// print the number as well
  }
  delay(5000);  // wait 5 seconds before you do it again
}



