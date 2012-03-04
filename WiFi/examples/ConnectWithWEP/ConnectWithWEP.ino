/*
 
 This example connects to a WEP-encrypted Wifi network. 
 Then it prints the  MAC address of the Wifi shield,
 the IP address obtained, and other network details.
 
 If you use 40-bit WEP, you need a key that is 10 characters long, 
 and the characters must be hexadecimal (0-9 or A-F). 
 e.g.  for 40-bit, ABBADEAF01 will work, but ABBADEAF won't work 
 (too short) and ABBAISDEAF won't work (I and S are not 
 hexadecimal characters). 
 
 For 128-bit, you need a string that is 26 characters long. 
 D0D0DEADF00DABBADEAFBEADED will work because it's 26 characters, 
 all in the 0-9, A-F range.
 
 Circuit:
 * WiFi shield attached
 
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 4 Mar 2012
 by Tom Igoe
 */
#include <WiFi.h>

char ssid[] = "yourNetwork";                     // your network SSID (name) 
char key[] = "D0D0DEADF00DABBADEAFBEADED";       // your network key
int keyIndex = 0;                                // your network key Index number
int status = WL_IDLE_STATUS;                     // the Wifi radio's status

void setup() {
  // initialize serial:
  Serial.begin(9600);

  // attempt to connect to an open network:
  Serial.print("Attempting to connect to WEP network: ");
  Serial.println(ssid);
  status = WiFi.begin(ssid, keyIndex, key);

  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  // if you are connected :
  else {
    Serial.print("You're connected to the network");
    printCurrentNet();
    printWifiData();
  }
}

void loop() {
  // check the network connection once every 10 seconds:
  delay(10000);
  printCurrentNet();
}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
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
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
  Serial.println();
}


