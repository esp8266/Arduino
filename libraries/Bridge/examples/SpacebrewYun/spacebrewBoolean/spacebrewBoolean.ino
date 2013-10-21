/*
  Spacebrew Boolean

 Demonstrates how to create a sketch that sends and receives a
 boolean value to and from Spacebrew. Every time the buttton is
 pressed (or other digital input component) a spacebrew message
 is sent. The sketch also accepts analog range messages from
 other Spacebrew apps.

 Make sure that your Yun is connected to the internet for this example
 to function properly.

 The circuit:
 - Button connected to Yun, using the Arduino's internal pullup resistor.

 created 2013
 by Julio Terra

 This example code is in the public domain.

 More information about Spacebrew is available at:
 http://spacebrew.cc/

 */

#include <Bridge.h>
#include <SpacebrewYun.h>

// create a variable of type SpacebrewYun and initialize it with the constructor
SpacebrewYun sb = SpacebrewYun("spacebrewYun Boolean", "Boolean sender and receiver");

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
  sb.addPublish("physical button", "boolean");
  sb.addSubscribe("virtual button", "boolean");

  // register the string message handler method
  sb.onBooleanMessage(handleBoolean);

  // connect to cloud spacebrew server at "sandbox.spacebrew.cc"
  sb.connect("sandbox.spacebrew.cc");

  pinMode(3, INPUT);
  digitalWrite(3, HIGH);
}


void loop() {
  // monitor spacebrew connection for new data
  sb.monitor();

  // connected to spacebrew then send a new value whenever the pot value changes
  if ( sb.connected() ) {
    int cur_value = digitalRead(3);
    if ( last_value != cur_value ) {
      if (cur_value == HIGH) sb.send("physical button", false);
      else sb.send("physical button", true);
      last_value = cur_value;
    }
  }
}

// handler method that is called whenever a new string message is received
void handleBoolean (String route, boolean value) {
  // print the message that was received
  Serial.print("From ");
  Serial.print(route);
  Serial.print(", received msg: ");
  Serial.println(value ? "true" : "false");
}

