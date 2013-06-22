
/*
 Running shell coommands using Process class. 
 
 This sketch demonstrate how to run linux shell commands
 using an Arduino Yún. 

 The circuit:
 * Arduino Yun
 
 created 12 Jun 2013
 by Cristian Maglie
 modified 21 June 2013
 by Tom Igoe
 
 This example code is in the public domain.

 */

#include <Process.h>

void setup() {
  // initialize the Bridge and Serial connections:
  Bridge.begin();
  Serial.begin(9600);
}

void loop() {
  Process p;
  // This command line prints the name of the wireless network
  // that the board is connected to, or the network which the board has created:
  p.runShellCommand(F("lua /usr/lib/lua/pretty_wifi_info.lua | grep SSID"));

  // Read command output
  while (p.available()) {
    char c = p.read();
    Serial.print(c);
  } 
  while (true);    // do nothing more
}

