#include <ESP8266WiFi.h>
#include <time.h>
#include <ESP8266HTTPClient.h>
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
    return true;
}


TEST_CASE("Can sync time", "[time]")
{
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

TEST_CASE("#1745 mktime and localtime", "[time]")
{
    struct tm tm_in;
    struct tm tm_out;

    const int years[] = {2012, 2013, 2014};
    const time_t timestamps[] = {1332640800, 1364176800, 1395712800};

    for (size_t i = 0; i < sizeof(years)/sizeof(years[0]); ++i) {
        tm_in.tm_year = years[i] - 1900;
        tm_in.tm_mon = 2;
        tm_in.tm_mday = 25;
        tm_in.tm_hour = 2;
        tm_in.tm_min = 0;
        tm_in.tm_sec = 0;
        tm_in.tm_wday = 0;
        time_t ts = mktime(&tm_in);
        CHECK(ts == timestamps[i]);
        localtime_r(&ts, &tm_out);
        CHECK(tm_in.tm_year == tm_out.tm_year);
        CHECK(tm_in.tm_mon  == tm_out.tm_mon );
        CHECK(tm_in.tm_mday == tm_out.tm_mday);
        CHECK(tm_in.tm_hour == tm_out.tm_hour);
        CHECK(tm_in.tm_min  == tm_out.tm_min );
        CHECK(tm_in.tm_sec  == tm_out.tm_sec );
        CHECK(tm_in.tm_wday == tm_out.tm_wday);
    }
}

void loop()
{
}
