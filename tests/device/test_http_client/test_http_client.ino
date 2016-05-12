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

TEST_CASE("HTTP GET request", "[HTTPClient]")
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
// TEST_CASE("HTTP GET request", "[HTTPClient]")
// {
//     const int repeatCount = 10;

//     String url = createBin(false);
//     int heapBefore = ESP.getFreeHeap();
//     for (int i = 0; i < repeatCount; ++i) {
//         HTTPClient http;
//         http.begin(url);
//         auto httpCode = http.GET();
//         REQUIRE(httpCode == HTTP_CODE_OK);
//         String payload = http.getString();
//         payload.replace("\n", "\\n");
//         String quotedPayload;
//         quotedPayload.reserve(payload.length() + 3);
//         quotedPayload += "\"";
//         quotedPayload += payload;
//         quotedPayload += "\"";
//         Serial.println("----payload:");
//         Serial.println(quotedPayload);
//         Serial.println("----");
//         Serial.println("----test_payload:");
//         Serial.println(test_payload);
//         Serial.println("----");
//         CHECK(quotedPayload == test_payload);
//         http.end();
//         delay(100);
//     }
//     int heapAfter = ESP.getFreeHeap();
//     CHECK(heapBefore - heapAfter <= 8);
// }

// TEST_CASE("HTTPS GET request", "[HTTPClient]")
// {
//     const int repeatCount = 10;

//     String url = createBin(true);
//     int heapBefore = ESP.getFreeHeap();
//     for (int i = 0; i < repeatCount; ++i) {
//         HTTPClient http;
//         http.begin(url, mockbin_fingerprint);
//         auto httpCode = http.GET();
//         REQUIRE(httpCode == HTTP_CODE_OK);
//         String payload = http.getString();
//         CHECK(payload == test_payload);
//         http.end();
//         delay(100);
//     }
//     int heapAfter = ESP.getFreeHeap();
//     CHECK(heapBefore - heapAfter <= 8);
// }

void loop()
{
}

