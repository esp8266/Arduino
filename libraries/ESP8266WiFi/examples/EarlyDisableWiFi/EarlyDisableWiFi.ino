
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

// preinit() is called before system startup
// from nonos-sdk's user entry point user_init()

void preinit() {
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

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("sleeping 5s");

  // during this period, a simple amp meter shows
  // an average of 20mA with a Wemos D1 mini
  // a DSO is needed to check #2111
  delay(5000);

  Serial.println("waking  WiFi up, sleeping 5s");
  WiFi.forceSleepWake();

  // amp meter raises to 75mA
  delay(5000);

  Serial.println("connecting to AP " STASSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  for (bool configured = false; !configured;) {
    for (auto addr : addrList)
      if ((configured = !addr->isLocal() && addr->ifnumber() == STATION_IF)) {
        Serial.printf("STA: IF='%s' hostname='%s' addr= %s\n",
                      addr->ifname().c_str(),
                      addr->ifhostname(),
                      addr->addr().toString().c_str());
        break;
      }
    Serial.print('.');
    delay(500);
  }

  // amp meter cycles within 75-80 mA

}

void loop() {
}
