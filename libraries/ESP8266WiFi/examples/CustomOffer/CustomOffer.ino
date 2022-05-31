#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LwipDhcpServer.h>

void setup() {
  dhcpSoftAP.custom_offer_options = [](const DhcpServer&, auto& options) {
    options.add(43, { 0xca, 0xfe, 0xca, 0xfe, 0xfe })  // VENDOR is... vendor specific
      .add(114, "https://192.168.4.1");                // Captive portal
  };
  WiFi.softAP("TEST", "testtesttest");
}

void loop() {
  delay(100);
}
