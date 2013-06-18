/*
  Arduino Yun Wireless Config Check
 
 Checks the wireless state of Arduino Yun by calling
 the linux command iwconfig.
 
 Upload this to an Arduino Yun via serial (not WiFi) 
 then open the serial monitor to see the status of
 your Yun's WiFi connection. If it's connected to 
 a wireless network, the ESSID (name) of that network
 and the signal strength will appear.
 
 The circuit:
 * Arduino Yun
 
 created 22 May 2013
 by Tom Igoe
 
 This example code is in the public domain.
 */

#include <Process.h>

void setup() {
  Serial.begin(9600);  // initialize serial communication
  while(!Serial);      // do nothing until the serial monitor is opened
 
  pinMode(13,OUTPUT);  
  digitalWrite(13, LOW);  
  Bridge.begin();      // make contact with the linux processor
  digitalWrite(13, HIGH); 

  delay(2000);         // wait 2 seconds

  Process wifiCheck;    // initialize a new process


  wifiCheck.begin("iwconfig");        // command you want to run
  wifiCheck.addParameter("wlan0");    // parameter of the command
  wifiCheck.run();                    // run the command

  // while there's any characters coming back from the 
  // process, print them to the serial monitor:
  while (wifiCheck.available() > 0) {
    char thisChar = wifiCheck.read();
    Serial.print(thisChar);
  }
}

void loop() {
  // nothing to do here.
}

