#include <Process.h>

void brk() {
  Bridge.print((char)3);
  Bridge.find("#");
}

void setup() {
  Bridge.begin();
  
  Process p;
  p.begin("curl");
  p.addParameter("http://arduino.cc/asciilogo.txt");
  p.run();
  
  String res = "";
  while (p.IO.available()>0) {
    p.IO.read();
  }
}

void loop() {
}

