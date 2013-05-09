
#include <Bridge.h>

void brk() {
  Bridge.print((char)3);
}

void setup() {
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH); 

  delay(2000);
  int handle = Bridge.beginCommand("curl");
  Bridge.commandAddEscapedParam("http://arduino.cc/asciilogo.txt");
  Bridge.endCommand();

  while (Bridge.commandIsRunning(handle))
    delay(250);
  
  int size = Bridge.commandOutputSize(handle);
  char buff[20];
  Bridge.readCommandOutput(handle, 0, size, buff);
  buff[size]=0;
  Bridge.print(buff);brk();
}

void loop() {
}


