#include <Process.h>

void setup() {
  Serial.begin(9600);  // initialize serial communication
  while(!Serial);      // do nothing until the serial monitor is opened
 
  Serial.println("Starting bridge...\n");
  pinMode(13,OUTPUT);  
  digitalWrite(13, LOW);  
  Bridge.begin();      // make contact with the linux processor
  digitalWrite(13, HIGH); 

  delay(2000);         // wait 2 seconds
}

void loop() {
  Process wifiCheck;    // initialize a new process

  wifiCheck.runShellCommand("lua /arduino/pretty_wifi_info.lua");        // command you want to run

  // while there's any characters coming back from the 
  // process, print them to the serial monitor:
  while (wifiCheck.available() > 0) {
    char thisChar = wifiCheck.read();
    Serial.print(thisChar);
  }
  
  Serial.println();
  
  delay(5000);
}

