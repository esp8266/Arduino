/*
  WiFiServer.cpp - TCP/IP server for esp8266, mostly compatible
                   with Arduino WiFi shield library

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

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


#define LWIP_INTERNAL

extern "C" {
    #include "osapi.h"
    #include "ets_sys.h"
}

#include "debug.h"
#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "lwip/opt.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include <include/ClientContext.h>

#ifndef MAX_PENDING_CLIENTS_PER_PORT
#define MAX_PENDING_CLIENTS_PER_PORT 5
#endif

WiFiServer::WiFiServer(const IPAddress& addr, uint16_t port)
: _port(port)
, _addr(addr)
{
}

WiFiServer::WiFiServer(uint16_t port)
: _port(port)
, _addr(IP_ANY_TYPE)
{
}

void WiFiServer::begin() {
	begin(_port);
}

void WiFiServer::begin(uint16_t port) {
    return begin(port, MAX_PENDING_CLIENTS_PER_PORT);
}

void WiFiServer::begin(uint16_t port, uint8_t backlog) {
    close();
    if (!backlog)
        return;
    _port = port;
    tcp_pcb* pcb = tcp_new();
    if (!pcb)
        return;

    pcb->so_options |= SOF_REUSEADDR;

    // (IPAddress _addr) operator-converted to (const ip_addr_t*)
    if (tcp_bind(pcb, _addr, _port) != ERR_OK) {
        tcp_close(pcb);
        return;
    }

    tcp_pcb* listen_pcb = tcp_listen_with_backlog(pcb, backlog);

    if (!listen_pcb) {
        tcp_close(pcb);
        return;
    }
    _listen_pcb = listen_pcb;
    _port = _listen_pcb->local_port;
    tcp_accept(listen_pcb, &WiFiServer::_s_accept);
    tcp_arg(listen_pcb, (void*) this);
}

void WiFiServer::setNoDelay(bool nodelay) {
    _noDelay = nodelay? _ndTrue: _ndFalse;
}

bool WiFiServer::getNoDelay() {
    switch (_noDelay)
    {
    case _ndFalse: return false;
    case _ndTrue: return true;
    default: return WiFiClient::getDefaultNoDelay();
    }
}

bool WiFiServer::hasClient() {
    if (_unclaimed)
        return true;
    return false;
}

WiFiClient WiFiServer::available(byte* status) {
    (void) status;
    if (_unclaimed) {
        WiFiClient result(_unclaimed);

        // pcb can be null when peer has already closed the connection
        if (_unclaimed->getPCB()) {
            // give permission to lwIP to accept one more peer
            tcp_backlog_accepted(_unclaimed->getPCB());
        }

        _unclaimed = _unclaimed->next();
        result.setNoDelay(getNoDelay());
        DEBUGV("WS:av status=%d WCav=%d\r\n", result.status(), result.available());
        return result;
    }

    yield();
    return WiFiClient();
}

uint8_t WiFiServer::status()  {
    if (!_listen_pcb)
        return CLOSED;
    return _listen_pcb->state;
}

uint16_t WiFiServer::port() const {
    return _port;
}

void WiFiServer::close() {
    if (!_listen_pcb) {
      return;
    }
    tcp_close(_listen_pcb);
    _listen_pcb = nullptr;
}

void WiFiServer::stop() {
    close();
}

size_t WiFiServer::write(uint8_t b) {
    return write(&b, 1);
}

size_t WiFiServer::write(const uint8_t *buffer, size_t size) {
    // write to all clients
    // not implemented
    (void) buffer;
    (void) size;
    return 0;
}

template<typename T>
T* slist_append_tail(T* head, T* item) {
    if (!head)
        return item;
    T* last = head;
    while(last->next())
        last = last->next();
    last->next(item);
    return head;
}

long WiFiServer::_accept(tcp_pcb* apcb, long err) {
    (void) err;
    DEBUGV("WS:ac\r\n");

    // always accept new PCB so incoming data can be stored in our buffers even before
    // user calls ::available()
    ClientContext* client = new ClientContext(apcb, &WiFiServer::_s_discard, this);

    // backlog doc:
    // http://lwip.100.n7.nabble.com/Problem-re-opening-listening-pbc-tt32484.html#a32494
    // https://www.nongnu.org/lwip/2_1_x/group__tcp__raw.html#gaeff14f321d1eecd0431611f382fcd338

    // increase lwIP's backlog
    tcp_backlog_delayed(apcb);

    _unclaimed = slist_append_tail(_unclaimed, client);

    return ERR_OK;
}

void WiFiServer::_discard(ClientContext* client) {
    (void) client;
    // _discarded = slist_append_tail(_discarded, client);
    DEBUGV("WS:dis\r\n");
}

long WiFiServer::_s_accept(void *arg, tcp_pcb* newpcb, long err) {
    return reinterpret_cast<WiFiServer*>(arg)->_accept(newpcb, err);
}

void WiFiServer::_s_discard(void* server, ClientContext* ctx) {
    reinterpret_cast<WiFiServer*>(server)->_discard(ctx);
}
