/*
  WiFiServer.h - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino LLC.  All right reserved.

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

  Modified by Ivan Grokhotkov, December 2014 - esp8266 support
*/

#ifndef wifiserver_h
#define wifiserver_h

extern "C" {
  #include "include/wl_definitions.h"

  struct tcp_pcb;
}

#include "Server.h"
#include "IPAddress.h"

class ClientContext;
class WiFiClient;

class WiFiServer : public Server {
private:
  uint16_t _port;
  IPAddress _addr;
  tcp_pcb* _pcb;

  ClientContext* _unclaimed;
  ClientContext* _discarded;

public:
  WiFiServer(IPAddress addr, uint16_t port);
  WiFiServer(uint16_t port);
  WiFiClient available(uint8_t* status = NULL);
  bool hasClient();
  void begin();
  void setNoDelay(bool nodelay);
  bool getNoDelay();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  uint8_t status();

  using Print::write;

protected:
  int8_t _accept(tcp_pcb* newpcb, int8_t err);
  void   _discard(ClientContext* client);

  static int8_t _s_accept(void *arg, tcp_pcb* newpcb, int8_t err);
  static void _s_discard(void* server, ClientContext* ctx);
};

#endif
