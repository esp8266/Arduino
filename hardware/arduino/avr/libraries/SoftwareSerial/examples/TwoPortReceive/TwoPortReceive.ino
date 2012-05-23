/*
  Software serial multple serial test
 
 Receives from the two software serial ports, 
 sends to the hardware serial port. 
 
 In order to listen on a software port, you call port.listen(). 
 When using two software serial ports, you have to switch ports
 by listen()ing on each one in turn. Pick a logical time to switch
 ports, like the end of an expected transmission, or when the 
 buffer is empty. This example switches ports when there is nothing
 more to read from a port
 
 The circuit: 
 Two devices which communicate serially are needed.
 * First serial device's TX attached to digital pin 2, RX to pin 3
 * Second serial device's TX attached to digital pin 4, RX to pin 5
 
 created 18 Apr. 2011
 modified 9 Apr 2012
 by Tom Igoe
 based on Mikal Hart's twoPortRXExample
 
 This example code is in the public domain.
 
 */

#include <SoftwareSerial.h>
// software serial #1: TX = digital pin 2, RX = digital pin 3
SoftwareSerial portOne(2, 3);

// software serial #2: TX = digital pin 4, RX = digital pin 5
SoftwareSerial portTwo(4, 5);

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // Start each software serial port
  portOne.begin(9600);
  portTwo.begin(9600);
}

void loop()
{
  // By default, the last intialized port is listening.
  // when you want to listen on a port, explicitly select it:
  portOne.listen();
  Serial.println("Data from port one:");
  // while there is data coming in, read it
  // and send to the hardware serial port:
  while (portOne.available() > 0) {
    char inByte = portOne.read();
    Serial.write(inByte);
  }

  // blank line to separate data from the two ports:
  Serial.println();

  // Now listen on the second port
  portTwo.listen();
  // while there is data coming in, read it
  // and send to the hardware serial port:
  Serial.println("Data from port two:");
  while (portTwo.available() > 0) {
    char inByte = portTwo.read();
    Serial.write(inByte);
  }

  // blank line to separate data from the two ports:
  Serial.println();
}






