/*
  This example is a variation on BasicOTA.

  As is, this example will "always" erase WiFi Settings and reset after a
  successful update. You can make this conditional.
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

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  Serial.println(String("Reset Reason: ") + ESP.getResetReason());

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
      By calling "ArduinoOTA.setEraseConfig(ArduinoOTA::OTA_ERASE_CFG_ABORT_ON_ERROR),"
      this example will erase the "WiFi Settings" as part of an OTA update. When
      erasing WiFi Settings fails, the OTA Update aborts, and eboot will not
      copy the new ".bin" in place.

      Without the call to "ArduinoOTA.setEraseConfig" legacy behavior, the
      system restarts without touching the WiFi Settings.

      Options for "setEraseConfig" to handle eraseConfig failures:
        OTA_ERASE_CFG_NO              - Do not erase WiFi Settings
        OTA_ERASE_CFG_IGNORE_ERROR    - Ignore the error and continue with update ".bin" copy
        OTA_ERASE_CFG_ABORT_ON_ERROR  - Cancel flash update copy at reboot

      To meet unique requirements, you can make the call below conditional.
      Also, this call could be enabled before ArduinoOTA.onEnd() and canceled
      here with "ArduinoOTA.setEraseConfig(OTA_ERASE_CFG_NO)."
    */
    ArduinoOTA.setEraseConfig(OTA_ERASE_CFG_ABORT_ON_ERROR);
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
      Serial.println("Erase WiFi Settings Failed");
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
