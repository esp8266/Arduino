/*
  WiFiClient.cpp - TCP/IP client for esp8266, mostly compatible
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

extern "C"
{
    #include "include/wl_definitions.h"
    #include "osapi.h"
    #include "ets_sys.h"
}

#include "debug.h"
#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "lwip/opt.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "include/ClientContext.h"
#include "c_types.h"

uint16_t WiFiClient::_localPort = 0;

template<>
WiFiClient* SList<WiFiClient>::_s_first = 0;


WiFiClient::WiFiClient()
: _client(0)
{
    WiFiClient::_add(this);
}

WiFiClient::WiFiClient(ClientContext* client) : _client(client)
{
    _client->ref();
    WiFiClient::_add(this);
}

WiFiClient::~WiFiClient()
{
    WiFiClient::_remove(this);
    if (_client)
        _client->unref();
}

WiFiClient::WiFiClient(const WiFiClient& other)
{
    _client = other._client;
    if (_client)
        _client->ref();
    WiFiClient::_add(this);
}

WiFiClient& WiFiClient::operator=(const WiFiClient& other)
{
   if (_client)
        _client->unref();
    _client = other._client;
    if (_client)
        _client->ref();
    return *this;
}


int WiFiClient::connect(const char* host, uint16_t port)
{
    IPAddress remote_addr;
    if (WiFi.hostByName(host, remote_addr))
    {
        return connect(remote_addr, port);
    }
    return 0;
}

int WiFiClient::connect(IPAddress ip, uint16_t port)
{
    ip_addr_t addr;
    addr.addr = ip;

    if (_client)
        stop();

    // if the default interface is down, tcp_connect exits early without
    // ever calling tcp_err
    // http://lists.gnu.org/archive/html/lwip-devel/2010-05/msg00001.html
    netif* interface = ip_route(&addr);
    if (!interface) {
        DEBUGV("no route to host\r\n");
        return 0;
    }

    tcp_pcb* pcb = tcp_new();
    if (!pcb)
        return 0;

    if (_localPort > 0) {
        pcb->local_port = _localPort++;
    }

    tcp_arg(pcb, this);
    tcp_err(pcb, &WiFiClient::_s_err);
    tcp_connect(pcb, &addr, port, reinterpret_cast<tcp_connected_fn>(&WiFiClient::_s_connected));

    esp_yield();
    if (_client)
        return 1;

    //  if tcp_error was called, pcb has already been destroyed.
    // tcp_abort(pcb);
    return 0;
}

int8_t WiFiClient::_connected(void* pcb, int8_t err)
{
    tcp_pcb* tpcb = reinterpret_cast<tcp_pcb*>(pcb);
    _client = new ClientContext(tpcb, 0, 0);
    _client->ref();
    esp_schedule();
    return ERR_OK;
}

void WiFiClient::_err(int8_t err)
{
    DEBUGV(":err %d\r\n", err);
    esp_schedule();
}


void WiFiClient::setNoDelay(bool nodelay) {
    if (!_client)
        return;
    _client->setNoDelay(nodelay);
}

bool WiFiClient::getNoDelay() {
    if (!_client)
        return false;
    return _client->getNoDelay();
}

size_t WiFiClient::write(uint8_t b)
{
    return write(&b, 1);
}

size_t WiFiClient::write(const uint8_t *buf, size_t size)
{
    if (!_client || !size)
    {
        return 0;
    }

    return _client->write(reinterpret_cast<const char*>(buf), size);
}

size_t WiFiClient::write_P(PGM_P buf, size_t size)
{
    if (!_client || !size)
    {
        return 0;
    }

    char chunkUnit[WIFICLIENT_MAX_PACKET_SIZE + 1];
    chunkUnit[WIFICLIENT_MAX_PACKET_SIZE] = '\0';
    size_t remaining_size = size;

    while (buf != NULL && remaining_size > 0) {
        size_t chunkUnitLen = WIFICLIENT_MAX_PACKET_SIZE;

        if (remaining_size < WIFICLIENT_MAX_PACKET_SIZE) chunkUnitLen = remaining_size;
        // due to the memcpy signature, lots of casts are needed
        memcpy_P((void*)chunkUnit, (PGM_VOID_P)buf, chunkUnitLen);

        buf += chunkUnitLen;
        remaining_size -= chunkUnitLen;

        // write is so overloaded, had to use the cast to get it pick the right one
        _client->write((const char*)chunkUnit, chunkUnitLen);
    }
    return size;
}

int WiFiClient::available()
{
    if (!_client)
        return false;

    int result = _client->getSize();

    if (!result) {
        optimistic_yield(100);
    }
    return result;
}

int WiFiClient::read()
{
    if (!available())
        return -1;

    return _client->read();
}


int WiFiClient::read(uint8_t* buf, size_t size)
{
    return (int) _client->read(reinterpret_cast<char*>(buf), size);
}

int WiFiClient::peek()
{
    if (!available())
        return -1;

    return _client->peek();
}

size_t WiFiClient::peekBytes(uint8_t *buffer, size_t length) {
    size_t count = 0;

    if(!_client) {
        return 0;
    }

    _startMillis = millis();
    while((available() < (int) length) && ((millis() - _startMillis) < _timeout)) {
        yield();
    }

    if(available() < (int) length) {
        count = available();
    } else {
        count = length;
    }

    return _client->peekBytes((char *)buffer, count);
}

void WiFiClient::flush()
{
    if (_client)
        _client->flush();
}

void WiFiClient::stop()
{
    if (!_client)
        return;

    _client->unref();
    _client = 0;
}

uint8_t WiFiClient::connected()
{
    if (!_client)
        return 0;

    return _client->state() == ESTABLISHED || available();
}

uint8_t WiFiClient::status()
{
    if (!_client)
        return CLOSED;
    return _client->state();
}

 WiFiClient::operator bool()
{
    return _client != 0;
}

IPAddress WiFiClient::remoteIP()
{
    if (!_client)
        return IPAddress(0U);

    return IPAddress(_client->getRemoteAddress());
}

uint16_t WiFiClient::remotePort()
{
    if (!_client)
        return 0;

    return _client->getRemotePort();
}

IPAddress WiFiClient::localIP()
{
    if (!_client)
        return IPAddress(0U);

    return IPAddress(_client->getLocalAddress());
}

uint16_t WiFiClient::localPort()
{
    if (!_client)
        return 0;

    return _client->getLocalPort();
}

int8_t WiFiClient::_s_connected(void* arg, void* tpcb, int8_t err)
{
    return reinterpret_cast<WiFiClient*>(arg)->_connected(tpcb, err);
}

void WiFiClient::_s_err(void* arg, int8_t err)
{
    reinterpret_cast<WiFiClient*>(arg)->_err(err);
}

void WiFiClient::stopAll()
{
    for (WiFiClient* it = _s_first; it; it = it->_next) {
        it->stop();
    }
}


void WiFiClient::stopAllExcept(WiFiClient* except) 
{
    for (WiFiClient* it = _s_first; it; it = it->_next) {
        if (it != except) {
            it->stop();
        }
    }
}
