#include <ESP8266WiFi.h>
#include <time.h>
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


TEST_CASE("Can sync time", "[time]")
{
    int timezone = 3;
    int dst = 0;

    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("\nWaiting for time");
    unsigned timeout = 5000;
    unsigned start = millis();
    while (millis() - start < timeout) {
        time_t now = time(nullptr);
        if (now > (2016 - 1970) * 365 * 24 * 3600) {
            return;
        }
        delay(100);
    }
    {
      time_t now = time(nullptr);
      Serial.println(ctime(&now));
    }
    CHECK(false);
}

void loop()
{
}
