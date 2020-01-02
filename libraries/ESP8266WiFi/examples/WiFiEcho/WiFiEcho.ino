/*
  WiFiEcho - Echo tester

  released to public domain
*/

#include <ESP8266WiFi.h>
#include <PolledTimeout.h>
#include <algorithm> // std::min

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

constexpr int port = 23;

WiFiServer server(port);
WiFiClient client;

constexpr size_t stackProtector = 128;
constexpr uint32_t breathMs = 200;
esp8266::polledTimeout::oneShotFastMs enoughMs(breathMs);
int t = 1; // test (1, 2 or 3, see below)

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

  //start server
  server.begin();
  //server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.printf(" %d' to try echo, use a bandwidth meter and try typing 1, 2 or 3 on console during transfer\n", port);

}

void loop() {

  //check if there are any new clients
  if (server.hasClient()) {
    client = server.available();
    Serial.println("New client");
  }

  if (Serial.available())
    switch (Serial.read()) {
      case '1': t = 1; Serial.println("byte-by-byte"); break;
      case '2': t = 2; Serial.println("through buffer"); break;
      case '3': t = 3; Serial.println("direct access"); break;
    }

  enoughMs.reset(breathMs);

  if (t == 1) {
    // byte by byte
    while (client.available() && client.availableForWrite() && !enoughMs) {
      // working char by char is not efficient
      client.write(client.read());
    }
  }

  else if (t == 2) {
    // block by block through a local buffer (2 copies)
    while (client.available() && client.availableForWrite() && !enoughMs) {
      size_t maxTo = std::min(client.available(), client.availableForWrite());
      maxTo = std::min(maxTo, stackProtector);
      uint8_t buf[maxTo];
      size_t tcp_got = client.read(buf, maxTo);
      size_t tcp_sent = client.write(buf, tcp_got);
      if (tcp_sent != maxTo) {
        Serial.printf("len mismatch: available:%zd tcp-read:%zd serial-write:%zd\n", maxTo, tcp_got, tcp_sent);
      }
    }
  }

  else if (t == 3) {
    // stream to print, possibly with only one copy
    //client.to(&client); // <=> client.to(&client, -1, -1, client->getTimeout())
    client.to(&client, -1, -1, breathMs);

    switch (client.getLastTo()) {
      case Stream::STREAMTO_SUCCESS: break;
      case Stream::STREAMTO_TIMED_OUT: Serial.println("Stream::to: timeout"); break;
      case Stream::STREAMTO_READ_ERROR: Serial.println("Stream::to: read error"); break;
      case Stream::STREAMTO_WRITE_ERROR: Serial.println("Stream::to: write error"); break;
      case Stream::STREAMTO_SHORT: Serial.println("Stream::to: short transfer"); break;
    }
  }

}
