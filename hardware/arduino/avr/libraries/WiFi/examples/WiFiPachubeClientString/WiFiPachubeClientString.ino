/*
  Wifi Pachube sensor client with Strings
 
 This sketch connects an analog sensor to Pachube (http://www.pachube.com)
 using a Arduino Wifi shield.
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 This example has been updated to use version 2.0 of the pachube.com API. 
 To make it work, create a feed with a datastream, and give it the ID
 sensor1. Or change the code below to match your feed.
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 Circuit:
 * Analog sensor attached to analog in 0
 * Wifi shield attached to pins 10, 11, 12, 13
 
 created 16 Mar 2012
 modified 31 May 2012
 by Tom Igoe
 modified 8 Sept 2012
 by Scott Fitzgerald
 
 This code is in the public domain.
 
 */

#include <SPI.h>
#include <WiFi.h>

#define APIKEY         "YOUR API KEY GOES HERE" // replace your pachube api key here
#define FEEDID         00000                    // replace your feed ID
#define USERAGENT      "My Arduino Project"     // user agent is the project name

char ssid[] = "yourNetwork";      //  your network SSID (name) 
char pass[] = "secretPassword";   // your network password

int status = WL_IDLE_STATUS;

// initialize the library instance:
WiFiClient client;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(216,52,233,121);      // numeric IP for api.pachube.com
char server[] = "api.pachube.com";   // name address for pachube API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  //delay between updates to pachube.com

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 
  
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  } 
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
  // read the analog sensor:
  int sensorReading = analogRead(A0);   
  // convert the data to a String to send it:

  String dataString = "sensor1,";
  dataString += sensorReading;

  // you can append multiple readings to this String if your
  // pachube feed is set up to handle multiple values:
  int otherSensorReading = analogRead(A1);
  dataString += "\nsensor2,";
  dataString += otherSensorReading;

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data: 
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    sendData(dataString);
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void sendData(String thisData) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.pachube.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");
    client.println(thisData.length());

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.println(thisData);
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


