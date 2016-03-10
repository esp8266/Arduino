#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BSTest.h>
#include <test_config.h>

BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    WiFi.persistent(false);
    WiFi.begin(STA_SSID, STA_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    BS_RUN(Serial);
}

TEST_CASE("HTTP client GET request", "[HTTPClient]")
{
    HTTPClient http;
    http.begin("http://httpbin.org/get?a=1&b=asdadf");
    auto httpCode = http.GET();
    CHECK(httpCode == HTTP_CODE_OK);
    String payload = http.getString();
    CHECK(payload.indexOf("\"a\": \"1\"") != -1);
    CHECK(payload.indexOf("\"b\": \"asdad\"") != -1);
    http.end();
}

void loop()
{
}

