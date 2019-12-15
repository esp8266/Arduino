/*
  WiFiEcho - Echo tester

  released to public domain
*/

#include <ESP8266WiFi.h>

#include <algorithm> // std::min

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

#define STACK_PROTECTOR 128

const int port = 23;
int t = 3;

WiFiServer server(port);
WiFiClient client;

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
  Serial.printf(" %d' to connect\n", port);
}

void loop() {

  //check if there are any new clients
  if (server.hasClient()) {
    client = server.available();
    Serial.println("New client");
  }

  if (Serial.available())
    switch (Serial.read()) {
      case '1': t = 1; break;
      case '2': t = 2; break;
      case '3': t = 3; break;
    }

  if (t == 1)
  {
    // byte by byte
    while (client.available() && client.availableForWrite())
    {
      // working char by char is not efficient
      client.write(client.read());
    }
  }

  else if (t == 2)
  {
    // block by block through a local buffer (2 copies)
    while (client.available() && client.availableForWrite()) {
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

  else if (t == 3)
  {
    // stream to print, possibly with only one copy
    client.to(client, -1, 0);  // all of it, immediate return (no timeout)
  }

}
