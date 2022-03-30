
/*
  Interactive script meant for debugging only
  Run it on serial console and keep this source file opened for the list of commands
  Please configure SSID, PSK and IPAddresses below to fit with your network

  Released to public domain
*/

#include "ESP8266WiFi.h"
#include "user_interface.h"

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* SSID = STASSID;
const char* PSK = STAPSK;

IPAddress staticip(192, 168, 1, 123);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PSK);
  Serial.println("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  Serial.print("WL_IDLE_STATUS      = 0\n"
               "WL_NO_SSID_AVAIL    = 1\n"
               "WL_SCAN_COMPLETED   = 2\n"
               "WL_CONNECTED        = 3\n"
               "WL_CONNECT_FAILED   = 4\n"
               "WL_CONNECTION_LOST  = 5\n"
               "WL_WRONG_PASSWORD   = 6\n"
               "WL_DISCONNECTED     = 7\n");
}

void WiFiOn() {
  wifi_fpm_do_wakeup();
  wifi_fpm_close();
  wifi_set_opmode(STATION_MODE);
  wifi_station_connect();
}

void WiFiOff() {
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_do_sleep(0xFFFFFFF);
}

void loop() {
#define TEST(name, var, varinit, func) \
  static decltype(func) var = (varinit); \
  if ((var) != (func)) { \
    var = (func); \
    Serial.printf("**** %s: ", name); \
    Serial.println(var); \
  }

#define DO(x...) \
  Serial.println(F(#x)); \
  x; \
  break

  TEST("Free Heap", freeHeap, 0, ESP.getFreeHeap());
  TEST("WiFiStatus", status, WL_IDLE_STATUS, WiFi.status());
  TEST("STA-IP", localIp, (uint32_t)0, WiFi.localIP());
  TEST("AP-IP", apIp, (uint32_t)0, WiFi.softAPIP());

  switch (Serial.read()) {
    case 'F': DO(WiFiOff());
    case 'N': DO(WiFiOn());
    case '1': DO(WiFi.mode(WIFI_AP));
    case '2': DO(WiFi.mode(WIFI_AP_STA));
    case '3': DO(WiFi.mode(WIFI_STA));
    case 'R': DO(if (((GPI >> 16) & 0xf) == 1) ESP.reset() /* else must hard reset */);
    case 'd': DO(WiFi.disconnect());
    case 'b': DO(WiFi.begin());
    case 'B': DO(WiFi.begin(SSID, PSK));
    case 'r': DO(WiFi.reconnect());
    case 'c': DO(wifi_station_connect());
    case 'a': DO(WiFi.setAutoReconnect(false));
    case 'A': DO(WiFi.setAutoReconnect(true));
    case 'n': DO(WiFi.setSleepMode(WIFI_NONE_SLEEP));
    case 'l': DO(WiFi.setSleepMode(WIFI_LIGHT_SLEEP));
    case 'm': DO(WiFi.setSleepMode(WIFI_MODEM_SLEEP));
    case 'S': DO(WiFi.config(staticip, gateway, subnet));  // use static address
    case 's': DO(WiFi.config(0u, 0u, 0u));                 // back to dhcp client
  }
}
