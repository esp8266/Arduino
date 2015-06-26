/*
  WiFi Status

 This sketch runs a script called "pretty-wifi-info.lua"
 installed on your Yún in folder /usr/bin.
 It prints information about the status of your wifi connection.

 It uses Serial to print, so you need to connect your Yún to your
 computer using a USB cable and select the appropriate port from
 the Port menu

 created  18 June 2013
 By Federico Fissore

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/YunWiFiStatus

 */

#include <Process.h>

void setup() {
  Serial.begin(9600);  // initialize serial communication
  while (!Serial);     // do nothing until the serial monitor is opened

  Serial.println("Starting bridge...\n");
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();  // make contact with the linux processor
  digitalWrite(13, HIGH);  // Led on pin 13 turns on when the bridge is ready

  delay(2000);  // wait 2 seconds
}

void loop() {
  Process wifiCheck;  // initialize a new process

  wifiCheck.runShellCommand("/usr/bin/pretty-wifi-info.lua");  // command you want to run

  // while there's any characters coming back from the
  // process, print them to the serial monitor:
  while (wifiCheck.available() > 0) {
    char c = wifiCheck.read();
    Serial.print(c);
  }

  Serial.println();

  delay(5000);
}

