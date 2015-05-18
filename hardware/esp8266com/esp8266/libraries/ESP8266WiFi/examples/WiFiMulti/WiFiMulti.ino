/*
 *  This sketch trys to Connect to the best AP based on a given list
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti = ESP8266WiFiMulti();

void setup() {
    Serial.begin(115200);
    delay(10);
	
    WiFiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
    WiFiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
    WiFiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

	Serial.println("Connecting Wifi...");
    if(wifiMulti.run() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
}

void loop() {
    if(wifiMulti.run() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        delay(1000);
    }
}