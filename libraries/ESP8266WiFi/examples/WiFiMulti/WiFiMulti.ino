/*
    This sketch shows how to use multiple WiFi networks.

    In this example, ESP8266 works in AP mode.
    It demonstrates:
    - Fast connect to previous WiFi network at startup
    - Registering multiple networks (at least 1)
    - Connect to WiFi with strongest signal (RSSI)
    - Fall back to connect to next WiFi when a connection failed or lost
    - Fall back to connect to hidden SSID's which are not reported by WiFi scan
    - Connect to WiFi using a static IP configuration

    To enable debugging output, select in the Arduino iDE:
    - Tools | Debug Port: Serial
    - Tools | Debug Level: WiFi
*/

#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 5000;

void setup() {
  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);

  Serial.begin(115200);
  Serial.println("\nESP8266 Multi WiFi example");

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Register multi WiFi networks
  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  // Register custom SSID callback
  wifiMulti.onSSIDSelected(onSSIDSelected);

  // Insert more sketch code
}

void onSSIDSelected(const char *ssid) {
  // Before connecting to the second AP, assign static IP using config(...).
  if (strcmp(ssid, "ssid_from_AP_2") == 0) {
    IPAddress ip2(192, 168, 1, 123);
    IPAddress gw2(192, 168, 1, 1);
    IPAddress subnet2(255, 255, 255, 0);
    WiFi.config(ip2, gw2, subnet2);
    return;
  }

  // Make sure that DHCP is used for other networks.
  WiFi.config(0U, 0U, 0U);
}

void loop() {
  // Maintain WiFi connection
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not connected!");
  }

  delay(1000);
}
