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
#include "cbuf.h"
#include "include/ClientContext.h"

WiFiServer::WiFiServer(uint16_t port)
{
    _port = port;
    _pcb = 0;
    _unclaimed = 0;
    _discarded = 0;
}

void WiFiServer::begin()
{
    err_t err;

    tcp_pcb* pcb = tcp_new();
    if (!pcb)
        return;

    err = tcp_bind(pcb, INADDR_ANY, _port);

    if (err != ERR_OK)
    {
        tcp_close(pcb);
        return;
    }

    tcp_pcb* listen_pcb = tcp_listen(pcb);
    if (!listen_pcb)
    {
        tcp_close(pcb);
        return;
    }
    _pcb = listen_pcb;
    tcp_accept(listen_pcb, &WiFiServer::_s_accept);
    tcp_arg(listen_pcb, (void*) this);
}

extern "C" uint32_t esp_micros_at_task_start();

WiFiClient WiFiServer::available(byte* status)
{
    static uint32_t lastPollTime = 0;

    if (_unclaimed)
    {
        WiFiClient result(_unclaimed);
        _unclaimed = _unclaimed->next();
        DEBUGV("WS:av\r\n");
        return result;
    }

    if (lastPollTime > esp_micros_at_task_start())
        yield();
    lastPollTime = micros();

    return WiFiClient();
}

uint8_t WiFiServer::status() {
    if (!_pcb)
        return CLOSED;
    return _pcb->state;
}


size_t WiFiServer::write(uint8_t b)
{
    return write(&b, 1);
}

size_t WiFiServer::write(const uint8_t *buffer, size_t size)
{
    // write to all clients
    // not implemented
    return 0;
}

template<typename T>
T* slist_append_tail(T* head, T* item)
{
    if (!head)
        return item;
    T* last = head;
    while(last->next())
        last = last->next();
    last->next(item);
    return head;
}

int8_t WiFiServer::_accept(tcp_pcb* apcb, int8_t err)
{
    DEBUGV("WS:ac\r\n");
    ClientContext* client = new ClientContext(apcb, &WiFiServer::_s_discard, this);
    _unclaimed = slist_append_tail(_unclaimed, client);
    tcp_accepted(_pcb);
    // printf("WiFiServer::_accept\r\n");
    return ERR_OK;
}

void WiFiServer::_discard(ClientContext* client)
{
    // _discarded = slist_append_tail(_discarded, client);
    DEBUGV("WS:dis\r\n");
    // printf("WiFiServer::_discard\r\n");
}

int8_t WiFiServer::_s_accept(void *arg, tcp_pcb* newpcb, int8_t err)
{
    return reinterpret_cast<WiFiServer*>(arg)->_accept(newpcb, err);
}

void WiFiServer::_s_discard(void* server, ClientContext* ctx)
{
    reinterpret_cast<WiFiServer*>(server)->_discard(ctx);
}

