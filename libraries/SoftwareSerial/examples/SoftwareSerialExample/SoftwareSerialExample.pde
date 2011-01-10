#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

void setup()  
{
  Serial.begin(57600);
  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("Hello, world?");
}

void loop() // run over and over
{
  if (mySerial.available())
    Serial.print((char)mySerial.read());
  if (Serial.available())
    mySerial.print((char)Serial.read());
}
