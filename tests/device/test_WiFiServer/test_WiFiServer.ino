#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <BSTest.h>


BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    BS_RUN(Serial);
}

bool pretest()
{
    WiFi.persistent(false);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    MDNS.begin("esp8266-wfs-test");
    return true;
}

TEST_CASE("Simple echo server", "[WiFiServer]")
{
    const uint32_t timeout = 10000;
    const uint16_t port = 5000;
    const int maxRequests = 5;
    const int minRequestLength = 128;
    WiFiServer server(port);
    server.begin();
    auto start = millis();
    
    int replyCount = 0;
    while (millis() - start < timeout) {
        delay(50);
        WiFiClient client = server.available();
        if (!client) {
            continue;
        }
        String request = client.readStringUntil('\n');
        CHECK(request.length() >= minRequestLength);
        client.print(request);
        client.print('\n');
        if (++replyCount == maxRequests) {
            break;
        }
    }
    CHECK(replyCount == maxRequests);
}

void loop()
{
}

