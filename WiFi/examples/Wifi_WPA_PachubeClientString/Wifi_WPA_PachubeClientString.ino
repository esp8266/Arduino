/*
  Pachube sensor client with Strings
 
 This sketch connects an analog sensor to Pachube (http://www.pachube.com)
 using an Arduino WiFi shield.
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 Circuit:
 * Analog sensors attached to analog in 0 and 1
 * WoFo shield attached to pins 10, 11, 12, 13
 
 created 15 March 2010
 updated 5 June 2011
 by Tom Igoe
 
 This code is in the public domain.
 
 */


#include <SPI.h>
#include <WiFi.h>

char ssid[] = "yourNetwork";
char pass[] = "secretPassword";
int status = WL_IDLE_STATUS;

//  The address of the server you want to connect to (pachube.com):
IPAddress server(173,203,98,29);

// initialize the library instance:
Client client;

long lastConnectionTime = 0;        // last time you connected to the server, in milliseconds
boolean lastConnected = false;      // state of the connection last time through the main loop
const int postingInterval = 30000;  //delay between updates to Pachube.com

int startWiFiWpa()
{
  Serial.println("\nSetup WiFi Wpa...");
  //strcpy(ssid, "AndroidAP9647");
  strcpy(ssid, "tigoenet");
  Serial.print("SSID: ");
  Serial.println(ssid);
  const char *pass = "m30w-m30w";
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED)
  {
    Serial.println("Connection Failed");
  }
  return status;
}



void setup() {
  // start the WiFi connection and the server:
  Serial.begin(9600);
  Serial.println("*** Start WebClient WiFi example ***");

  int _status = startWiFiWpa();
  if ( _status == WL_CONNECTED)
  {
    Serial.println("\nWiFi Connected.");
  }

  delay(1000);
}


void loop() {
  // read the analog sensor:
  int sensorReading = analogRead(A0);   
  // convert the data to a String to send it:
  String dataString = String(sensorReading);

  // you can append multiple readings to this String if your
  // pachube feed is set up to handle multiple values:
  int otherSensorReading = analogRead(A1);
  dataString += ",";
  dataString += String(otherSensorReading);

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
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
    // send the HTTP PUT request. 
    // fill in your feed address here:
    client.print("PUT /api/YOUR_FEED_HERE.csv HTTP/1.1\n");
    client.print("Host: www.pachube.com\n");
    // fill in your Pachube API key here:
    client.print("X-PachubeApiKey: YOUR_KEY_HERE\n");
    client.print("Content-Length: ");
    client.println(thisData.length(), DEC);

    // last pieces of the HTTP PUT request:
    client.print("Content-Type: text/csv\n");
    client.println("Connection: close\n");

    // here's the actual content of the PUT request:
    client.println(thisData);

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

