#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BSTest.h>
#include <test_config.h>
#include <pgmspace.h>

BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    WiFi.persistent(false);
    WiFi.begin(STA_SSID, STA_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    BS_RUN(Serial);
}

const char* fp = "40 A3 6C E3 8A DF A2 D4 13 B0 32 5C 87 44 54 28 0B CE C5 A4";

TEST_CASE("HTTP GET & POST requests", "[HTTPClient]")
{
    {
        // small request
        HTTPClient http;
        http.begin(SERVER_IP, 8088, "/");
        auto httpCode = http.GET();
        REQUIRE(httpCode == HTTP_CODE_OK);
        String payload = http.getString();
        REQUIRE(payload == "hello!!!");
    }
    {
        // request which returns 8000 bytes
        HTTPClient http;
        http.begin(SERVER_IP, 8088, "/data?size=8000");
        auto httpCode = http.GET();
        REQUIRE(httpCode == HTTP_CODE_OK);
        String payload = http.getString();
        auto len = payload.length();
        REQUIRE(len == 8000);
        for (int i = 0; i < len; ++i) {
            if (payload[i] != 'a') {
                REQUIRE(false);
            }
        }
    }
    {
        // can do two POST requests with one HTTPClient object (#1902)
        HTTPClient http;
        http.begin(SERVER_IP, 8088, "/");
        http.addHeader("Content-Type", "text/plain");
        auto httpCode = http.POST("foo");
        Serial.println(httpCode);
        REQUIRE(httpCode == HTTP_CODE_OK);
        http.end();

        httpCode = http.POST("bar");
        REQUIRE(httpCode == HTTP_CODE_OK);
        http.end();
    }
}


TEST_CASE("HTTPS GET request", "[HTTPClient]")
{
    {
        // small request
        HTTPClient http;
        http.begin(SERVER_IP, 8088, "/", fp);
        auto httpCode = http.GET();
        REQUIRE(httpCode == HTTP_CODE_OK);
        String payload = http.getString();
        REQUIRE(payload == "hello!!!");
    }
    {
        // request which returns 8000 bytes
        HTTPClient http;
        http.begin(SERVER_IP, 8088, "/data?size=8000", fp);
        auto httpCode = http.GET();
        REQUIRE(httpCode == HTTP_CODE_OK);
        String payload = http.getString();
        auto len = payload.length();
        REQUIRE(len == 8000);
        for (int i = 0; i < len; ++i) {
            if (payload[i] != 'a') {
                REQUIRE(false);
            }
        }
    }

}

void loop()
{
}

