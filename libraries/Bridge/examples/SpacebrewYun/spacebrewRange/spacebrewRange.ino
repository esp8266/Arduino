/*
  Spacebrew Range

 Demonstrates how to create a sketch that sends and receives analog
 range value to and from Spacebrew. Every time the state of the
 potentiometer (or other analog input component) change a spacebrew
 message is sent. The sketch also accepts analog range messages from
 other Spacebrew apps.

 Make sure that your Yun is connected to the internet for this example
 to function properly.

 The circuit:
 - Potentiometer connected to Yun. Middle pin connected to analog pin A0,
   other pins connected to 5v and GND pins.

 created 2013
 by Julio Terra

 This example code is in the public domain.

 More information about Spacebrew is available at:
 http://spacebrew.cc/

 */

#include <Bridge.h>
#include <SpacebrewYun.h>

// create a variable of type SpacebrewYun and initialize it with the constructor
SpacebrewYun sb = SpacebrewYun("spacebrewYun Range", "Range sender and receiver");

// variable that holds the last potentiometer value
int last_value = 0;

// create variables to manage interval between each time we send a string
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
  sb.addPublish("physical pot", "range");
  sb.addSubscribe("virtual pot", "range");

  // register the string message handler method
  sb.onRangeMessage(handleRange);

  // connect to cloud spacebrew server at "sandbox.spacebrew.cc"
  sb.connect("sandbox.spacebrew.cc");
}


void loop() {
  // monitor spacebrew connection for new data
  sb.monitor();

  // connected to spacebrew then send a new value whenever the pot value changes
  if ( sb.connected() ) {
    int cur_value = analogRead(A0);
    if ( last_value != cur_value ) {
      sb.send("physical pot", cur_value);
      last_value = cur_value;
    }
  }
}

// handler method that is called whenever a new string message is received
void handleRange (String route, int value) {
  // print the message that was received
  Serial.print("From ");
  Serial.print(route);
  Serial.print(", received msg: ");
  Serial.println(value);
}

