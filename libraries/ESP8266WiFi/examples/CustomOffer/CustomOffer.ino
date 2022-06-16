#include <Arduino.h>
#include <ESP8266WiFi.h>

void setup() {
  auto& server = WiFi.softAPDhcpServer();
  server.onSendOptions([](const DhcpServer& server, auto& options) {
    // VENDOR is... vendor specific
    options.add(43, { 0xca, 0xfe, 0xca, 0xfe, 0xfe });

    // Captive Portal URI
    const IPAddress gateway = netif_ip4_addr(server.getNetif());
    const String captive = F("http://") + gateway.toString();
    options.add(114, captive.c_str(), captive.length());
  });
  WiFi.softAP("TEST", "testtesttest");
}

void loop() {
  delay(100);
}
