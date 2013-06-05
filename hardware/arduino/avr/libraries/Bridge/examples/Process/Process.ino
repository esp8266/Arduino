/*
  Running process using Process class. 
 
 This sketch demonstrate how to run linux processes
 using an Arduino Yún. 
 
 created 5 Jun 2013
 by Cristian Maglie
 
 */

#include <Process.h>

void setup() {
  // Setup Bridge (needed every time we communicate with the Arduino Yún)
  Bridge.begin();
  
  // Setup Console
  Console.begin();
  // Buffering improves Console performance, but we must remember to
  // finish sending using the Console.flush() command.
  Console.buffer(64);
  
  // Wait until a Network Monitor is connected.
  while (!Console);

  // run various example processes
  runCurl();
  runCpuInfo();
}

void loop() {
  // Do nothing here.
}

void runCurl() {
  // Launch "curl" command and get Arduino asciilogo from the network
  
  Process p;                 // Create a process and call it "p"
  p.begin("curl");           // Process should launch the "curl" command
  p.addParameter("http://arduino.cc/asciilogo.txt"); // Add the URL parameter to "curl"
  p.run();                   // Run the process and wait for its termination

  // Print arduino logo over the console.
  // A process output can be read with the stream methods
  while (p.available()>0) {
    char c = p.read();
    Console.print(c);
  }
  // Ensure the latest bit of data is sent.
  Console.flush();
}

void runCpuInfo() {
  // Launch "cat /proc/cpuinfo" command (shows info on Atheros CPU)
  Process p;
  p.begin("cat");
  p.addParameter("/proc/cpuinfo");
  p.run();

  // Print command output on the Console.
  // A process output can be read with the stream methods
  while (p.available()>0) {
    char c = p.read();
    Console.print(c);
  }
  // Ensure the latest bit of data is sent.
  Console.flush();
}

