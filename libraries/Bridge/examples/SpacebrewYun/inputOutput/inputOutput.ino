/*
  Input Output

 Demonstrates how to create a sketch that sends and receives all standard
 spacebrew data types, and a custom data type. Every time data is
 received it is output to the Serial monitor.

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
SpacebrewYun sb = SpacebrewYun("aYun", "Arduino Yun spacebrew test");

// create variables to manage interval between each time we send a string
long last = 0;
int interval = 2000;

int counter = 0;

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
  sb.addPublish("string test", "string");
  sb.addPublish("range test", "range");
  sb.addPublish("boolean test", "boolean");
  sb.addPublish("custom test", "crazy");
  sb.addSubscribe("string test", "string");
  sb.addSubscribe("range test", "range");
  sb.addSubscribe("boolean test", "boolean");
  sb.addSubscribe("custom test", "crazy");

  // register the string message handler method
  sb.onRangeMessage(handleRange);
  sb.onStringMessage(handleString);
  sb.onBooleanMessage(handleBoolean);
  sb.onCustomMessage(handleCustom);

  // connect to cloud spacebrew server at "sandbox.spacebrew.cc"
  sb.connect("sandbox.spacebrew.cc");

}


void loop() {
  // monitor spacebrew connection for new data
  sb.monitor();

  // connected to spacebrew then send a string every 2 seconds
  if ( sb.connected() ) {

    // check if it is time to send a new message
    if ( (millis() - last) > interval ) {
      String test_str_msg = "testing, testing, ";
      test_str_msg += counter;
      counter ++;

      sb.send("string test", test_str_msg);
      sb.send("range test", 500);
      sb.send("boolean test", true);
      sb.send("custom test", "youre loco");

      last = millis();

    }
  }
}

// define handler methods, all standard data type handlers take two appropriate arguments

void handleRange (String route, int value) {
  Serial.print("Range msg ");
  Serial.print(route);
  Serial.print(", value ");
  Serial.println(value);
}

void handleString (String route, String value) {
  Serial.print("String msg ");
  Serial.print(route);
  Serial.print(", value ");
  Serial.println(value);
}

void handleBoolean (String route, boolean value) {
  Serial.print("Boolen msg ");
  Serial.print(route);
  Serial.print(", value ");
  Serial.println(value ? "true" : "false");
}

// custom data type handlers takes three String arguments

void handleCustom (String route, String value, String type) {
  Serial.print("Custom msg ");
  Serial.print(route);
  Serial.print(" of type ");
  Serial.print(type);
  Serial.print(", value ");
  Serial.println(value);
}

