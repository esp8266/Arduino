/*

        This sketch will convert SPIFFS partitions to LittleFS on ESP8266

        Change the `TARGET_FS_LAYOUT` to the partition layout that you want target
        ie  what you are trying to copy from.

        Include in the sketch whatever you want the destination to be, in this case LittleFS,
        but it could be SPIFFS to convert back if need be.

        How it works:  It creates a LittleFS partition between the end of the sketch and the
        start of whatever filesystem you set as target.  This has IMPORTANT implications for the
        amount of data you can move!!!  eg a 4Mb flash module with a 3Mb SPIFFS partition only leaves
        about 450k for the temp file system, so if you have more data than that on your 3Mb SPIFFS it
        will fail.

*/



#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FSTools.h>
#include <LittleFS.h>


#define TARGET_FS_LAYOUT FST::layout_4m3m

FSTools fstools;

#ifndef STASSID
#define STASSID "xxxx"
#define STAPSK  "xxxx"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;


bool migrateFS() {
  if (!fstools.attemptToMountFS(LittleFS)) { //  Attempts to mount LittleFS without autoformat...
    Serial.println(F("Default FS not found"));
    if (fstools.mountAlternativeFS(FST::SPIFFS  /* FST::LITTLEFS */, TARGET_FS_LAYOUT, true)) {
      Serial.println(F("Alternative found"));
      if (fstools.moveFS(LittleFS)) {
        Serial.println(F("FileSystem Moved New FS contents:"));
        fstools.fileListIterator(LittleFS, "/", [](File & f) {
          Serial.printf_P(PSTR(" File: %-30s [%8uB]\n"), f.fullName(), f.size());
        });
        return true;
      }
    }
  }
  return false;
}


void initWiFiOTA() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void setup() {

  WiFi.persistent(false);
  WiFi.disconnect(true);
  Serial.begin(115200);

  Serial.println();
  Serial.printf("SDK Version: %s\n", ESP.getSdkVersion());
  Serial.println("Core Version: " + ESP.getCoreVersion());
  Serial.println("Full Version: " + ESP.getFullVersion());

  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());

  Serial.println("Booting");

  migrateFS(); // MUST call this before calling your own begin();

  initWiFiOTA();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}
