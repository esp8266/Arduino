/*
 WiFi example
 
 A simple connection with WiFi AP with Wireless Security 
 information loaded in EEPROM, if not available 
 try to access with WPA or WEP security keys
 
 created 13 July 2010
 by Domenico La Fauci
 */

#include <EEPROM.h>
#include <WiFi.h>
#include <utility/wl_types.h>

// network configuration.  gateway and subnet are optional.
byte mac[6] = { 0 };
byte ip[4] = { 0 };
byte gateway[4] = { 0 };
byte subnet[4] = { 0 };


void setup()
{
  Serial.begin(9600);
  Serial.println("Setup WiFi...");
  WiFi.begin();
  if (WiFi.get_status() == WL_NO_SSID_AVAIL)
  {
      //uint8_t result = WiFi.get_result();
      //Serial.println(result,16);
             
      // SSID not present in EEPROM      
      char ssid[] = "Cariddi";
      Serial.println(ssid);
      delay(2000);

      // Using WPA
      WiFi.begin(ssid, strlen(ssid));      
      delay(10000);
      if (WiFi.get_result() == WL_CONNECT_FAILED)
      {
        Serial.println("Trying with Passphrase...");
        uint8_t key_idx = 0;
        const char *pass = "12345678";
        uint8_t pass_len = strlen(pass);
        WiFi.beginp(ssid, strlen(ssid), pass, pass_len);
      }    
	  // wait to trying connection...      
      delay(5000);
      
	  // using WEP     
      if (WiFi.get_result() == WL_CONNECT_FAILED)
      {
        Serial.println("Trying with Key...");
        uint8_t key_idx = 0;
        const char *key = "12345678";
        uint8_t key_len = strlen(key);
        WiFi.begink(ssid, strlen(ssid),key_idx, key, key_len);
      }
      // wait to trying connection...
      delay(5000);
      
      {
          WiFi.getIpAddr(ip,subnet,gateway);
          Serial.print("IP: ");
          Serial.print(ip[3],10);Serial.print(".");
          Serial.print(ip[2],10);Serial.print(".");
          Serial.print(ip[1],10);Serial.print(".");
          Serial.println(ip[0],10);
          
          Serial.print("NETMASK: ");
          Serial.print(subnet[3],10);Serial.print(".");
          Serial.print(subnet[2],10);Serial.print(".");
          Serial.print(subnet[1],10);Serial.print(".");
          Serial.println(subnet[0],10);
          
          Serial.print("GATEWAY: ");
          Serial.print(gateway[3],10);Serial.print(".");
          Serial.print(gateway[2],10);Serial.print(".");
          Serial.print(gateway[1],10);Serial.print(".");
          Serial.println(gateway[0],10);

          WiFi.getMacAddr(mac);
          Serial.print("MAC: ");
          Serial.print(mac[5],16);Serial.print(":");
          Serial.print(mac[4],16);Serial.print(":");
          Serial.print(mac[3],16);Serial.print(":");
          Serial.print(mac[2],16);Serial.print(":");
          Serial.print(mac[1],16);Serial.print(":");
          Serial.println(mac[0],16);

      }
  }
}

void loop()
{
  static uint8_t count = 0;
  while (WiFi.get_status() != WL_CONNECTED)
  {
     uint8_t result = WiFi.get_result(count++);
     Serial.println(result);
    delay(3000);
  }
}

