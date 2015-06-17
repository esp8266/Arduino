/*
  Running process using Process class.

 This sketch demonstrate how to run linux processes
 using an Arduino Yún.

 created 5 Jun 2013
 by Cristian Maglie

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Process

 */

#include <Process.h>

void setup() {
  // Initialize Bridge
  Bridge.begin();

  // Initialize Serial
  Serial.begin(9600);

  // Wait until a Serial Monitor is connected.
  while (!Serial);

  // run various example processes
  runCurl();
  runCpuInfo();
}

void loop() {
  // Do nothing here.
}

void runCurl() {
  // Launch "curl" command and get Arduino ascii art logo from the network
  // curl is command line program for transferring data using different internet protocols
  Process p;		// Create a process and call it "p"
  p.begin("curl");	// Process that launch the "curl" command
  p.addParameter("http://www.arduino.cc/asciilogo.txt"); // Add the URL parameter to "curl"
  p.run();		// Run the process and wait for its termination

  // Print arduino logo over the Serial
  // A process output can be read with the stream methods
  while (p.available() > 0) {
    char c = p.read();
    Serial.print(c);
  }
  // Ensure the last bit of data is sent.
  Serial.flush();
}

void runCpuInfo() {
  // Launch "cat /proc/cpuinfo" command (shows info on Atheros CPU)
  // cat is a command line utility that shows the content of a file
  Process p;		// Create a process and call it "p"
  p.begin("cat");	// Process that launch the "cat" command
  p.addParameter("/proc/cpuinfo"); // Add the cpuifo file path as parameter to cut
  p.run();		// Run the process and wait for its termination

  // Print command output on the Serial.
  // A process output can be read with the stream methods
  while (p.available() > 0) {
    char c = p.read();
    Serial.print(c);
  }
  // Ensure the last bit of data is sent.
  Serial.flush();
}

