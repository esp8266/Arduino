
#include <ESP8266WiFi.h>

// early_init() is called before system startup
// from nonos-sdk's user entry point user_init()

void early_init() {
  // Global constructors are not called yet
  // (global class instances like WiFi, Serial... are not yet initialized).
  // In this function, we must only use nonos-sdk API.

  // example

  // https://github.com/esp8266/Arduino/issues/2111#issuecomment-224251391
  // WiFi.persistent(false);
  // WiFi.mode(WIFI_OFF);
  // WiFi.forceSleepBegin();

  //WiFi.mode(WIFI_OFF) equivalent:
  // datasheet:
  // Set Wi-Fi working mode to Station mode, SoftAP
  // or Station + SoftAP, and do not update flash
  // (not persistent)
  wifi_set_opmode_current(WIFI_OFF);

  //WiFi.forceSleepBegin(/*default*/0) equivalent:
  // sleep forever until wifi_fpm_do_wakeup() is called
  wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_do_sleep(0xFFFFFFF);

  // use WiFi.forceSleepWake() to wake WiFi up
}

void setup () {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
}

void loop () {
}
