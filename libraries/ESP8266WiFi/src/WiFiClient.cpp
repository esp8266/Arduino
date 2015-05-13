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
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "cbuf.h"
#include "include/ClientContext.h"
#include "c_types.h"

ICACHE_FLASH_ATTR WiFiClient::WiFiClient() 
: _client(0)
{
}

ICACHE_FLASH_ATTR WiFiClient::WiFiClient(ClientContext* client) : _client(client)
{
    _client->ref();
}

ICACHE_FLASH_ATTR WiFiClient::~WiFiClient()
{
    if (_client)
        _client->unref();
}

ICACHE_FLASH_ATTR WiFiClient::WiFiClient(const WiFiClient& other)
{
    _client = other._client;
    if (_client)
        _client->ref();
}

WiFiClient& ICACHE_FLASH_ATTR  WiFiClient::operator=(const WiFiClient& other)
{
   if (_client)
        _client->unref(); 
    _client = other._client;
    if (_client)
        _client->ref();
    return *this;
}


int ICACHE_FLASH_ATTR  WiFiClient::connect(const char* host, uint16_t port) 
{
    IPAddress remote_addr;
    if (WiFi.hostByName(host, remote_addr))
    {
        return connect(remote_addr, port);
    }
    return 0;
}

int ICACHE_FLASH_ATTR  WiFiClient::connect(IPAddress ip, uint16_t port) 
{
    if (_client)
        stop();

    tcp_pcb* pcb = tcp_new();
    if (!pcb)
        return 0;

    ip_addr_t addr;
    addr.addr = ip;
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

int8_t ICACHE_FLASH_ATTR WiFiClient::_connected(void* pcb, int8_t err)
{
    tcp_pcb* tpcb = reinterpret_cast<tcp_pcb*>(pcb);
    _client = new ClientContext(tpcb, 0, 0);
    _client->ref();
    esp_schedule();
    return ERR_OK;
}

void ICACHE_FLASH_ATTR WiFiClient::_err(int8_t err)
{
    DEBUGV(":err %d\r\n", err);
    esp_schedule();
}

size_t ICACHE_FLASH_ATTR WiFiClient::write(uint8_t b) 
{
    return write(&b, 1);
}

size_t ICACHE_FLASH_ATTR WiFiClient::write(const uint8_t *buf, size_t size) 
{
    if (!_client || !size)
    {
        return 0;
    }

    return _client->write(reinterpret_cast<const char*>(buf), size);
}

extern "C" uint32_t esp_micros_at_task_start();

int ICACHE_FLASH_ATTR WiFiClient::available()
{
    static uint32_t lastPollTime = 0;
    if (!_client)
        return 0;

    if (lastPollTime > esp_micros_at_task_start())
        yield();

    lastPollTime = micros();

    int result = _client->getSize();
    return result;
}

int ICACHE_FLASH_ATTR WiFiClient::read() 
{
    if (!available())
        return -1;

    return _client->read();
}


int ICACHE_FLASH_ATTR WiFiClient::read(uint8_t* buf, size_t size) 
{
    return (int) _client->read(reinterpret_cast<char*>(buf), size);
}

int ICACHE_FLASH_ATTR WiFiClient::peek() 
{
    if (!available())
        return -1;

    return _client->peek();
}

void ICACHE_FLASH_ATTR WiFiClient::flush() 
{
    if (_client)
        _client->flush();
}

void ICACHE_FLASH_ATTR WiFiClient::stop() 
{
    if (!_client)
        return;

    _client->unref();
    _client = 0;
}

uint8_t ICACHE_FLASH_ATTR WiFiClient::connected() 
{
    if (!_client)
        return 0;

    return _client->state() == ESTABLISHED || available();
}

uint8_t ICACHE_FLASH_ATTR WiFiClient::status() 
{
    if (!_client)
        return CLOSED;
    return _client->state();
}

ICACHE_FLASH_ATTR  WiFiClient::operator bool() 
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

int8_t ICACHE_FLASH_ATTR WiFiClient::_s_connected(void* arg, void* tpcb, int8_t err)
{
    return reinterpret_cast<WiFiClient*>(arg)->_connected(tpcb, err);
}

void ICACHE_FLASH_ATTR WiFiClient::_s_err(void* arg, int8_t err)
{
    reinterpret_cast<WiFiClient*>(arg)->_err(err);
}

