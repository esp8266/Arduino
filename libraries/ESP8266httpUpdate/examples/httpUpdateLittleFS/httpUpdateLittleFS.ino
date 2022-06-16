/**
   httpUpdateLittleFS.ino

    Created on: 05.12.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

ESP8266WiFiMulti WiFiMulti;

#ifndef APSSID
#define APSSID "APSSID"
#define APPSK "APPSK"
#endif

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APSSID, APPSK);
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    Serial.println("Update LittleFS...");

    WiFiClient client;

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    t_httpUpdate_return ret = ESPhttpUpdate.updateFS(client, "http://server/spiffs.bin");
    if (ret == HTTP_UPDATE_OK) {
      Serial.println("Update sketch...");
      ret = ESPhttpUpdate.update(client, "http://server/file.bin");

      switch (ret) {
        case HTTP_UPDATE_FAILED: Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); break;

        case HTTP_UPDATE_NO_UPDATES: Serial.println("HTTP_UPDATE_NO_UPDATES"); break;

        case HTTP_UPDATE_OK: Serial.println("HTTP_UPDATE_OK"); break;
      }
    }
  }
}
