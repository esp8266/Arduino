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
  #include <wl_definitions.h>

  struct tcp_pcb;
}

#include <Server.h>
#include <IPAddress.h>
#include <lwip/err.h>

// lwIP-v2 backlog facility allows to keep memory safe by limiting the
// maximum number of incoming *pending clients*.  Default number of possibly
// simultaneously pending clients is defined in WiFiServer.cpp
// (MAX_PENDING_CLIENTS_PER_PORT=5).  User can override it at runtime from
// sketch:
//      WiFiServer::begin(port, max-simultaneous-pending-clients);
//
// An "incoming pending" client is a new incoming TCP connection trying to
// reach the TCP server.  It is "pending" until lwIP acknowledges it and
// "accepted / no more pending" when user calls WiFiServer::available().
//
// Before the backlog feature or with lwIP-v1.4, there was no pending
// connections: They were immediately accepted and filling RAM.
//
// Several pending clients can appear during the time when one client is
// served by a long not-async service like ESP8266WebServer.  During that
// time WiFiServer::available() cannot be called.
//
// Note: This *does not limit* the number of *simultaneously accepted
//       clients*.  Such limit management is left to the user.
//
// Thus, when the maximum number of pending connections is reached, new
// connections are delayed.
// By "delayed", it is meant that WiFiServer(lwIP) will not answer to the
// SYN packet until there is room for a new one: The TCP server on that port
// will be mute.  The TCP client will regularly try to connect until success
// or a timeout occurs (72s on windows).
//
// When user calls WiFiServer::available(), the tcp server stops muting and
// answers to newcomers (until the "backlog" pending list is full again).

class ClientContext;
class WiFiClient;

class WiFiServer {
  // Secure server needs access to all the private entries here
protected:
  uint16_t _port;
  IPAddress _addr;
  tcp_pcb* _listen_pcb = nullptr;

  ClientContext* _unclaimed = nullptr;
  ClientContext* _discarded = nullptr;
  enum { _ndDefault, _ndFalse, _ndTrue } _noDelay = _ndDefault;

public:
  WiFiServer(const IPAddress& addr, uint16_t port);
  WiFiServer(uint16_t port);
  virtual ~WiFiServer() {}
  WiFiClient accept(); // https://www.arduino.cc/en/Reference/EthernetServerAccept
  WiFiClient available(uint8_t* status = NULL) __attribute__((deprecated("Renamed to accept().")));
  bool hasClient();
  // hasClientData():
  // returns the amount of data available from the first client
  // or 0 if there is none
  size_t hasClientData();
  // hasMaxPendingClients():
  // returns true if the queue of pending clients is full
  bool hasMaxPendingClients();
  void begin();
  void begin(uint16_t port);
  void begin(uint16_t port, uint8_t backlog);
  void setNoDelay(bool nodelay);
  bool getNoDelay();
  uint8_t status();
  uint16_t port() const;
  void close();
  void stop();

  using ClientType = WiFiClient;

protected:
  err_t  _accept(tcp_pcb* newpcb, err_t err);
  void   _discard(ClientContext* client);

  static err_t _s_accept(void *arg, tcp_pcb* newpcb, err_t err);
  static void _s_discard(void* server, ClientContext* ctx);

#if CORE_MOCK
  void _mockUnclaimed ();
#endif

};

#endif
