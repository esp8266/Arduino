/**
   StreamHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

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

    HTTPClient http;

    BearSSL::WiFiClientSecure *client = new BearSSL::WiFiClientSecure ;

    bool mfln = client->probeMaxFragmentLength("tls.mbed.org", 443, 1024);
    Serial.printf("\nConnecting to https://tls.mbed.org\n");
    Serial.printf("Maximum fragment Length negotiation supported: %s\n", mfln ? "yes" : "no");
    if (mfln) {
      client->setBufferSizes(1024, 1024);
    }

    Serial.print("[HTTPS] begin...\n");

    // configure server and url
    const uint8_t fingerprint[20] = {0xEB, 0xD9, 0xDF, 0x37, 0xC2, 0xCC, 0x84, 0x89, 0x00, 0xA0, 0x58, 0x52, 0x24, 0x04, 0xE4, 0x37, 0x3E, 0x2B, 0xF1, 0x41};
    client->setFingerprint(fingerprint);

    if (http.begin(*client, "https://tls.mbed.org/")) {

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {

          // get lenght of document (is -1 when Server sends no Content-Length header)
          int len = http.getSize();

          // create buffer for read
          static uint8_t buff[128] = { 0 };

          // get tcp stream
          WiFiClient * stream = client;

          // read all data from server
          while (http.connected() && (len > 0 || len == -1)) {
            // get available data size
            size_t size = stream->available();

            if (size) {
              // read up to 128 byte
              int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

              // write it to Serial
              Serial.write(buff, c);

              if (len > 0) {
                len -= c;
              }
            }
            delay(1);
          }

          Serial.println();
          Serial.print("[HTTPS] connection closed or file end.\n");

        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("Unable to connect\n");
    }

    delete client;
  }

  delay(10000);
}
