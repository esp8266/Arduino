
#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>

#include "WifiHealth.h"

#include <umm_malloc/umm_malloc_cfg.h>
#if HEAP_STATIC_RESERVE_SIZE
#include <evlog/src/event_logger.h>
#else
#define EVLOG1(a) do{ (void)a; } while(false)
#endif
#if ENABLE_FLASH_STATS
#include <evlog/src/flash_stats.h>
#endif

#ifndef STASSID
#pragma message("Using default SSID: your-ssid")
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

/*  Erase Config Options
  ERASE_CONFIG_NONE
  ERASE_CONFIG_EEPROM
  ERASE_CONFIG_RF_CAL
  ERASE_CONFIG_PERSISTANT
  ERASE_CONFIG_BLANK_BIN
  ERASE_CONFIG_SDK_DATA
  ERASE_CONFIG_ALL_DATA
*/
ERASE_CONFIG_MASK_t eraseConfigOption = ERASE_CONFIG_BLANK_BIN;

#define String_F(a) String(F(a))


// Make sure handler's never fall out of scope
WiFiEventHandler handler1;
WiFiEventHandler handler2;

extern char **environ;
void setTimeTZ(const char *tz) {
  char **old_environ = environ;
  char * tzEnv[2] = { (char *)tz, NULL };
  environ = &tzEnv[0];
  tzset();
  environ = old_environ;
}

void setTimeTZ(String& tz) {
  setTimeTZ(tz.c_str());
}

