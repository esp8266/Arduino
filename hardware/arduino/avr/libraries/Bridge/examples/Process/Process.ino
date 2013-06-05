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
  Process p;
  p.begin("curl");
  p.addParameter("http://arduino.cc/asciilogo.txt");
  p.run();

  // Print arduino logo over the console
  while (p.available()>0) {
    char c = p.read();
    Console.print(c);
  }
  Console.flush();
}

void runCpuInfo() {
  // Launch "cat /proc/cpuinfo" command (shows info on Atheros CPU)
  Process p;
  p.begin("cat");
  p.addParameter("/proc/cpuinfo");
  p.run();

  // Print command output on the Console
  while (p.available()>0) {
    char c = p.read();
    Console.print(c);
  }
  Console.flush();
}

