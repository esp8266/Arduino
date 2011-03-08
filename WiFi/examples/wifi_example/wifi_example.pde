/*
 WiFi example

 A simple connection with WiFi AP with Wireless Security
 information try to access with WPA or WEP security keys

 created 13 July 2010
 by Domenico La Fauci
 */

#include <WiFi.h>
#include <IPAddress.h>

byte mac[6] = { 0 };
IPAddress ip;
IPAddress gateway;
IPAddress subnet;


void setup()
{
  Serial.begin(9600);
  Serial.println("Setup WiFi...");
  char ssid[] = "Cariddi";
  Serial.println(ssid);
  int status = WiFi.begin(ssid);
  if ( status != WL_CONNECTED)
  {
    // Using WPA
    Serial.println("Trying with Passphrase...");
    const char *pass = "12345678";
    status = WiFi.begin(ssid, pass);
    if ( status != WL_CONNECTED)
    {
      // using WEP
      Serial.println("Trying with Key...");
      uint8_t key_idx = 0;
      const char *key = "12345678";
      status = WiFi.begin(ssid,key_idx, key);
      if ( status != WL_CONNECTED)
      {
        Serial.println("Wifi Connection failed!");
        return;
      }
    }
  }
  Serial.println("Wifi Connected!");

  ip = WiFi.localIp();

  Serial.print("IP: ");
  Serial.print(ip[3],10);Serial.print(".");
  Serial.print(ip[2],10);Serial.print(".");
  Serial.print(ip[1],10);Serial.print(".");
  Serial.println(ip[0],10);

  subnet = WiFi.subnetMask();
  Serial.print("NETMASK: ");
  Serial.print(subnet[3],10);Serial.print(".");
  Serial.print(subnet[2],10);Serial.print(".");
  Serial.print(subnet[1],10);Serial.print(".");
  Serial.println(subnet[0],10);

  gateway = WiFi.gatewayIP();
  Serial.print("GATEWAY: ");
  Serial.print(gateway[3],10);Serial.print(".");
  Serial.print(gateway[2],10);Serial.print(".");
  Serial.print(gateway[1],10);Serial.print(".");
  Serial.println(gateway[0],10);

  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],16);Serial.print(":");
  Serial.print(mac[4],16);Serial.print(":");
  Serial.print(mac[3],16);Serial.print(":");
  Serial.print(mac[2],16);Serial.print(":");
  Serial.print(mac[1],16);Serial.print(":");
  Serial.println(mac[0],16);
}

void loop()
{
}

