/*
  Spacebrew String

 Demonstrates how to create a sketch that sends and receives strings
 to and from Spacebrew. Every time string data is received it
 is output to the Serial monitor.

 Make sure that your Yun is connected to the internet for this example
 to function properly.

 The circuit:
 - No circuit required

 created 2013
 by Julio Terra

 This example code is in the public domain.

 More information about Spacebrew is available at:
 http://spacebrew.cc/

 */

#include <Bridge.h>
#include <SpacebrewYun.h>

// create a variable of type SpacebrewYun and initialize it with the constructor
SpacebrewYun sb = SpacebrewYun("spacebrewYun Strings", "String sender and receiver");

// create variables to manage interval between each time we send a string
long last_time = 0;
int interval = 2000;

void setup() {

  // start the serial port
  Serial.begin(57600);

  // for debugging, wait until a serial console is connected
  delay(4000);
  while (!Serial) {
    ;
  }

  // start-up the bridge
  Bridge.begin();

  // configure the spacebrew object to print status messages to serial
  sb.verbose(true);

  // configure the spacebrew publisher and subscriber
  sb.addPublish("speak", "string");
  sb.addSubscribe("listen", "string");

  // register the string message handler method
  sb.onStringMessage(handleString);

  // connect to cloud spacebrew server at "sandbox.spacebrew.cc"
  sb.connect("sandbox.spacebrew.cc");
}


void loop() {
  // monitor spacebrew connection for new data
  sb.monitor();

  // connected to spacebrew then send a string every 2 seconds
  if ( sb.connected() ) {

    // check if it is time to send a new message
    if ( (millis() - last_time) > interval ) {
      sb.send("speak", "is anybody out there?");
      last_time = millis();
    }
  }
}

// handler method that is called whenever a new string message is received
void handleString (String route, String value) {
  // print the message that was received
  Serial.print("From ");
  Serial.print(route);
  Serial.print(", received msg: ");
  Serial.println(value);
}

