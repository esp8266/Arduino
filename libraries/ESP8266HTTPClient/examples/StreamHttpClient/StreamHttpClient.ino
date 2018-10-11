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

    WiFiClient client;

    Serial.print("[HTTP] begin...\n");

    // configure server and url
    http.begin(client, "http://jigsaw.w3.org/HTTP/connection.html");
    //http.begin(client, "jigsaw.w3.org", 80, "/HTTP/connection.html");

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        // get lenght of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();

        // create buffer for read
        uint8_t buff[128] = { 0 };

        // get tcp stream
        WiFiClient * stream = &client;

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
        Serial.print("[HTTP] connection closed or file end.\n");

      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(10000);
}
