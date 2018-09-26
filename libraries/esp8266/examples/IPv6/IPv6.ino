
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
#include <WiFiUdp.h>

#define SSID "ssid"
#define PSK  "psk"

#define FQDN  F("www.google.com") // with both IPv4 & IPv6 addresses
#define FQDN6 F("ipv6.google.com") // does not resolve in IPv4
#define STATUSDELAY_MS 30000
#define TCP_PORT 23
#define UDP_PORT 23

WiFiServer statusServer(TCP_PORT);
WiFiUDP udp;

void fqdn(Print& out, const String& fqdn) {
  out.print(F("resolving "));
  out.print(fqdn);
  out.print(F(": "));
  IPAddress result;
  if (WiFi.hostByName(fqdn.c_str(), result)) {
    result.printTo(out);
    out.println();
  } else {
    out.println(F("timeout or not found"));
  }
}

void status(Print& out) {
  out.println(F("------------------------------"));
  out.println(ESP.getFullVersion());

  for (int i = 0; i < 3; i++) {
    IPAddress dns = WiFi.dnsIP(i);
    if (dns.isSet()) {
      out.print(F("dns"));
      out.print(i);
      out.print(F("="));
      dns.printTo(out);
      out.println();
    }
  }

  out.println(F("Try me at these addresses:"));
  out.println(F("(with 'telnet <addr> or 'nc -u <addr> 23')"));
  for (auto a : addrList) {
    out.printf("IF='%s'(%d) IPv6=%d local=%d hostname='%s' addr= ",
               a->ifname().c_str(),
               a->ifnumber(),
               !a->isV4(),
               a->isLocal(),
               a->ifhostname());
    a->addr().printTo(out);

    if (a->isLegacy()) {
      out.print(F(" / mask:"));
      a->netmask().printTo(out);
      out.print(F(" / gw:"));
      a->gw().printTo(out);
    }
    out.println();
  }

  // lwIP's dns client will ask for IPv4 first (by default)
  // an example is provided with a fqdn which does not resolve with IPv4
  fqdn(out, FQDN);
  fqdn(out, FQDN6);

  out.println(F("------------------------------"));
}

void setup() {
  WiFi.hostname("ipv6test");

  Serial.begin(115200);
  Serial.println();
  Serial.println(ESP.getFullVersion());

  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PSK);

  status(Serial);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println(F("connected: "));

  statusServer.begin();
  udp.begin(UDP_PORT);

  Serial.print(F("TCP server on port "));
  Serial.print(TCP_PORT);
  Serial.print(F(" - UDP server on port "));
  Serial.println(UDP_PORT);
}

unsigned long statusTimeMs = 0;

void loop() {

  if (statusServer.hasClient()) {
    WiFiClient cli = statusServer.available();
    status(cli);
    cli.stop();
  }

  // if there's data available, read a packet
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print(F("udp received "));
    Serial.print(packetSize);
    Serial.print(F(" bytes from "));
    udp.remoteIP().printTo(Serial);
    Serial.print(F(" :"));
    Serial.println(udp.remotePort());
    int  c;
    while ((c = udp.read()) >= 0) {
      Serial.write(c);
    }

    // send a reply, to the IP address and port that sent us the packet we received
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    status(udp);
    udp.endPacket();
  }


  unsigned long now = millis();
  if (now > statusTimeMs) {
    statusTimeMs = now + STATUSDELAY_MS;
    status(Serial);
  }

}
