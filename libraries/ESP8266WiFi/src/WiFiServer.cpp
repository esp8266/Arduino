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
#include "include/ClientContext.h"

WiFiServer::WiFiServer(IPAddress addr, uint16_t port)
: _port(port)
, _addr(addr)
, _pcb(nullptr)
, _unclaimed(nullptr)
, _discarded(nullptr)
{
}

WiFiServer::WiFiServer(uint16_t port)
: _port(port)
, _addr((uint32_t) IPADDR_ANY)
, _pcb(nullptr)
, _unclaimed(nullptr)
, _discarded(nullptr)
{
}

void WiFiServer::begin() {
    err_t err;
    tcp_pcb* pcb = tcp_new();
    if (!pcb)
        return;

    ip_addr_t local_addr;
    local_addr.addr = (uint32_t) _addr;
    err = tcp_bind(pcb, &local_addr, _port);

    if (err != ERR_OK) {
        tcp_close(pcb);
        return;
    }

    tcp_pcb* listen_pcb = tcp_listen(pcb);
    if (!listen_pcb) {
        tcp_close(pcb);
        return;
    }
    _pcb = listen_pcb;
    tcp_accept(listen_pcb, &WiFiServer::_s_accept);
    tcp_arg(listen_pcb, (void*) this);
}

void WiFiServer::setNoDelay(bool nodelay) {
    if (!_pcb)
      return;

    if (nodelay)
        tcp_nagle_disable(_pcb);
    else
        tcp_nagle_enable(_pcb);
}

bool WiFiServer::getNoDelay() {
    if (!_pcb)
        return false;
    return tcp_nagle_disabled(_pcb);
}

bool WiFiServer::hasClient() {
    if (_unclaimed)
        return true;
    return false;
}

WiFiClient WiFiServer::available(byte* status) {
    if (_unclaimed) {
        WiFiClient result(_unclaimed);
        _unclaimed = _unclaimed->next();
        DEBUGV("WS:av\r\n");
        return result;
    }

    optimistic_yield(1000);
    return WiFiClient();
}

uint8_t WiFiServer::status()  {
    if (!_pcb)
        return CLOSED;
    return _pcb->state;
}

void WiFiServer::close() {
    if (!_pcb) {
      return;
    }
    tcp_close(_pcb);
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

int8_t WiFiServer::_accept(tcp_pcb* apcb, int8_t err) {
    DEBUGV("WS:ac\r\n");
    ClientContext* client = new ClientContext(apcb, &WiFiServer::_s_discard, this);
    _unclaimed = slist_append_tail(_unclaimed, client);
    tcp_accepted(_pcb);
    return ERR_OK;
}

void WiFiServer::_discard(ClientContext* client) {
    // _discarded = slist_append_tail(_discarded, client);
    DEBUGV("WS:dis\r\n");
}

int8_t WiFiServer::_s_accept(void *arg, tcp_pcb* newpcb, int8_t err) {
    return reinterpret_cast<WiFiServer*>(arg)->_accept(newpcb, err);
}

void WiFiServer::_s_discard(void* server, ClientContext* ctx) {
    reinterpret_cast<WiFiServer*>(server)->_discard(ctx);
}
