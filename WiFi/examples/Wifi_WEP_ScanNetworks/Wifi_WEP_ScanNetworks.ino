/*
 
 WEP connection using the WiFi shield. Attempts to connect
 and prints out info about the network
 
 Circuit:
 * WiFi shield attached
 
 created 13 July 2010
 by Domenico La Fauci
 modified 5 June 2011
 by Tom Igoe
 */


#include <SPI.h>
#include <WiFi.h>

char ssid[] = "yourNetwork";     // the name of your network
char keyIndex = 0;               // WEP networks can have multiple keys. 
char key[] = "BAE4B2EDB9171646AA0DC8ED19";  // the key you're using to connect
int status = WL_IDLE_STATUS;     // the Wifi radio's status


byte mac[6];                     // the MAC address of your Wifi shield
IPAddress ip;                    // the IP address of your shield
IPAddress gateway;               // the router's address
IPAddress subnet;                // the subnet mask

void setup() {
  // initialize serial:
  Serial.begin(9600);
 
  // attempt to connect using WEP encryption:
  Serial.println("Attempting to connect to WEP-128 network...");
  status = WiFi.begin(ssid, keyIndex, key);
  
  
  Serial.print("SSID: ");
  Serial.println(ssid);

  // scan for existing networks:
  Serial.println("Scanning available networks...");
  scanNetworks();

  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  // if you are connected, print out info about the connection:
  else {
    printIpData();      
    printCurrentNet();
  }
}

void loop() {
  // do nothing
}

void printIpData() {
  // print your WiFi shield's IP address:
  ip = WiFi.localIp();
  Serial.print("IP: ");
  Serial.print(ip[3]);
  Serial.print(".");
  Serial.print(ip[2]);
  Serial.print(".");
  Serial.print(ip[1]);
  Serial.print(".");
  Serial.println(ip[0]);

  // print your subnet mask:
  subnet = WiFi.subnetMask();
  Serial.print("NETMASK: ");
  Serial.print(subnet[3]);
  Serial.print(".");
  Serial.print(subnet[2]);
  Serial.print(".");
  Serial.print(subnet[1]);
  Serial.print(".");
  Serial.println(subnet[0]);

  // print your gateway address:
  gateway = WiFi.gatewayIP();
  Serial.print("GATEWAY: ");
  Serial.print(gateway[3]);
  Serial.print(".");
  Serial.print(gateway[2]);
  Serial.print(".");
  Serial.print(gateway[1]);
  Serial.print(".");
  Serial.println(gateway[0]);

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

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
}

void scanNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi.scanNetworks();

  // print the list of networks seen:
  Serial.print("SSID List:");
  Serial.println(numSsid);
  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet<numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") Network: ");
    Serial.println(WiFi.SSID(thisNet));
  }
}






