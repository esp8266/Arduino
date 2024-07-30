/**
   Based on StreamHTTPClient.ino

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "certs.h"

ESP8266WiFiMulti WiFiMulti;

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
  WiFiMulti.addAP("SSID", "PASSWORD");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    auto certs = std::make_unique<BearSSL::X509List>(cert_Amazon_RSA_2048_M02);
    auto client = std::make_unique<BearSSL::WiFiClientSecure>();

    client->setTrustAnchors(certs.get());
    // Or, if you prefer to use fingerprinting:
    // client->setFingerprint(fingerprint___mbed_com);
    // This is *not* a recommended option, as fingerprint changes with the host certificate

    // Or, if you are *absolutely* sure it is ok to ignore the SSL certificate:
    // client->setInsecure();

    bool mfln = client->probeMaxFragmentLength(mbed_host, mbed_port, 1024);
    Serial.printf("\nConnecting to %s:%hu...\n", mbed_host, mbed_port);
    Serial.printf("Maximum fragment Length negotiation supported: %s\n", mfln ? "yes" : "no");
    if (mfln) { client->setBufferSizes(1024, 1024); }

    Serial.print("[HTTPS] begin...\n");

    HTTPClient https;

    if (https.begin(*client, mbed_host, mbed_port)) {

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {

          // get length of document (is -1 when Server sends no Content-Length header)
          int len = https.getSize();

          // create buffer for read
          static uint8_t buff[128] = { 0 };

          // read all data from server
          while (https.connected() && (len > 0 || len == -1)) {
            // get available data size
            size_t size = client->available();

            if (size) {
              // read up to 128 byte
              int c = client->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

              // write it to Serial
              Serial.write(buff, c);

              if (len > 0) { len -= c; }
            }
            delay(1);
          }

          Serial.println();
          Serial.print("[HTTPS] connection closed or file end.\n");
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("Unable to connect\n");
    }
  }

  Serial.println("Wait 10s before the next round...");
  delay(10000);
}
