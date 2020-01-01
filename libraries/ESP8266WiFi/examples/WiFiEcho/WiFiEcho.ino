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

#define STACK_PROTECTOR 128

const int port = 23;
int t = 1; // test (1, 2 or 3, see below)

WiFiServer server(port);
WiFiClient client;

constexpr uint32_t breathMs = 100;
esp8266::polledTimeout::oneShotFastMs enoughMs(breathMs);

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
      maxTo = std::min(maxTo, (size_t)STACK_PROTECTOR);
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

#if CORE_MOCK

    // (without limit, with a bandwitdh tester, this call would endlessly transfer data)
    client.to(&client, 10000000, 0);  // transfer large chunks, no timeout
    if (client.getLastTo() == Stream::STREAMTO_SHORT)
      // don't really care about really transfered bytes
    {
      client.clearWriteError();
    }

#else

    client.to(&client, -1, 0);  // on esp: no size limit, no timeout

#endif
    switch (client.getLastTo()) {
      case Stream::STREAMTO_SUCCESS: break;
      case Stream::STREAMTO_TIMED_OUT: Serial.println("Stream::to: timeout"); break;
      case Stream::STREAMTO_READ_ERROR: Serial.println("Stream::to: read error"); break;
      case Stream::STREAMTO_WRITE_ERROR: Serial.println("Stream::to: write error"); break;
      case Stream::STREAMTO_SHORT: Serial.println("Stream::to: short transfer"); break;
    }
  }

}
