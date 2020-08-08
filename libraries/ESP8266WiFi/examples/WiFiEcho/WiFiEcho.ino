/*
  WiFiEcho - Echo server

  released to public domain
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PolledTimeout.h>
#include <algorithm> // std::min

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

constexpr int port = 23;

WiFiServer server(port);
WiFiClient client;

constexpr size_t sizes [] = { 0, 512, 384, 256, 128, 64, 16, 8, 4 };
constexpr uint32_t breathMs = 200;
esp8266::polledTimeout::oneShotFastMs enoughMs(breathMs);
esp8266::polledTimeout::periodicFastMs test(2000);
int t = 1; // test (1, 2 or 3, see below)
int s = 0; // sizes[] index

void setup() {

  Serial.begin(115200);
  Serial.println(ESP.getFullVersion());

  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  Serial.print("\nConnecting to ");
  Serial.println(STASSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();
  Serial.print("connected, address=");
  Serial.println(WiFi.localIP());

  server.begin();

  MDNS.begin("echo23");

  Serial.print("Ready! Use 'telnet/nc ");
  Serial.print(WiFi.localIP());
  Serial.printf(" %d' to try echo, use a bandwidth meter and try typing 1, 2 or 3 on console during transfer\n", port);
  Serial.printf("Use 'python3 echo-client.py' to measure bandwidth and compare algorithms\n");
}


void loop() {

  MDNS.update();

  static uint32_t tot = 0;
  static uint32_t cnt = 0;
  if (test && cnt)
  {
    Serial.printf("measured-block-size=%u min-free-stack=%u", tot/cnt, ESP.getFreeContStack());
    if (t == 2 && sizes[s])
        Serial.printf(" (blocks: at most %d bytes)", sizes[s]);
    if (t == 3 && sizes[s])
        Serial.printf(" (blocks: exactly %d bytes)", sizes[s]);
    if (t == 3 && !sizes[s])
        Serial.printf(" (blocks: any size)");
    Serial.printf("\n");
  }

  //check if there are any new clients
  if (server.hasClient()) {
    client = server.available();
    Serial.println("New client");
  }

  if (Serial.available())
  {
    s = (s + 1) % (sizeof(sizes) / sizeof(sizes[0]));
    switch (Serial.read()) {
      case '1': if (t != 1) s = 0; t = 1; Serial.println("byte-by-byte"); break;
      case '2': if (t != 2) s = 1; t = 2; Serial.printf("through buffer\n"); break;
      case '3': if (t != 3) s = 0; t = 3; Serial.printf("direct access\n"); break;
    }
    tot = cnt = 0;
    ESP.resetFreeContStack();
  }

  enoughMs.reset(breathMs);

  if (t == 1) {
    // byte by byte
    while (client.available() && client.availableForWrite() && !enoughMs) {
      // working char by char is not efficient
      client.write(client.read());
      cnt++;
      tot+=1;
    }
  }

  else if (t == 2) {
    // block by block through a local buffer (2 copies)
    while (client.available() && client.availableForWrite() && !enoughMs) {
      size_t maxTo = std::min(client.available(), client.availableForWrite());
      maxTo = std::min(maxTo, sizes[s]);
      uint8_t buf[maxTo];
      size_t tcp_got = client.read(buf, maxTo);
      size_t tcp_sent = client.write(buf, tcp_got);
      if (tcp_sent != maxTo) {
        Serial.printf("len mismatch: available:%zd tcp-read:%zd serial-write:%zd\n", maxTo, tcp_got, tcp_sent);
      }
      tot += tcp_sent;
      cnt++;
    }
  }

  else if (t == 3) {
    // stream to print, possibly with only one copy
    if (sizes[s])
      tot += client.toSize(&client, sizes[s]);
    else
      tot += client.toAll(&client);
    cnt++;

    switch (client.getLastTo()) {
      case Stream::STREAMTO_SUCCESS: break;
      case Stream::STREAMTO_TIMED_OUT: Serial.println("Stream::to: timeout"); break;
      case Stream::STREAMTO_READ_ERROR: Serial.println("Stream::to: read error"); break;
      case Stream::STREAMTO_WRITE_ERROR: Serial.println("Stream::to: write error"); break;
      case Stream::STREAMTO_SHORT: Serial.println("Stream::to: short transfer"); break;
    }
  }

}
