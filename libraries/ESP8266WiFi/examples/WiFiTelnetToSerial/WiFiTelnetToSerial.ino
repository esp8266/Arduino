/*
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WiFi library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <ESP8266WiFi.h>

#ifndef SSID
#define SSID "your-ssid"
#define PSK  "your-password"
#endif

/* SWAP_PINS:
 * 0: use Serial1 for logging (legacy example)
 * 1: configure Serial port on RX:GPIO13 TX:GPIO15
 *    and use SoftwareSerial for logging on standard serial pins
 */
#define SWAP_PINS 1

#if SWAP_PINS
#include <SoftwareSerial.h>
SoftwareSerial* logguer = nullptr;
#else
#define logguer (&Serial1)
#endif

#define STACK_PROTECTOR  512 // bytes

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2
const char* ssid = SSID;
const char* password = PSK;

const int port = 23;

WiFiServer server(port);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {

#if SWAP_PINS
  Serial.swap();
  // Hardware serial is now on RX:GPIO13 TX:GPIO15
  // use SoftwareSerial on regular RX(3)/TX(1) for logging
  logger = new SoftwareSerial(3, 1);
#endif
  logger->begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  logger->print("\nConnecting to ");
  logger->println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    logger->print('.');
    delay(500);
  }
  logger->println();
  logger->println("connected, address=" + WiFi.localIP());

  //start UART and the server
  Serial.begin(115200);
  server.begin();
  server.setNoDelay(true);

  logger->print("Ready! Use 'telnet ");
  logger->print(WiFi.localIP());
  logger->printf(" %d' to connect", port);
}

void loop() {
  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()) {
    //find free/disconnected spot
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
      if (!serverClients[i]) { // equivalent to !serverClients[i].connected()
        serverClients[i] = server.available();
        logger->print("New client: ");
        logger->print(i);
        break;
      }

    //no free/disconnected spot so reject
    if (i == MAX_SRV_CLIENTS) {
      server.available()->println("busy");
      // hints: server.available() is a WiFiClient with short-term scope
      // when out of scope, a WiFiClient will
      // - flush() - all data will be sent
      // - stop() - automatically too
      logger->printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
    }
  }

  //check TCP clients for data
  for (i = 0; i < MAX_SRV_CLIENTS; i++)
    while (serverClients[i].available() && Serial.availableForWrite() > 0)
      // working char by char is not very efficient
      // every single read() will send a TCP ACK
      Serial.write(serverClients[i].read());

  // determine maximum output size "fair TCP use"
  // client.availableForWrite() returns 0 when !client.connected()
  size_t maxToTcp = 0;
  for (i = 0; i < MAX_SRV_CLIENTS; i++)
    if (serverClients[i]) {
      size_t afw = serverClients[i].availableForWrite();
      if (afw) {
        if (!maxToTcp)
          maxToTcp = afw;
        else
          maxToTcp = std::min(maxToTcp, afw);
      } else {
        // warn but ignore congested clients
        logguer->println("one client is congested");
      }
    }

  //check UART for data
  size_t len = std::min(Serial.available(), maxToTcp);
  len = std::min(len, STACK_PROTECTOR);
  if (len) {
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    // push UART data to all connected telnet clients
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
      // if client.availableForWrite() was 0 (congested)
      // and increased since then,
      // ensure write space is sufficient:
      if (serverClient[i].availableForWrite() >= len)
        serverClients[i].write(sbuf, len);
  }
}
