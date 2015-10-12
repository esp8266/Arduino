#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "...";
const char* password = "...";

ArduinoOTA ota_server;

void setup() {
   Serial.begin(115200);

   Serial.println("Booting");
   WiFi.mode(WIFI_STA);

   /* try the flash stored password first */
   WiFi.begin();

   while (WiFi.waitForConnectResult() != WL_CONNECTED){
     WiFi.begin(ssid, password);
     Serial.println("Retrying connection...");
  }
  ota_server.setup();
  Serial.println("Ready");
}

void loop() {
  ota_server.handle();
  yield();
}
