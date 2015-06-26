/*
  Write to file using FileIO classes.

 This sketch demonstrate how to write file into the Yún filesystem.
 A shell script file is created in /tmp, and it is executed afterwards.

 created 7 June 2010
 by Cristian Maglie

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/FileWriteScript

 */

#include <FileIO.h>

void setup() {
  // Setup Bridge (needed every time we communicate with the Arduino Yún)
  Bridge.begin();
  // Initialize the Serial
  Serial.begin(9600);

  while (!Serial); // wait for Serial port to connect.
  Serial.println("File Write Script example\n\n");

  // Setup File IO
  FileSystem.begin();

  // Upload script used to gain network statistics
  uploadScript();
}

void loop() {
  // Run stats script every 5 secs.
  runScript();
  delay(5000);
}

// this function creates a file into the linux processor that contains a shell script
// to check the network traffic of the WiFi interface
void uploadScript() {
  // Write our shell script in /tmp
  // Using /tmp stores the script in RAM this way we can preserve
  // the limited amount of FLASH erase/write cycles
  File script = FileSystem.open("/tmp/wlan-stats.sh", FILE_WRITE);
  // Shell script header
  script.print("#!/bin/sh\n");
  // shell commands:
  // ifconfig: is a command line utility for controlling the network interfaces.
  //           wlan0 is the interface we want to query
  // grep: search inside the output of the ifconfig command the "RX bytes" keyword
  //       and extract the line that contains it
  script.print("ifconfig wlan0 | grep 'RX bytes'\n");
  script.close();  // close the file

  // Make the script executable
  Process chmod;
  chmod.begin("chmod");      // chmod: change mode
  chmod.addParameter("+x");  // x stays for executable
  chmod.addParameter("/tmp/wlan-stats.sh");  // path to the file to make it executable
  chmod.run();
}


// this function run the script and read the output data
void runScript() {
  // Run the script and show results on the Serial
  Process myscript;
  myscript.begin("/tmp/wlan-stats.sh");
  myscript.run();

  String output = "";

  // read the output of the script
  while (myscript.available()) {
    output += (char)myscript.read();
  }
  // remove the blank spaces at the beginning and the ending of the string
  output.trim();
  Serial.println(output);
  Serial.flush();
}

