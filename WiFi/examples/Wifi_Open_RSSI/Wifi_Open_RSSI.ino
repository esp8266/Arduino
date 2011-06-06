/*
 
 Open connection using the WiFi shield. Attempts to connect
 and prints out the signal strength.
 
 Circuit:
 * WiFi shield attached
 created 5 June 2011
 by Tom Igoe
 */
#include <SPI.h>
#include <WiFi.h>

char ssid[] = "yourNetwork";     // the name of your network
int status = WL_IDLE_STATUS;     // the Wifi radio's status


byte mac[6];                     // the MAC address of your Wifi shield
IPAddress ip;                    // the IP address of your shield

void setup() {
  // initialize serial:
  Serial.begin(9600);

  // attempt to connect using WEP encryption:
  Serial.println("Attempting to connect to open network...");
  status = WiFi.begin(ssid);

  Serial.print("SSID: ");
  Serial.println(ssid);

  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
}

void loop() {
  // if you're connected, print out the signal strength:
  if ( status != WL_CONNECTED) { 
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("RSSI:");
    Serial.println(rssi);
    delay(250);
  } 
}


