#include <Arduino.h>
#include <BSTest.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

extern "C" {
#include "user_interface.h"
}

BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    BS_RUN(Serial);
}

static void stopAll()
{
    WiFiClient::stopAll();
}

static void disconnectWiFI()
{
    wifi_station_disconnect();
}

/* Some IP address that we can try connecting to, and expect a timeout */
#define UNREACHABLE_IP "192.168.255.255"

TEST_CASE("WiFiClient::stopAll during WiFiClient::connect", "[wificlient]")
{
    WiFiClient client;
    Ticker t;
    t.once_ms(500, &stopAll);
    REQUIRE(client.connect(UNREACHABLE_IP, 1024) == 0);
}

TEST_CASE("WiFi disconnect during WiFiClient::connect", "[wificlient]")
{
    WiFiClient client;
    Ticker t;
    t.once_ms(500, &disconnectWiFI);
    REQUIRE(client.connect(UNREACHABLE_IP, 1024) == 0);
}

void loop()
{
}