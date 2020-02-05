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

// lwIP-v2 backlog facility allows to keep memory safe by limiting the
// maximum number of simultaneously accepted clients.  Default number of
// possibly simultaneously accepted client is defined in WiFiServer.cpp,
// user can overide it at runtime from sketch:
//      WiFiServer::begin(port, max-simultaneous-clients-per-port);
// New incoming clients to this server will be delayed until an already
// connected one leaves.
// By "delayed", it is meant that WiFiServer will not answer to the SYN
// packet until there is room for a new one: The server (tcp on that port)
// will be invisible.  When a connected client leaves, the server will
// accept a newcomer at the moment when one of them retries to connect by
// sending another SYN (which will not immediately happen), and before it
// times-out trying to connect.
//
// Back to Arduino scenario:
//
// Usually only one client is used at a time: The client (like a http
// browser) is accepted, served then closed.  The ESP8266WebServer cannot
// answer to another client during that time even if it is tcp-accepted
// (syn-acked), until the current one has been served.
//
// Now that backlog is enabled, at most
// MAX_DEFAULT_SIMULTANEOUS_CLIENTS_PER_PORT clients will be tcp-accepted by
// a WiFiServer even if it is currently busy serving one client (ie: busy
// and not checking WiFiServer::available()/hasClient()).  But next clients
// will be kept out and will have to tcp-retry (as if they were trying to
// connect through a mute firewall):
// - Then eventually one of the connected clients leaves
// - Then a while *after* that moment a waiting/potential client say hello
//   if it has not given up waiting (72 seconds on windows for the connect
//   timeout).
//
// Note that ESPAsync servers may serve more than one client at a time so
// the default setting is not 1 or 2.  It was 0xff by default.  It can be
// overriden at runtime in WiFiServer::begin().

class ClientContext;
class WiFiClient;

class WiFiServer : public Server {
  // Secure server needs access to all the private entries here
protected:
  uint16_t _port;
  IPAddress _addr;
  tcp_pcb* _listen_pcb;

  ClientContext* _unclaimed;
  ClientContext* _discarded;
  enum { _ndDefault, _ndFalse, _ndTrue } _noDelay = _ndDefault;

public:
  WiFiServer(const IPAddress& addr, uint16_t port);
  WiFiServer(uint16_t port);
  virtual ~WiFiServer() {}
  WiFiClient available(uint8_t* status = NULL);
  bool hasClient();
  void begin();
  void begin(uint16_t port) { begin(port, 0); }
  void begin(uint16_t port, uint8_t backlog);
  void setNoDelay(bool nodelay);
  bool getNoDelay();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  uint8_t status();
  uint16_t port() const;
  void close();
  void stop();

  using Print::write;
  using ClientType = WiFiClient;

protected:
  long _accept(tcp_pcb* newpcb, long err);
  void   _discard(ClientContext* client);

  static long _s_accept(void *arg, tcp_pcb* newpcb, long err);
  static void _s_discard(void* server, ClientContext* ctx);
};

#endif
