#include <SoftwareSerial.h>

SoftwareSerial ss(2, 3);
SoftwareSerial ss2(4, 5);

/* This sample shows how to correctly process received data
   on two different "soft" serial ports.  Here we listen on 
   the first port (ss) until we receive a '?' character.  Then
   we begin listening on the other soft port.
*/

void setup()
{
  // Start the HW serial port
  Serial.begin(57600);

  // Start each soft serial port
  ss.begin(4800);
  ss2.begin(4800);

  // By default, the most recently "begun" port is listening.
  // We want to listen on ss, so let's explicitly select it.
  ss.listen();
  
  // Simply wait for a ? character to come down the pipe
  Serial.println("Data from the first port: ");
  char c = 0;
  do
    if (ss.available())
    {
      c = (char)ss.read();
      Serial.print(c);
    }
  while (c != '?');

  // Now listen on the second port
  ss2.listen();

  Serial.println("Data from the second port: ");
}

void loop()
{
  if (ss2.available())
  {
    char c = (char)ss2.read();
    Serial.print(c);
  }
}
  
