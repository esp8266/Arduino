
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <ESP8266WiFi.h> 

////////////////////////////////////////////////////////

#define SSID            "open"
#define SSIDPWD         ""
#define TZ              1       // utc+TZ in hours
#define DST_MN          60      // use 60 for summer time in some countries

#define NTP0_OR_LOCAL1  1       // 0:use NTP  1:fake external RTC
#define RTC_TEST     1510592825 // 1510592825 = Monday 13 November 2017 17:07:05 UTC

////////////////////////////////////////////////////////

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

void setup() {
        Serial.begin(115200);
        
#if NTP0_OR_LOCAL1

        // local
        ESP.eraseConfig();
        time_t rtc = RTC_TEST;
        timeval tv = { rtc, 0 };
        timezone tz = { TZ_MN + DST_MN, 0 };
        settimeofday(&tv, &tz);
        
#else

        // NTP
        configTime(TZ_SEC, DST_SEC, "pool.ntp.org");
        WiFi.mode(WIFI_STA);
        WiFi.begin(SSID, SSIDPWD);
        // don't wait

#endif
} 

// for testing purpose:
extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);

void loop() {

        // same result as with time()
        timeval tv;
        gettimeofday(&tv, NULL);
        Serial.print("gtod:");
        Serial.print((uint32_t)tv.tv_sec);
#if 0
        // but usec is wrong
        Serial.print("/");
        Serial.print((uint32_t)tv.tv_usec);
#endif

        // same result as with millis()/1000
        timespec tp;
        clock_gettime(0, &tp);
        Serial.print(" -- clock:");
        Serial.print((uint32_t)tp.tv_sec);
#if 0
        // nsec is wrong
        Serial.print("/");
        Serial.print((uint32_t)tp.tv_nsec/1000000);
        Serial.print("ms");
#endif

        // gives EPOCH+tz+dst
        time_t now = time(nullptr);
        Serial.print(" -- time:");
        Serial.print((uint32_t)now);

        // from boot:
        Serial.print(" -- millis:");
        Serial.print((uint32_t)millis());
        Serial.print(" -- micros:");
        Serial.print((uint32_t)micros());

        // human readable
        Serial.print(" -- ctime:(UTC+");
        Serial.print((uint32_t)(TZ*60 + DST_MN));
        Serial.print("mn)");
        Serial.print(ctime(&now));

        // simple drifting loop
        delay(1000);
}

