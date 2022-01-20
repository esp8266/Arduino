// Example: storing JSON configuration file in flash file system
//
// Uses ArduinoJson library by Benoit Blanchon.
// https://github.com/bblanchon/ArduinoJson
//
// Created Aug 10, 2015 by Ivan Grokhotkov.
//
// This example code is in the public domain.

#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>

// more and possibly updated information can be found at:
// https://arduinojson.org/v6/example/config/

bool loadConfig()
{
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile)
  {
    Serial.println("Failed to open config file");
    return false;
  }

  StaticJsonDocument<200> doc;
  auto                    error = deserializeJson(doc, configFile);
  if (error)
  {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* serverName  = doc["serverName"];
  const char* accessToken = doc["accessToken"];

  // Real world application would store these values in some variables for
  // later use.

  Serial.print("Loaded serverName: ");
  Serial.println(serverName);
  Serial.print("Loaded accessToken: ");
  Serial.println(accessToken);
  return true;
}

bool saveConfig()
{
  StaticJsonDocument<200> doc;
  doc["serverName"]  = "api.example.com";
  doc["accessToken"] = "128du9as8du12eoue8da98h123ueh9h98";

  File configFile    = LittleFS.open("/config.json", "w");
  if (!configFile)
  {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  delay(1000);
  Serial.println("Mounting FS...");

  if (!LittleFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }

  if (!saveConfig())
  {
    Serial.println("Failed to save config");
  }
  else
  {
    Serial.println("Config saved");
  }

  if (!loadConfig())
  {
    Serial.println("Failed to load config");
  }
  else
  {
    Serial.println("Config loaded");
  }
}

void loop()
{
}
