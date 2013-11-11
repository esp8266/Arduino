/*
 Xively client with Strings
 
 This sketch connects two analog sensors to Xively (http://www.xively.com)
 through a Telefonica GSM/GPRS shield.
 
 This example has been updated to use version 2.0 of the Xively.com API. 
 To make it work, create a feed with two datastreams, and give them the IDs
 sensor1 and sensor2. Or change the code below to match your feed.

 This example uses the String library, which is part of the Arduino core from
 version 0019.

 Circuit:
 * Analog sensors attached to A0 and A1
 * GSM shield attached to an Arduino
 * SIM card with a data plan

 created 8 March 2012
 by Tom Igoe
 and adapted for GSM shield by David Del Peral

 This code is in the public domain.

 */

// Include the GSM library
#include <GSM.h>

// Xively login information
#define APIKEY         "YOUR API KEY GOES HERE"  // replace your xively api key here
#define FEEDID         00000                     // replace your feed ID
#define USERAGENT      "My Project"              // user agent is the project name

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "GPRS_APN" // replace your GPRS APN
#define GPRS_LOGIN     "login"    // replace with your GPRS login
#define GPRS_PASSWORD  "password" // replace with your GPRS password

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// IPAddress server(216,52,233,121);     // numeric IP for api.xively.com
char server[] = "api.xively.com";       // name address for Xively API

unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  // delay between updates to Xively.com

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (notConnected)
  {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("Connected to GPRS network");
}

void loop()
{
  // read the sensor on A0
  int sensorReading = analogRead(A0);

  // convert the data to a String
  String dataString = "sensor1,";
  dataString += sensorReading;

  // you can append multiple readings to this String to 
  // send the xively feed multiple values
  int otherSensorReading = analogRead(A1);
  dataString += "\nsensor2,";
  dataString += otherSensorReading;

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client
  if (!client.connected() && lastConnected)
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data
  if (!client.connected() && (millis() - lastConnectionTime > postingInterval))
  {
    sendData(dataString);
  }
  // store the state of the connection for next time through
  // the loop
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server
void sendData(String thisData)
{
  // if there's a successful connection:
  if (client.connect(server, 80))
  {
    Serial.println("connecting...");

    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.xively.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");
    client.println(thisData.length());

    // last pieces of the HTTP PUT request
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request
    client.println(thisData);
  }
  else
  {
    // if you couldn't make a connection
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}
