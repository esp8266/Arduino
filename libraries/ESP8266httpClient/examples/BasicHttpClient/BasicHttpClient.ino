/**
 * BasicHttpClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266httpClient.h>


ESP8266WiFiMulti WiFiMulti;

void setup() {
    Serial.begin(115200);


    Serial.println();
    Serial.println();
    Serial.println();


      for(uint8_t t = 4; t > 0; t--) {
          Serial.printf("[SETUP] WAIT %d...\n", t);
          Serial.flush();
          delay(1000);
      }

    WiFiMulti.addAP("SSID", "PASSWORD");

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }
}

void loop() {
    if((WiFiMulti.run() == WL_CONNECTED)) {
        httpClient http;

        Serial.print("[HTTP] begin...\n");

        http.begin("192.168.1.12", 80, "/test.html");

        Serial.print("[HTTP] GET...\n");
        if(http.GET()) {

            Serial.print("[HTTP] GET... ok.\n");

            size_t len = http.getSize();

            uint8_t buff[128] = { 0 };
            WiFiClient stream = http.getStream();
            while(http.connected() && len > 0) {
                size_t size = stream.available();
                int c = stream.readBytes(buff, ((size > 128) ? 128 : size));

                Serial.write(buff, c);
                len -= c;

                delay(0);
            }
            Serial.println();
            Serial.print("[HTTP] connection closed or file end.\n");

        } else {

            Serial.print("[HTTP] GET... fail.\n");
        }

        delay(10000);
    }
}
