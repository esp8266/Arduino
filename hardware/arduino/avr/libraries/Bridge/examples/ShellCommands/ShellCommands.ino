
/* Demonstrate shell commands */

#include <Process.h>

void setup() {
  Bridge.begin();
  Console.begin();
  Console.buffer(64);
}

void loop() {
  Process p;
  // This command line prints the name of the wireless that the board is connected to or that the board has created
  p.runShellCommand(F("lua /usr/lib/lua/pretty_wifi_info.lua | grep SSID"));

  // Read command output
  while (p.available()) {
    char c = p.read();
    Console.print(c);
  }
  Console.flush();
  
  delay(5000);
}

