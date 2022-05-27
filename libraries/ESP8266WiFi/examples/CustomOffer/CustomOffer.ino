#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LwipDhcpServer.h>

void setup() {
    dhcpSoftAP.add_custom_offer_option(
        DhcpServer::Option{
            .code = 43,
            .data = {0xca,0xfe,0xfe,0xfe,0xfe}
        });
    WiFi.softAP("TEST", "testtesttest");
}

void loop() {
    delay(100);
}
