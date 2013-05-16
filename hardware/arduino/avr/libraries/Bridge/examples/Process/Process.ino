#include <Process.h>

void setup() {
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);

  Serial.begin(9600);
  Bridge.begin();

  digitalWrite(13,HIGH);
  delay(2000);
  
  Process p;
  p.begin("curl");
  p.addParameter("http://arduino.cc/asciilogo.txt");
  p.run();
  
  while (p.available()>0) {
    char c = p.read();
    Serial.print(c);
  }
}

void loop() {
}
