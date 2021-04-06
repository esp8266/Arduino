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
    #include "wl_definitions.h"
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
#include <include/ClientContext.h>
#include "c_types.h"
#include <StreamDev.h>

uint16_t WiFiClient::_localPort = 0;

static bool defaultNoDelay = false; // false == Nagle enabled by default
static bool defaultSync = false;

bool getDefaultPrivateGlobalSyncValue ()
{
    return defaultSync;
}

void WiFiClient::setDefaultNoDelay (bool noDelay)
{
    defaultNoDelay = noDelay;
}

void WiFiClient::setDefaultSync (bool sync)
{
    defaultSync = sync;
}

bool WiFiClient::getDefaultNoDelay ()
{
    return defaultNoDelay;
}

bool WiFiClient::getDefaultSync ()
{
    return defaultSync;
}

template<>
WiFiClient* SList<WiFiClient>::_s_first = 0;


WiFiClient::WiFiClient()
: _client(0)
{
    _timeout = 5000;
    WiFiClient::_add(this);
}

WiFiClient::WiFiClient(ClientContext* client)
: _client(client)
{
    _timeout = 5000;
    _client->ref();
    WiFiClient::_add(this);

    setSync(defaultSync);
    setNoDelay(defaultNoDelay);
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
    _timeout = other._timeout;
    _localPort = other._localPort;
    if (_client)
        _client->ref();
    WiFiClient::_add(this);
}

WiFiClient& WiFiClient::operator=(const WiFiClient& other)
{
   if (_client)
        _client->unref();
    _client = other._client;
    _timeout = other._timeout;
    _localPort = other._localPort;
    if (_client)
        _client->ref();
    return *this;
}

int WiFiClient::connect(const char* host, uint16_t port)
{
    IPAddress remote_addr;
    if (WiFi.hostByName(host, remote_addr, _timeout))
    {
        return connect(remote_addr, port);
    }
    return 0;
}

int WiFiClient::connect(const String& host, uint16_t port)
{
    return connect(host.c_str(), port);
}

int WiFiClient::connect(IPAddress ip, uint16_t port)
{
    if (_client) {
        stop();
        _client->unref();
        _client = nullptr;
    }

    tcp_pcb* pcb = tcp_new();
    if (!pcb)
        return 0;

    if (_localPort > 0) {
        pcb->local_port = _localPort++;
    }

    _client = new ClientContext(pcb, nullptr, nullptr);
    _client->ref();
    _client->setTimeout(_timeout);
    int res = _client->connect(ip, port);
    if (res == 0) {
        _client->unref();
        _client = nullptr;
        return 0;
    }

    setSync(defaultSync);
    setNoDelay(defaultNoDelay);

    return 1;
}

void WiFiClient::setNoDelay(bool nodelay) {
    if (!_client)
        return;
    _client->setNoDelay(nodelay);
}

bool WiFiClient::getNoDelay() const {
    if (!_client)
        return false;
    return _client->getNoDelay();
}

void WiFiClient::setSync(bool sync)
{
    if (!_client)
        return;
    _client->setSync(sync);
}

bool WiFiClient::getSync() const
{
    if (!_client)
        return false;
    return _client->getSync();
}

int WiFiClient::availableForWrite ()
{
    return _client? _client->availableForWrite(): 0;
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
    _client->setTimeout(_timeout);
    StreamConstPtr ptr(buf, size);
    return _client->write(ptr);
}

size_t WiFiClient::write(Stream& stream, size_t unused)
{
    (void) unused;
    return WiFiClient::write(stream);
}

size_t WiFiClient::write(Stream& stream)
{
    if (!_client || !stream.available())
    {
        return 0;
    }
    if (stream.hasPeekBufferAPI())
    {
        _client->setTimeout(_timeout);
        return _client->write(stream);
    }
    return stream.sendAvailable(this);
}

size_t WiFiClient::write_P(PGM_P buf, size_t size)
{
    if (!_client || !size)
    {
        return 0;
    }
    _client->setTimeout(_timeout);
    StreamConstPtr nopeek(buf, size);
    return nopeek.sendAll(this);
}

int WiFiClient::available()
{
    if (!_client)
        return 0;

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
    return (int)_client->read((char*)buf, size);
}

int WiFiClient::read(char* buf, size_t size)
{
    return (int)_client->read(buf, size);
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

bool WiFiClient::flush(unsigned int maxWaitMs)
{
    if (!_client)
        return true;

    if (maxWaitMs == 0)
        maxWaitMs = WIFICLIENT_MAX_FLUSH_WAIT_MS;
    return _client->wait_until_acked(maxWaitMs);
}

bool WiFiClient::stop(unsigned int maxWaitMs)
{
    if (!_client)
        return true;

    bool ret = flush(maxWaitMs); // virtual, may be ssl's
    if (_client->close() != ERR_OK)
        ret = false;
    return ret;
}

uint8_t WiFiClient::connected()
{
    if (!_client || _client->state() == CLOSED)
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
    return available() || connected();
}

IPAddress WiFiClient::remoteIP()
{
    if (!_client || !_client->getRemoteAddress())
        return IPAddress(0U);

    return _client->getRemoteAddress();
}

uint16_t WiFiClient::remotePort()
{
    if (!_client)
        return 0;

    return _client->getRemotePort();
}

IPAddress WiFiClient::localIP()
{
    if (!_client || !_client->getLocalAddress())
        return IPAddress(0U);

    return IPAddress(_client->getLocalAddress());
}

uint16_t WiFiClient::localPort()
{
    if (!_client)
        return 0;

    return _client->getLocalPort();
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


void WiFiClient::keepAlive (uint16_t idle_sec, uint16_t intv_sec, uint8_t count)
{
    _client->keepAlive(idle_sec, intv_sec, count);
}

bool WiFiClient::isKeepAliveEnabled () const
{
    return _client->isKeepAliveEnabled();
}

uint16_t WiFiClient::getKeepAliveIdle () const
{
    return _client->getKeepAliveIdle();
}

uint16_t WiFiClient::getKeepAliveInterval () const
{
    return _client->getKeepAliveInterval();
}

uint8_t WiFiClient::getKeepAliveCount () const
{
    return _client->getKeepAliveCount();
}

bool WiFiClient::hasPeekBufferAPI () const
{
    return true;
}

// return a pointer to available data buffer (size = peekAvailable())
// semantic forbids any kind of read() before calling peekConsume()
const char* WiFiClient::peekBuffer ()
{
    return _client? _client->peekBuffer(): nullptr;
}

// return number of byte accessible by peekBuffer()
size_t WiFiClient::peekAvailable ()
{
    return _client? _client->peekAvailable(): 0;
}

// consume bytes after use (see peekBuffer)
void WiFiClient::peekConsume (size_t consume)
{
    if (_client)
        _client->peekConsume(consume);
}
