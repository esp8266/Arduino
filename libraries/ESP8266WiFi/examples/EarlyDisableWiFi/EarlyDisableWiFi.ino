
#include <ESP8266WiFi.h>
#include <AddrList.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

// preinit() is called before system startup
// from nonos-sdk's user entry point user_init()

void preinit() {
  // Global WiFi constructors are not called yet
  // (global class instances like WiFi, Serial... are not yet initialized)..
  // No global object methods or C++ exceptions can be called in here!
  //The below is a static class method, which is similar to a function, so it's ok.
  ESP8266WiFiClass::preinitWiFiOff();
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("sleeping 5s");

  // during this period, a simple amp meter shows
  // an average of 20mA with a Wemos D1 mini
  // a DSO is needed to check #2111
  delay(5000);

  Serial.println("waking WiFi up, sleeping 5s");
  WiFi.forceSleepWake();

  // amp meter raises to 75mA
  delay(5000);

  Serial.println("connecting to AP " STASSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  for (bool configured = false; !configured;) {
    for (auto addr : addrList)
      if ((configured = !addr.isLocal() && addr.ifnumber() == STATION_IF)) {
        Serial.printf("STA: IF='%s' hostname='%s' addr= %s\n",
                      addr.ifname().c_str(),
                      addr.ifhostname(),
                      addr.toString().c_str());
        break;
      }
    Serial.print('.');
    delay(500);
  }

  // amp meter cycles within 75-80 mA

}

void loop() {
}
