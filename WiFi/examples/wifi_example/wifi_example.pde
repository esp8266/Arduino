/*
 WiFi example
 
 A simple connection with WiFi AP with Wireless Security 
 information loaded in EEPROM
 
 created 13 July 2010
 by Domenico La Fauci
 */

#include <WiFi.h>
#include <EEPROM.h>

void setup()
{   
    Serial.begin(9600);
    WiFi.begin();
  if (WiFi.get_status() == WL_NO_SSID_AVAIL)
  {
      // SSID not present in EEPROM
      char ssid[] = "Cariddi";
      WiFi.begin(ssid, strlen(ssid));
  }
}

void loop()
{
}

