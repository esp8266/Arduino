/**
 * reuseConnection.ino
 *
 *  Created on: 22.11.2015
 *
 */


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

HTTPClient http;

void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("SSID", "PASSWORD");


}

void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        http.begin("192.168.1.12", 80, "/test.html");

        int httpCode = http.GET();
        if(httpCode) {
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == 200) {
                http.writeToStream(&USE_SERIAL);
            }
        } else {
            USE_SERIAL.print("[HTTP] GET... failed, no connection or no HTTP server\n");
        }
    }

    delay(1000);
}



