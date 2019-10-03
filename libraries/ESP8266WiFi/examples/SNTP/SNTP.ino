
/*
  arduino SNTP example
  released to public domain

  This demonstrates how the built-in SNTP client can be used to automatically 
  maintain system time. The SNTP servers can be configured via DHCP (using 
  option 42), DHCPv6 or manually.

  example output:

  ------------------------------
  SDK:2.2.2-dev(38a443e)/Core:2.5.2-173-g875991d0=20502173/lwIP:IPv6+STABLE-2_1_2_RELEASE/glue:1.2-19-g5416edd/BearSSL:89454af
  dns0: 192.168.1.3 IPv6:No
  dns1: 2001:470:xxxx:xxxx::3 IPv6:Yes
  sntp0: 192.168.1.3 IPv6:No
  sntp1: 2001:470:xxxx:xxxx::3 IPv6:Yes
  Current UTC time: Thu Oct  3 14:32:32 2019
  ------------------------------

*/

#include <ESP8266WiFi.h>
#include <PolledTimeout.h> 
#include <time.h>
#include <sntp.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

//
// Uncomment the define to allow static configuration of an SNTP server
//
//#define SNTP_SERVER "0.pool.ntp.org"

#define STATUSDELAY_MS 10000

esp8266::polledTimeout::periodicMs statusPeriod(STATUSDELAY_MS);

const char* getTimeString(void) {
  static char   acTimeString[32];
  
  time_t now = time(nullptr);
  asctime_r(gmtime(&now), acTimeString);
  size_t stLength;
  while (((stLength = strlen(acTimeString))) &&
         ('\n' == acTimeString[stLength - 1])) {
    acTimeString[stLength - 1] = 0; // Remove trailing line break...
  }
  return acTimeString;
}

void status(Print& out) {
  out.println(F("------------------------------"));
  out.println(ESP.getFullVersion());

  for (int i = 0; i < DNS_MAX_SERVERS; i++) {
    IPAddress dns = WiFi.dnsIP(i);
    if (dns.isSet()) {
      out.printf("dns%d: %s IPv6:%s\n", i, dns.toString().c_str(), dns.isV6() ? "Yes" : "No");
    }
  }

//
// When using LwIP v2 we can list all currently configured SNTP servers
//
#if SNTP_MAX_SERVERS && LWIP_VERSION_MAJOR != 1
  for (int i = 0; i < SNTP_MAX_SERVERS; i++) {
    IPAddress sntp = *sntp_getserver(i);
    
    if (sntp.isSet()) {
      out.printf("sntp%d: %s IPv6:%s\n", i, sntp.toString().c_str(), sntp.isV6() ? "Yes" : "No");
    }
  }
#endif

  out.printf("Current UTC time: %s\n", getTimeString());
  out.println(F("------------------------------"));
}

void setup() {
  WiFi.hostname("sntptest");

  Serial.begin(115200);
  Serial.println();
  Serial.println(ESP.getFullVersion());

#if LWIP_IPV6
  Serial.printf("IPV6 is enabled\n");
#else
  Serial.printf("IPV6 is NOT enabled\n");
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  //
  // Override the default timezone of UTC+8 so that system 
  // time is reported in UTC
  //
  sntp_set_timezone(0);

  //
  // If we are using a static SNTP server turn off DHCP updates
  // which will overwrite it
  //
  #ifdef SNTP_SERVER
    sntp_servermode_dhcp(0);
    sntp_setservername(0,SNTP_SERVER);  
  #endif
  
  status(Serial);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  Serial.println(F("connected: "));

  //
  // We can only turn on SNTP after connection when using LwIP v1
  //
  #if LWIP_VERSION_MAJOR == 1
    sntp_init();
    sntp_setservername(0,SNTP_SERVER);  
  #endif
  
  statusPeriod.reset();
}

unsigned long statusTimeMs = 0;

void loop() {
  if (statusPeriod) {
    status(Serial);
  }
}
