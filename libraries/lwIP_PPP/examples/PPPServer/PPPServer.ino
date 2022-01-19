
// This is still beta / a work in progress

// To run this sketch an (other) USB-serial converter is needed connected to RX-TX ports (below)
// hardware serial is used for logging
// software serial is used for the PPP link
// this example is subject for changes once everything is stabilized

// testing on linux:
// sudo /usr/sbin/pppd /dev/ttyUSB1 38400 noipdefault nocrtscts local defaultroute noauth nodetach debug dump
// sudo /usr/sbin/pppd /dev/ttyUSB1 38400 noipdefault nocrtscts local defaultroute noauth

// proxy arp is needed but we don't have it
// http://lwip.100.n7.nabble.com/PPP-proxy-arp-support-tp33286p33345.html
// using NAT instead

#if LWIP_FEATURES && !LWIP_IPV6

#include <lwip/napt.h>
#include <lwip/dns.h>
#include <PPPServer.h>
#include <dhcpserver.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

#define LOGGERBAUD 115200
#define PPPLINKBAUD 38400

#define NAPT 200
#define NAPT_PORT 3

#define RX 13 // d1mini D7
#define TX 15 // d1mini D8

SoftwareSerial ppplink(RX, TX);
HardwareSerial& logger = Serial;
PPPServer ppp(&ppplink);

void PPPConnectedCallback(netif* nif) {
  logger.printf("ppp: ip=%s/mask=%s/gw=%s\n",
      IPAddress(&nif->ip_addr).toString().c_str(),
      IPAddress(&nif->netmask).toString().c_str(),
      IPAddress(&nif->gw).toString().c_str());

  logger.printf("Heap before: %d\n", ESP.getFreeHeap());
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  logger.printf("ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(nif->num, 1);
    logger.printf("ip_napt_enable(nif): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
      logger.printf("PPP client is NATed\n");
    }

    // could not make this work yet,
    // but packets are arriving on ppp client (= linux host)
    logger.printf("redirect22=%d\n", ip_portmap_add(IP_PROTO_TCP, ip_2_ip4(&nif->ip_addr)->addr, 22, ip_2_ip4(&nif->gw)->addr, 22));
    logger.printf("redirect80=%d\n", ip_portmap_add(IP_PROTO_TCP, ip_2_ip4(&nif->ip_addr)->addr, 80, ip_2_ip4(&nif->gw)->addr, 80));
    logger.printf("redirect443=%d\n", ip_portmap_add(IP_PROTO_TCP, ip_2_ip4(&nif->ip_addr)->addr, 443, ip_2_ip4(&nif->gw)->addr, 443));
  }
  logger.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
  if (ret != ERR_OK) {
    logger.printf("NAPT initialization failed\n");
  }
}

void setup() {
  logger.begin(LOGGERBAUD);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    logger.print('.');
    delay(500);
  }
  logger.printf("\nSTA: %s (dns: %s / %s)\n",
      WiFi.localIP().toString().c_str(),
      WiFi.dnsIP(0).toString().c_str(),
      WiFi.dnsIP(1).toString().c_str());

  ppplink.begin(PPPLINKBAUD);
  ppplink.enableIntTx(true);
  logger.println();
  logger.printf("\n\nhey, trying to be a PPP server here\n\n");
  logger.printf("Now try this on your linux host:\n\n");
  logger.printf("connect a serial<->usb module (e.g. to /dev/ttyUSB1) and link it to the ESP (esprx=%d esptx=%d), then run:\n\n", RX, TX);
  logger.printf("sudo /usr/sbin/pppd /dev/ttyUSB1 %d noipdefault nocrtscts local defaultroute noauth nodetach debug dump\n\n", PPPLINKBAUD);

  ppp.ifUpCb(PPPConnectedCallback);
  bool ret = ppp.begin(WiFi.localIP());
  logger.printf("ppp: %d\n", ret);
}

#else

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nPPP/NAPT not supported in this configuration\n");
}

#endif

void loop() {
}
