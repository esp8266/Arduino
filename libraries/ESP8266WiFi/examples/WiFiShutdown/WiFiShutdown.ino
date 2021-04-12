
// Demonstrate the use of WiFi.shutdown() and WiFi.resumeFromShutdown()
// Released to public domain

// Current on WEMOS D1 mini (including: LDO, usbserial chip):
// ~85mA during normal operations
// ~30mA during wifi shutdown
//  ~5mA during deepsleep

#ifndef STASSID
#define STASSID "mynetwork"
#define STAPSK "mynetworkpasswd"
#endif

#ifndef RTC_USER_DATA_SLOT_WIFI_STATE
#define RTC_USER_DATA_SLOT_WIFI_STATE 33u
#endif

#include <ESP8266WiFi.h>
#include <include/WiFiState.h> // WiFiState structure details

WiFiState state;

const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  Serial.begin(74880);
  //Serial.setDebugOutput(true);  // If you need debug output
  Serial.println("Trying to resume WiFi connection...");

  // May be necessary after deepSleep. Otherwise you may get "error: pll_cal exceeds 2ms!!!" when trying to connect
  delay(1);

  // ---
  // Here you can do whatever you need to do that doesn't need a WiFi connection.
  // ---

  ESP.rtcUserMemoryRead(RTC_USER_DATA_SLOT_WIFI_STATE, reinterpret_cast<uint32_t *>(&state), sizeof(state));
  unsigned long start = millis();

  if (!WiFi.resumeFromShutdown(&state)
      || (WiFi.waitForConnectResult(10000) != WL_CONNECTED)) {
    Serial.println("Cannot resume WiFi connection, connecting via begin...");
    WiFi.persistent(false);

    if (!WiFi.mode(WIFI_STA)
        || !WiFi.begin(ssid, password)
        || (WiFi.waitForConnectResult(10000) != WL_CONNECTED)) {
      WiFi.mode(WIFI_OFF);
      Serial.println("Cannot connect!");
      Serial.flush();
      ESP.deepSleep(10e6, RF_DISABLED);
      return;
    }
  }

  unsigned long duration = millis() - start;
  Serial.printf("Duration: %f", duration * 0.001);
  Serial.println();

  // ---
  // Here you can do whatever you need to do that needs a WiFi connection.
  // ---

  WiFi.shutdown(0, &state);
  ESP.rtcUserMemoryWrite(RTC_USER_DATA_SLOT_WIFI_STATE, reinterpret_cast<uint32_t *>(&state), sizeof(state));

  // ---
  // Here you can do whatever you need to do that doesn't need a WiFi connection anymore.
  // ---

  Serial.println("Done.");
  Serial.flush();
  ESP.deepSleep(10e6, RF_DISABLED);
}

void loop() {
  // Nothing to do here.
}