void setup() {
  Serial.begin(115200);
  delay(20);
  Serial.println();
  Serial.println("Booting");

  WiFi.persistent(false); // w/o this a flash write occurs at every boot
  WiFi.mode(WIFI_OFF);

  String tz = F("TZ=PST+8PDT+7,M3.2.0M11.1.0");
  setTimeTZ(tz);
  configTime(0, 0, "pool.ntp.org");

  // Register wifi Event to control connection LED
  handler1 = WiFi.onStationModeConnected([](WiFiEventStationModeConnected data) {
    onWiFiConnected(data);
  });
  handler2 = WiFi.onStationModeDisconnected([](WiFiEventStationModeDisconnected data) {
    onWiFiDisconnected(data);
  });

  WifiUp = WiFi.isConnected();
  if (WifiUp) {
    Serial.println(String_F("WiFi was already connected. We are now disconnecting."));
    WiFi.disconnect();
    WifiUp = false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    EVLOG1("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  EVLOG1("WiFi Up!");

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";

      Update.setEraseConfigOption(eraseConfigOption);

    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println();
  scheduleCheck.attach_ms(1000, updateWiFiStats);

  telnetAgentSetup();
}

void printHelp(Print& oStream) {
  oStream.println();
  oStream.println(F("Hot key help:"));
  oStream.println();
  oStream.println(F("    Erase Config Options"));
  oStream.println(F("  0 - ERASE_CONFIG_NONE"));
  oStream.println(F("  1 - ERASE_CONFIG_EEPROM"));
  oStream.println(F("  2 - ERASE_CONFIG_RF_CAL"));
  oStream.println(F("  3 - ERASE_CONFIG_PERSISTANT"));
  oStream.println(F("  4 - ERASE_CONFIG_BLANK_BIN"));
  oStream.println(F("  5 - ERASE_CONFIG_SDK_DATA"));
  oStream.println(F("  6 - ERASE_CONFIG_ALL_DATA"));
  oStream.println(F("  9 - print detailed sector map"));
  oStream.println();
  oStream.println(F("    Other Options"));
  oStream.println(F("  t - time information"));
  oStream.println(F("  u - umm_info"));
  oStream.println(F("  w - WiFi Stats"));
  oStream.println(F("  R - Restart"));
#if ENABLE_FLASH_STATS
  oStream.println(F("  f - flash Stats"));
#endif
#if HEAP_STATIC_RESERVE_SIZE
  oStream.println(F("  m - static reserve memory information"));
  oStream.println(F("  e - EvLog Report"));
  oStream.println(F("  E - EvLog Clear"));
#endif
  oStream.println(F("  ? - This help message"));
  oStream.println();
}

void printTimes(Print& out) {
  //  #if (EVLOG_TIMESTAMP == EVLOG_TIMESTAMP_CLOCKCYCLES)
  {
    uint32_t fraction = esp_get_cycle_count();
    fraction /= clockCyclesPerMicrosecond();
    time_t gtime = (time_t)(fraction / 1000000U);
    fraction %= 1000000;
    const char *ts_fmt = PSTR("%s.%06u: ccount/%d");
    struct tm *tv = gmtime(&gtime);
    char buf[10];
    strftime(buf, sizeof(buf), "%T", tv);
    out.printf_P(ts_fmt, buf, fraction, clockCyclesPerMicrosecond());
    out.println();
  }
  //  #elif (EVLOG_TIMESTAMP == EVLOG_TIMESTAMP_MICROS)
  {
    uint32_t fraction = micros();
    time_t gtime = (time_t)(fraction / 1000000U);
    fraction %= 1000000;
    const char *ts_fmt = PSTR("%s.%06u: micros()");
    struct tm *tv = gmtime(&gtime);
    char buf[10];
    strftime(buf, sizeof(buf), "%T", tv);
    out.printf_P(ts_fmt, buf, fraction);
    out.println();
  }
  //  #elif (EVLOG_TIMESTAMP == EVLOG_TIMESTAMP_MILLIS)
  {
    uint32_t fraction = millis();
    time_t gtime = (time_t)(fraction / 1000U);
    fraction %= 1000U;
    const char *ts_fmt = PSTR("%s.%03u:    millis()");
    char buf[10];
    struct tm *tv = gmtime(&gtime);
    strftime(buf, sizeof(buf), "%T", tv);
    out.printf_P(ts_fmt, buf, fraction);
    out.println();
  }
  //  #endif
}

int cmdLoop(Print& oStream, char inChar) {
  switch (inChar) {
    case 't':
      printLocalTime(oStream);
      oStream.println();
      printTimes(oStream);
      oStream.println();
      break;
#if HEAP_STATIC_RESERVE_SIZE
    case 'm':
      oStream.printf_P(PSTR("umm_get_static_reserve_size %u"), umm_get_static_reserve_size());
      oStream.println();
      oStream.printf_P(PSTR("umm_get_static_reserve_addr %p"), umm_get_static_reserve_addr());
      oStream.println();
      oStream.println();
      break;
#endif
    case '?':
      printHelp(oStream);
      break;
    case 'u':
      umm_info(NULL, true);
      break;

#if ENABLE_FLASH_STATS
    case 'f':
      printFlashStatsReport(oStream);
      oStream.println();
      break;
#endif
    case 'w':
      printWiFiStats(oStream);
      oStream.println();
      break;
    case 'R':
      oStream.println(F("Restart ..."));
      delay(20);
      WiFi.mode(WIFI_OFF);
      ESP.restart();
      break;

#if HEAP_STATIC_RESERVE_SIZE
    case 'E':
      evlog_restart(1);
      oStream.println(F("EvLog restarted."));
      break;
    case 'e':
      evlogPrintReport(oStream);
      oStream.println();
      break;
#endif
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
      if (queueEraseConfig(inChar)) {
        oStream.println(F("Erase config request queued. Press 'R' to process or start OTA Update."));
      }
      oStream.println();
      break;
    case '9':
      printFlashEraseMap(oStream);
      oStream.println();
      break;
    default:
      oStream.println();
      break;
  }
  return 0;
}

void serialClientLoop(void) {
  if (Serial.available() > 0) {
    char inChar = Serial.read();
    cmdLoop(Serial, inChar);
  }
}

void loop() {
  ArduinoOTA.handle();
  serialClientLoop();
  handleTelnetAgent();
}
