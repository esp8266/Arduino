/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include "certs.h"

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

ESP8266WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);
  Serial.println("setup() done connecting to ssid '" STASSID "'");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    auto certs = std::make_unique<BearSSL::X509List>(cert_Cloudflare_Inc_ECC_CA_3);
    auto client = std::make_unique<BearSSL::WiFiClientSecure>();

    client->setTrustAnchors(certs.get());
    // Or, if you prefer to use fingerprinting:
    // client->setFingerprint(fingerprint_w3_org);
    // This is *not* a recommended option, as fingerprint changes with the host certificate

    // Or, if you are *absolutely* sure it is ok to ignore the SSL certificate:
    // client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, jigsaw_host, jigsaw_port)) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          // String payload = https.getString(1024);  // optionally pre-reserve string to avoid reallocations in chunk mode
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  Serial.println("Wait 10s before next round...");
  delay(10000);
}
