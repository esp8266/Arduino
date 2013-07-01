
/*
  WiFi Status 
 
 This sketch run a script already present on your Yún in the
 /arduino directory called "pretty_wifi_info.lua" that takes 
 the informations of the WiFi interface and print it on the
 Serial monitor.
 
 created  18 June 2013
 By Federico Fissore
 
 This example code is in the public domain.
 
 */

#include <Process.h>

void setup() {
  Serial.begin(9600);  // initialize serial communication
  while(!Serial);      // do nothing until the serial monitor is opened
 
  Serial.println("Starting bridge...\n");
  pinMode(13,OUTPUT);  
  digitalWrite(13, LOW);  
  Bridge.begin();  // make contact with the linux processor
  digitalWrite(13, HIGH);  // Led on pin 13 turns on when the bridge is ready

  delay(2000);  // wait 2 seconds
}

void loop() {
  Process wifiCheck;  // initialize a new process

  wifiCheck.runShellCommand("lua /arduino/pretty_wifi_info.lua");  // command you want to run

  // while there's any characters coming back from the 
  // process, print them to the serial monitor:
  while (wifiCheck.available() > 0) {
    char c = wifiCheck.read();
    Serial.print(c);
  }
  
  Serial.println();
  
  delay(5000);
}

