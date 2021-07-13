#include <StreamString.h>

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2
const unsigned telnetPort = 23;
#define STACK_PROTECTOR  512 // bytes

WiFiServer server(telnetPort);
WiFiClient serverClients[MAX_SRV_CLIENTS];
StreamString telnetOut;

void telnetAgentSetup(void) {
  //start server
  server.begin();
  server.setNoDelay(true);
}

void handleTelnetAgent(void) {
  //check if there are any new clients
  if (server.hasClient()) {

    //find free/disconnected spot
    int i;
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
      if (!serverClients[i]) { // equivalent to !serverClients[i].connected()
        serverClients[i] = server.available();
        Serial.print("New client: index ");
        Serial.print(i);
        break;
      }

    //no free/disconnected spot so reject
    if (i == MAX_SRV_CLIENTS) {
      server.available().println("busy");
      // hints: server.available() is a WiFiClient with short-term scope
      // when out of scope, a WiFiClient will
      // - flush() - all data will be sent
      // - stop() - automatically too
      Serial.printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
    }
  }

  //check TCP clients for data
#if 1
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    while (serverClients[i].available()) {
      int hotKey = serverClients[i].read();

      if ((hotKey > 0 && hotKey > ' ') || hotKey == '\r') {
        cmdLoop(telnetOut, hotKey);
      }
    }

#else
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    while (serverClients[i].available()) {
      size_t maxToSerial = 1; //std::min(serverClients[i].available(), Serial.availableForWrite());
      maxToSerial = std::min(maxToSerial, (size_t)STACK_PROTECTOR);
      uint8_t buf[maxToSerial];
      size_t tcp_got = serverClients[i].read(buf, maxToSerial);
      size_t serial_sent = 1; //Serial.write(buf, tcp_got);
      cmdLoop(telnetOut, *buf);
      if (serial_sent != maxToSerial) {
        Serial.printf("len mismatch: available:%zd tcp-read:%zd serial-write:%zd\n", maxToSerial, tcp_got, serial_sent);
      }
    }
#endif

  // determine maximum output size "fair TCP use"
  // client.availableForWrite() returns 0 when !client.connected()
  size_t maxToTcp = 0;
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    if (serverClients[i]) {
      size_t afw = serverClients[i].availableForWrite();
      if (afw) {
        if (!maxToTcp) {
          maxToTcp = afw;
        } else {
          maxToTcp = std::min(maxToTcp, afw);
        }
      } else {
        // warn but ignore congested clients
        Serial.println("one client is congested");
      }
    }

  //check responce buffer telnetOut
  size_t len = std::min((size_t)telnetOut.available(), maxToTcp);
  len = std::min(len, (size_t)STACK_PROTECTOR);
  if (len) {
    uint8_t sbuf[len];
    int serial_got = telnetOut.readBytes(sbuf, len);
    // push UART data to all connected telnet clients
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
      // if client.availableForWrite() was 0 (congested)
      // and increased since then,
      // ensure write space is sufficient:
      if (serverClients[i].availableForWrite() >= serial_got) {
        size_t tcp_sent = serverClients[i].write(sbuf, serial_got);
        if (tcp_sent != len) {
          Serial.printf("len mismatch: available:%zd serial-read:%zd tcp-write:%zd\n", len, serial_got, tcp_sent);
        }
      }
  }
}
