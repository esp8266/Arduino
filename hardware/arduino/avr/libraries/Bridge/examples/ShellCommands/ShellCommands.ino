
/* Demonstrate shell commands */

#include <Process.h>

void setup() {
  Bridge.begin();
  Console.begin();
  Console.buffer(64);
}

void loop() {
  Process p;
  // This command line prints the number of bytes received and transmitted from WLAN
  p.runShellCommand(F("ifconfig wlan0 | grep \"RX bytes\" | tr ':' ' ' | awk \"{ print \\$3 \\\" \\\" \\$8 }\"\n"));

  // Read command output
  while (p.available()) {
    char c = p.read();
    Console.print(c);
  }
  Console.flush();
  
  delay(5000);
}

