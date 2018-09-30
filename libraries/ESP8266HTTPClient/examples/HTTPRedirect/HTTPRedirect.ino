/**
   HTTPRedirect.ino

    Created on: 29-Sep-2018

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define WIFI_SSID "SSID"
#define WIFI_PASS "Password"


ESP8266WiFiMulti WiFiMulti;


void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 10; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    // we need the Location header to handle redirects
    const char * headerkeys[] = { "Location" };
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    // track these headers
    http.collectHeaders(headerkeys, headerkeyssize);

    Serial.print("[HTTP] begin...\n");
    // configure url
    http.begin("http://jigsaw.w3.org/HTTP/300/307.html"); //HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // As long as we get redirects, follow them
    while(httpCode == 301 || httpCode == 302 || httpCode == 307) {
      String location = http.header("Location");
      Serial.printf("[HTTP] redirect(%d): %s\n", httpCode, location.c_str());
      http.end();
      if (!http.setURL(location)) {
        Serial.printf("[HTTP] Bad redirect location: '%s'", location.c_str());
        return;
      }
      httpCode = http.GET();
    }

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... httpCode: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(10000);
}
