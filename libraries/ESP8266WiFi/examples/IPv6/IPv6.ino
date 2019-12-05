/*
  arduino IPv6 example
  released to public domain

  output is like:

  SDK:2.2.1(cfd48f3)/Core:2.4.2-141-g4f97603/lwIP:IPv6+STABLE-2_1_0_RC1/glue:arduino-2.4.2-30-ga53619c/BearSSL:6d1cefc
  dns0=10.43.1.254
  Try me at these addresses:
  (with 'telnet <addr> or 'nc -u <addr> 23')
  IF='st'(0) IPv6=0 local=0 hostname='ipv6test' addr= 10.43.1.244 / mask:255.255.255.0 / gw:10.43.1.254
  IF='st'(0) IPv6=1 local=1 hostname='ipv6test' addr= fe80::1afe:34ff:fed1:cec7
  IF='st'(0) IPV6=1 local=0 hostname='ipv6test' addr= 2xxx:xxxx:xxxx:xxxx:1afe:34ff:fed1:cec7
  resolving www.google.com: 216.58.205.100
  resolving ipv6.google.com: 2a00:1450:4002:808::200e
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PolledTimeout.h>
#include <AddrList.h>
#include <lwip/dns.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

#define FQDN  F("www.google.com") // with both IPv4 & IPv6 addresses
#define FQDN2 F("www.yahoo.com")  // with both IPv4 & IPv6 addresses
#define FQDN6 F("ipv6.google.com") // does not resolve in IPv4
#define STATUSDELAY_MS 10000
#define TCP_PORT 23
#define UDP_PORT 23

WiFiServer statusServer(TCP_PORT);
WiFiUDP udp;
esp8266::polledTimeout::periodicMs showStatusOnSerialNow(STATUSDELAY_MS);

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

#if LWIP_IPV4 && LWIP_IPV6
void fqdn_rt(Print& out, const String& fqdn, DNSResolveType resolveType) {
  out.print(F("resolving "));
  out.print(fqdn);
  out.print(F(": "));
  IPAddress result;
  if (WiFi.hostByName(fqdn.c_str(), result, 10000, resolveType)) {
    result.printTo(out);
    out.println();
  } else {
    out.println(F("timeout or not found"));
  }
}
#endif

void status(Print& out) {
  out.println(F("------------------------------"));
  out.println(ESP.getFullVersion());

  for (int i = 0; i < DNS_MAX_SERVERS; i++) {
    IPAddress dns = WiFi.dnsIP(i);
    if (dns.isSet()) {
      out.printf("dns%d: %s\n", i, dns.toString().c_str());
    }
  }

  out.println(F("Try me at these addresses:"));
  out.println(F("(with 'telnet <addr> or 'nc -u <addr> 23')"));
  for (auto a : addrList) {
    out.printf("IF='%s' IPv6=%d local=%d hostname='%s' addr= %s",
               a.ifname().c_str(),
               a.isV6(),
               a.isLocal(),
               a.ifhostname(),
               a.toString().c_str());

    if (a.isLegacy()) {
      out.printf(" / mask:%s / gw:%s",
                 a.netmask().toString().c_str(),
                 a.gw().toString().c_str());
    }

    out.println();

  }

  // lwIP's dns client will ask for IPv4 first (by default)
  // an example is provided with a fqdn which does not resolve with IPv4
  fqdn(out, FQDN);
  fqdn(out, FQDN6);
#if LWIP_IPV4 && LWIP_IPV6
  fqdn_rt(out, FQDN,  DNSResolveType::DNS_ADDRTYPE_IPV4_IPV6); // IPv4 before IPv6
  fqdn_rt(out, FQDN2, DNSResolveType::DNS_ADDRTYPE_IPV6_IPV4); // IPv6 before IPv4
#endif
  out.println(F("------------------------------"));
}

void setup() {
  WiFi.hostname("ipv6test");

  Serial.begin(115200);
  Serial.println();
  Serial.println(ESP.getFullVersion());

#if LWIP_IPV6
  Serial.printf("IPV6 is enabled\n");
#else
  Serial.printf("IPV6 is not enabled\n");
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  status(Serial);

#if 0 // 0: legacy connecting loop - 1: wait for IPv6

  // legacy loop (still valid with IPv4 only)

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

#else

  // Use this loop instead to wait for an IPv6 routable address

  // addr->isLocal() (meaning "not routable on internet") is true with:
  // - IPV4 DHCP autoconfigured address 169.254.x.x
  //   (false for any other including 192.168./16 and 10./24 since NAT may be in the equation)
  // - IPV6 link-local addresses (fe80::/64)

  for (bool configured = false; !configured;) {
    for (auto addr : addrList)
      if ((configured = !addr.isLocal()
                        // && addr.isV6() // uncomment when IPv6 is mandatory
                        // && addr.ifnumber() == STATION_IF
          )) {
        break;
      }
    Serial.print('.');
    delay(500);
  }

#endif

  Serial.println(F("connected: "));

  statusServer.begin();
  udp.begin(UDP_PORT);

  Serial.print(F("TCP server on port "));
  Serial.print(TCP_PORT);
  Serial.print(F(" - UDP server on port "));
  Serial.println(UDP_PORT);

  showStatusOnSerialNow.reset();
}

unsigned long statusTimeMs = 0;

void loop() {

  if (statusServer.hasClient()) {
    WiFiClient cli = statusServer.available();
    status(cli);
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


  if (showStatusOnSerialNow) {
    status(Serial);
  }

}
