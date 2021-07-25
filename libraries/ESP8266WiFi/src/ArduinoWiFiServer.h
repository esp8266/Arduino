/*
  ArduinoWiFiServer.h - Arduino compatible WiFiServer
  implementation for ESP8266Wifi library.
  Copyright (c) 2020 Juraj Andrassy

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

#ifndef arduinowifiserver_h
#define arduinowifiserver_h

#include <ESP8266WiFi.h>

#ifndef MAX_MONITORED_CLIENTS
#define MAX_MONITORED_CLIENTS 5
#endif

template <class TServer, class TClient>
class ArduinoComptibleWiFiServerTemplate : public TServer {
public:

  ArduinoComptibleWiFiServerTemplate(const IPAddress& addr, uint16_t port) : TServer(addr, port) {}
  ArduinoComptibleWiFiServerTemplate(uint16_t port) : TServer(port) {}
  virtual ~ArduinoComptibleWiFiServerTemplate() {}

  // https://www.arduino.cc/en/Reference/EthernetServerAccept
  TClient accept() {
    return TServer::available();
  }

  // https://www.arduino.cc/en/Reference/WiFiServerAvailable
  TClient available() {

    acceptClients();
 
    // find next client with data available
    for (uint8_t i = 0; i < MAX_MONITORED_CLIENTS; i++) {
      if (index == MAX_MONITORED_CLIENTS) {
        index = 0;
      }
      TClient& client = connectedClients[index];
      index++;
      if (client.available())
        return client;
    }
    return TClient(); // no client with data found
  }

  virtual size_t write(uint8_t b) override {
    return write(&b, 1);
  }

  virtual size_t write(const uint8_t *buf, size_t size) override {
    static uint32_t lastCheck;
    uint32_t m = millis();
    if (m - lastCheck > 100) {
      lastCheck = m;
      acceptClients();
    }
    if (size == 0)
      return 0;
    size_t ret = 0;
    size_t a = size;
    while (true) {
      for (uint8_t i = 0; i < MAX_MONITORED_CLIENTS; i++) {
        WiFiClient& client = connectedClients[i];
        if (client.status() == ESTABLISHED && client.availableForWrite() < (int) a) {
          a = client.availableForWrite();
        }
      }
      if (a == 0)
        break;
      for (uint8_t i = 0; i < MAX_MONITORED_CLIENTS; i++) {
        if (connectedClients[i].status() == ESTABLISHED) {
          connectedClients[i].write(buf, a);
        }
      }
      ret += a;
      if (ret == size)
        break;
      buf += a;
      a = size - ret;
    }
    return ret;
  }

  using Print::write;

  virtual void flush() override {
    flush(0);
  }

  virtual void flush(unsigned int maxWaitMs) {
    for (uint8_t i = 0; i < MAX_MONITORED_CLIENTS; i++) {
      if (connectedClients[i].status() == ESTABLISHED) {
        connectedClients[i].flush(maxWaitMs);
      }
    }
  }

  operator bool() {
    return (TServer::status() == LISTEN);
  }

  void close() {
    TServer::stop();
    for (uint8_t i = 0; i < MAX_MONITORED_CLIENTS; i++) {
      if (connectedClients[i]) {
        connectedClients[i].stop();
      }
    }
  }
  void stop() {close();}
  void end() {close();}

private:
  TClient connectedClients[MAX_MONITORED_CLIENTS];
  uint8_t index = 0;

  void acceptClients() {
    for (uint8_t i = 0; i < MAX_MONITORED_CLIENTS; i++) {
      if (!connectedClients[i]) {
        connectedClients[i] = accept();
      }
    }
  }
};

typedef ArduinoComptibleWiFiServerTemplate<WiFiServer, WiFiClient> ArduinoWiFiServer;
typedef ArduinoComptibleWiFiServerTemplate<WiFiServerSecure, WiFiClientSecure> ArduinoWiFiServerSecure;

#endif
