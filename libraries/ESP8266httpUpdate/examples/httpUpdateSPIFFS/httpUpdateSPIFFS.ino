/**
 * httpUpdateSPIFFS.ino
 *
 *  Created on: 05.12.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

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

        USE_SERIAL.println("Update SPIFFS...");
        t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs("http://server/spiffs.bin");
        if(ret == HTTP_UPDATE_OK) {
            USE_SERIAL.println("Update sketch...");
            ret = ESPhttpUpdate.update("http://server/file.bin");

            switch(ret) {
                case HTTP_UPDATE_FAILED:
                    USE_SERIAL.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                    break;

                case HTTP_UPDATE_NO_UPDATES:
                    USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
                    break;

                case HTTP_UPDATE_OK:
                    USE_SERIAL.println("HTTP_UPDATE_OK");
                    break;
            }
        }
    }
}

