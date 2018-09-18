
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
#define FQDN6 F("par21s04-in-x04.1e100.net") // does not resolve in IPv4 (wanted: better example)
#define STATUSDELAY_MS 30000
#define TCP_PORT 23
#define UDP_PORT 1111

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
  out.println();

  out.print(F("address= "));
  WiFi.localIP().printTo(out);
  out.println();
  out.print(F("gateway= "));
  WiFi.gatewayIP().printTo(out);
  out.println();
  out.print(F("netmask= "));
  WiFi.subnetMask().printTo(out);
  out.println();

  for (int i = 0; i < 3; i++) {
    out.print(F("dns"));
    out.print(i);
    out.print(F("= "));
    WiFi.dnsIP(i).printTo(out);
    out.println();
  }

  #if LWIP_IPV6
  out.println(F("Try me at these addresses:"));
  out.println(F("(with 'telnet <addr>')"));
  out.println(F("(with 'nc -u <addr> 1111')"));
  out.print(F("IPv6   link-scope(intranet)= "));
  WiFi.localIP6Link().printTo(out);
  out.println();
  out.print(F("IPV6 global-scope(internet)= "));
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

  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PSK);
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

    char* buffer = (char*)malloc(packetSize + 1);
    if (buffer) {
      udp.read(buffer, packetSize);
      buffer[packetSize] = 0;
      Serial.println(F("Contents:"));
      Serial.println(buffer);

      // send a reply, to the IP address and port that sent us the packet we received
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write(buffer, packetSize);
      udp.endPacket();
      free(buffer);
    } else {
      Serial.println(F("udp: malloc failed"));
      while (udp.read() >= 0);
    }
  }

  unsigned long now = millis();
  if (now > statusTimeMs) {
    statusTimeMs = now + STATUSDELAY_MS;
    status(Serial);
  }

}
