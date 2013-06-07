/*
  Write to file using FileIO classes.
 
 This sketch demonstrate how to write file into the Yún filesystem.
 A shell script file is created in /tmp, and it is executed afterwards.
 
 */
 
#include <FileIO.h>

void setup() {
  // Setup Bridge (needed every time we communicate with the Arduino Yún)
  Bridge.begin();

  // Setup Console
  Console.begin();
  // Buffering improves Console performance, but we must remember to
  // finish sending using the Console.flush() command.
  Console.buffer(64);
  
  // Setup File IO
  SD.begin();

  // Upload script used to gain network statistics  
  uploadScript();
}  

void loop() {
  // Run stats script every 5 secs.
  runScript();
  delay(5000);
}

void uploadScript() {
  // Write our shell script in /tmp
  // Using /tmp stores the script in RAM this way we can preserve 
  // the limited amount of FLASH erase/write cycles
  File script = SD.open("/tmp/wlan-stats.sh", FILE_WRITE);
  script.print("#!/bin/sh\n");
  script.print("ifconfig wlan0 | grep \"RX bytes\" | tr ':' ' ' | awk \"{ print \\$3 \\\" \\\" \\$8 }\"\n");
  script.close();
  
  // Make the script executable
  Process chmod;
  chmod.begin("chmod");
  chmod.addParameter("+x");
  chmod.addParameter("/tmp/wlan-stats.sh");
  chmod.run();
}

void runScript() {
  // Launch script and show results on the console
  Process myscript;
  myscript.begin("/tmp/wlan-stats.sh");
  myscript.run();
  
  Console.print("WiFi RX/TX bytes: ");
  while (myscript.available()) {
    char c = myscript.read();
    Console.print(c);
  }
  Console.println();
  Console.flush();
}

