/*
 
 This example  prints the Wifi shield's MAC address, and
 scans for available Wifi networks using the Wifi shield.
 Every ten seconds, it scans again. It doesn't actually 
 connect to any network, so no encryption scheme is specified.
 
 Circuit:
 * WiFi shield attached
 
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 4 Mar 2012
 by Tom Igoe
 */


#include <SPI.h>
#include <WiFi.h>

void setup() {
  // initialize serial:
  Serial.begin(9600);

  // attempt to connect using WEP encryption:
  Serial.println("Initializing Wifi...");
  printMacAddress();


  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
}

void loop() {
  delay(10000);
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
}

void printMacAddress() {
  // the MAC address of your Wifi shield
  byte mac[6];                     
  
  // print your MAC address:
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi.scanNetworks();

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);
  
  Serial.println("#\tName\tRSSI\tencryptionType:");
  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet<numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print("\t");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\t");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print("\t");
    Serial.println(WiFi.encryptionType(thisNet));
  }
}


