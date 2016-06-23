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

const char* fp = "44 40 9E 34 92 2D E4 61 A4 89 A8 D5 7F 71 B7 62 B3 FD DD E1";

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
        http.begin(SERVER_IP, 8088, "/data?size=4000", fp);
        auto httpCode = http.GET();
        REQUIRE(httpCode == HTTP_CODE_OK);
        String payload = http.getString();
        auto len = payload.length();
        REQUIRE(len == 4000);
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

