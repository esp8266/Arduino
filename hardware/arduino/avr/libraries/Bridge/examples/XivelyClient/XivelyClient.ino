
/*
  Xively sensor client with Strings 
 
 This sketch connects an analog sensor to Xively,
 using an Arduino Yun. 
 
 created  15 March 2010
 updated 27 May 2013
 by Tom Igoe
 
 NOT CURRENTLY WORKING as of Arduino 1.5.3 27 May 2013
 
 */

// include all Libraries needed:
#include <Process.h>
#include "passwords.h"      // contains my passwords, see below

/*
  NOTE: passwords.h is not included with this repo because it contains my passwords.
 You need to create it for your own version of this application.  To do so, make
 a new tab in Arduino, call it passwords.h, and include the following variables and constants:
 
 #define APIKEY        "foo"                  // replace your pachube api key here
 #define FEEDID        0000                   // replace your feed ID
 #define USERAGENT     "my-project"           // user agent is the project name
  */


// set up net client info:
boolean lastConnected = false;      // state of the connection last time through the main loop
const unsigned long postingInterval = 60000;  //delay between updates to Pachube.com
unsigned long lastRequest = 0;      // when you last made a request
String dataString = "";
Process xively;

void setup() {
  // start serial port:
  Serial.begin(9600);
  Bridge.begin();
  delay(2000);

  while(!Serial);    // wait for Serial Monitor to open
  Serial.println("Xively client");

  // reserve space for dataString:
  dataString.reserve(100);
  sendData();
}

void loop() {
  // get a timestamp so you can calculate reading and sending intervals:
  long now = millis();

  // convert the readings to a String to send it:
  dataString = "temp,";
  dataString += random(10) + 20;
  // add pressure:
  dataString += "\nPressure,";
  dataString += random(5) + 100;

  // if there's incoming data from the net connection,
  // send it out the serial port:
  if (xively.available()>0) {
    char c = xively.read();
    Serial.write(c);
  }


  // if you're not connected, and the sending interval has passed since
  // your last connection, then connect again and send data:
  if(now - lastRequest >= postingInterval) {
    sendData();
    lastRequest = now;
  }
}

// this method makes a HTTP connection to the server:
void sendData() {
  Serial.println("Sending data");
  dataString = "--data \'" + dataString;
  dataString += "\'";

  // form the string for the API header parameter:
  String apiString =  "--header \"X-ApiKey: ";
  apiString += APIKEY;
  apiString += "\"";

  // form the string for the URL parameter:
  String url =  " \"https://api.xively.com/v2/feeds/";
  url += FEEDID;
  url += ".csv\"";
 
  // send the HTTP PUT request:
  xively.begin("curl");
  xively.addParameter("-k");
  xively.addParameter("--request PUT");
  xively.addParameter(dataString);
  xively.addParameter(apiString); 
  xively.addParameter(url);
  xively.run();
}









