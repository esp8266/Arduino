/*
  This example is a variation on BasicOTA.

  After a successful update if will always erase WiFi Settings and reset.
*/
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

struct YourEEPROMData {
  // list of parameters you need to keep
  // ...

  uint32_t sdkCrc; //add this one
};

bool checkSdkCrc() {
  size_t length = strlen(SDK_VERSION);
  uint32_t sdkVersionCrc = crc32(SDK_VERSION, length);

  uint32_t savedSdkVersionCrc;
  EEPROM.begin((sizeof(struct YourEEPROMData) + 3) & ~3);
  EEPROM.get(offsetof(struct YourEEPROMData, sdkCrc), savedSdkVersionCrc);

  if (sdkVersionCrc == savedSdkVersionCrc) {
    return EEPROM.end();
  }

  // Remember new SDK CRC
  EEPROM.put(offsetof(struct YourEEPROMData, sdkVersionCrc);
  if (EEPROM.commit() && EEPROM.end()) {
    // Erase WiFi settings and reset
    ArduinoOTA.eraseConfigAndReset();
  }

  return false;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  Serial.println("Check for changes in SDK Version:");
  if (checkSdkCrc()) {
    Serial.println("  SDK version changed and update to saved details failed.");
  } else {
    Serial.println("  SDK version has not changed.");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    /*
      By calling "ArduinoOTA.setRebootOnSuccess(true, true);", this example
      will always erases the "WiFi Settings" as part of an OTA update.

      Modify the calling of "ArduinoOTA.setRebootOnSuccess(true, true);"
      to meet your requirements.
    */
    if (true) {
      ArduinoOTA.setRebootOnSuccess(true, true); // reboot = true, eraseConfig = true
    }
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
    } else if (error == OTA_ERASE_SETTINGS_ERROR) {
      Serial.println("Failed to erase WiFi Settings");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}
