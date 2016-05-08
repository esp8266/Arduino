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
    WiFi.persistent(false);
    WiFi.begin(STA_SSID, STA_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    BS_RUN(Serial);
}

// json doesn't allow newlines within a string
auto test_payload = (const __FlashStringHelper *) "\"\
0000000 53 48 45 4c 4c 20 3a 3d 20 2f 62 69 6e 2f 62 61\\n\
0000010 73 68 0a 56 20 3f 3d 20 30 0a 54 45 53 54 5f 4c\\n\
0000020 49 53 54 20 3f 3d 20 24 28 77 69 6c 64 63 61 72\\n\
0000030 64 20 74 65 73 74 5f 2a 2f 2a 2e 69 6e 6f 29 0a\\n\
0000040 45 53 50 38 32 36 36 5f 43 4f 52 45 5f 50 41 54\\n\
0000050 48 20 3f 3d 20 2e 2e 2f 2e 2e 0a 42 55 49 4c 44\\n\
0000060 5f 44 49 52 20 3f 3d 20 24 28 50 57 44 29 2f 2e\\n\
0000070 62 75 69 6c 64 0a 48 41 52 44 57 41 52 45 5f 44\\n\
0000080 49 52 20 3f 3d 20 24 28 50 57 44 29 2f 2e 68 61\\n\
0000090 72 64 77 61 72 65 0a 45 53 50 54 4f 4f 4c 20 3f\\n\
00000a0 3d 20 24 28 45 53 50 38 32 36 36 5f 43 4f 52 45\\n\
123\"";

auto bin_template_1 = (const __FlashStringHelper *) R"({
  "status": 200,
  "statusText": "OK",
  "httpVersion": "HTTP/1.1",
  "headers": [],
  "cookies": [],
  "content": {
    "mimeType": "text/plain",
    "text":
)";

auto bin_template_2 = (const __FlashStringHelper *) R"(
  }
})";

const char *mockbin_fingerprint = "F0 C1 7B 6A 89 4F AA 67 86 15 4A 17 6C 25 52 8E 2F F6 0F 34";

String createBin(bool https)
{
    String payload;
    payload.reserve(
        strlen_P((PGM_P) bin_template_1) + 
        strlen_P((PGM_P) test_payload) + 
        strlen_P((PGM_P) bin_template_2) + 1);
    payload += bin_template_1;
    payload += test_payload;
    payload += bin_template_2;

    HTTPClient http;
    if (https) {
        http.begin("https://mockbin.org/bin/create", mockbin_fingerprint);
    }
    else {
        http.begin("http://mockbin.org/bin/create");
    }
    const char* location = "Location";
    http.collectHeaders(&location, 1);
    http.addHeader("Content-Type", "application/json");
    auto result = http.POST(payload);
    Serial.println(result);
    Serial.println("----");
    Serial.println(http.getString());
    Serial.println("----");
    REQUIRE(result == 201);
    String url = "http://mockbin.org" + http.header(location);
    http.end();
    return url;
}

TEST_CASE("HTTP GET request", "[HTTPClient]")
{
    const int repeatCount = 10;

    String url = createBin(false);
    int heapBefore = ESP.getFreeHeap();
    for (int i = 0; i < repeatCount; ++i) {
        HTTPClient http;
        http.begin(url);
        auto httpCode = http.GET();
        REQUIRE(httpCode == HTTP_CODE_OK);
        String payload = http.getString();
        payload.replace("\n", "\\n");
        String quotedPayload;
        quotedPayload.reserve(payload.length() + 3);
        quotedPayload += "\"";
        quotedPayload += payload;
        quotedPayload += "\"";
        Serial.println("----payload:");
        Serial.println(quotedPayload);
        Serial.println("----");
        Serial.println("----test_payload:");
        Serial.println(test_payload);
        Serial.println("----");
        CHECK(quotedPayload == test_payload);
        http.end();
        delay(100);
    }
    int heapAfter = ESP.getFreeHeap();
    CHECK(heapBefore - heapAfter <= 8);
}

TEST_CASE("HTTPS GET request", "[HTTPClient]")
{
    const int repeatCount = 10;

    String url = createBin(true);
    int heapBefore = ESP.getFreeHeap();
    for (int i = 0; i < repeatCount; ++i) {
        HTTPClient http;
        http.begin(url, mockbin_fingerprint);
        auto httpCode = http.GET();
        REQUIRE(httpCode == HTTP_CODE_OK);
        String payload = http.getString();
        CHECK(payload == test_payload);
        http.end();
        delay(100);
    }
    int heapAfter = ESP.getFreeHeap();
    CHECK(heapBefore - heapAfter <= 8);
}

void loop()
{
}

