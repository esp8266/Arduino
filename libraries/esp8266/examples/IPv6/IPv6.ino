
/*
  arduino IPv6 example
  released to public domain

  output is like:

  address= 192.168.1.239
  gateway= 192.168.1.254
  netmask= 255.255.255.0
  dns0= 192.168.1.8
  dns1= 208.67.222.222
  dns2= 0.0.0.0

  Try me at these addresses (with 'telnet <addr>'):
  IPv6   link-scope(intranet)= fe80::6201:94ff:fe1a:8bcd
  IPV6 global-scope(internet)= 2xxx:xxxx:xxxx:xxxx:6201:94ff:fe1a:8bcd

  resolving www.google.com: 216.58.208.196
  resolving par21s04-in-x04.1e100.net: 2a00:1450:4007:811::2004
*/

#include <ESP8266WiFi.h>

#define SSID "ssid"
#define PSK  "psk"

#define FQDN  "www.google.com" // with both IPv4 & IPv6 addresses
#define FQDN6 "par21s04-in-x04.1e100.net" // does not resolve in IPv4 (wanted: better example)
#define STATUSDELAY_MS 30000

WiFiServer statusServer(23);

void fqdn (Print& out, const String& fqdn) {
  out.print("resolving ");
  out.print(fqdn);
  out.print(": ");
  IPAddress result;
  if (WiFi.hostByName(fqdn.c_str(), result)) {
    result.printTo(out);
    out.println();
  } else
    out.println("timeout or not found");
}

void status (Print& out) {
  out.println();

  out.print("address= ");
  WiFi.localIP().printTo(out);
  out.println();
  out.print("gateway= ");
  WiFi.gatewayIP().printTo(out);
  out.println();
  out.print("netmask= ");
  WiFi.subnetMask().printTo(out);
  out.println();

  for (int i = 0; i < 3; i++) {
    out.print("dns");
    out.print(i);
    out.print("= ");
    WiFi.dnsIP(i).printTo(out);
    out.println();
  }

#if LWIP_IPV6
  out.println("Try me at these addresses (with 'telnet <addr>'):");
  out.print("IPv6   link-scope(intranet)= ");
  WiFi.localIP6Link().printTo(out);
  out.println();
  out.print("IPV6 global-scope(internet)= ");
  WiFi.localIP6Global().printTo(out);
  out.println();
#endif

  // lwIP's dns client will ask for IPv4 first (by default)
  // an example is provided with a fqdn which does not resolve with IPv4
  fqdn(out, FQDN);
  fqdn(out, FQDN6);

  out.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(ESP.getFullVersion());

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PSK);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }
  Serial.println("connected");

  statusServer.begin();
}

unsigned long statusTimeMs = 0;

void loop() {
  
  if (statusServer.hasClient()) {
    WiFiClient cli = statusServer.available();
    cli.setNoDelay(0); // should be default, was not default
    status(cli);
    cli.flush(); // not needed before .stop()
    cli.stop();
  }

  unsigned long now = millis();
  if (now > statusTimeMs) {
    statusTimeMs = now + STATUSDELAY_MS;
    status(Serial);
  }

}
