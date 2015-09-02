/*
  WiFiClientSecure.cpp - Variant of WiFiClient with TLS support
  Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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
    #include "osapi.h"
    #include "ets_sys.h"
}
#include <errno.h>
#include "debug.h"
#include "ESP8266WiFi.h"
#include "WiFiClientSecure.h"
#include "WiFiClient.h"
#include "lwip/opt.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "cbuf.h"
#include "include/ClientContext.h"
#include "c_types.h"


class SSLContext {
public:
    SSLContext() {
        if (_ssl_ctx_refcnt == 0) {
            _ssl_ctx = ssl_ctx_new(SSL_SERVER_VERIFY_LATER | SSL_DISPLAY_STATES, 0);
        }
        ++_ssl_ctx_refcnt;

    }

    ~SSLContext() {
        if (_ssl) {
            ssl_free(_ssl);
            _ssl = nullptr;
        }

        --_ssl_ctx_refcnt;
        if (_ssl_ctx_refcnt == 0) {
            ssl_ctx_free(_ssl_ctx);
        }
    }

    void ref() {
        ++_refcnt;
    }

    void unref() {
        if (--_refcnt == 0) {
            delete this;
        }
    }

    void connect(ClientContext* ctx) {
        _ssl = ssl_client_new(_ssl_ctx, reinterpret_cast<int>(ctx), nullptr, 0);
    }

    operator SSL*() {
        return _ssl;
    }

protected:
    static SSL_CTX* _ssl_ctx;
    static int _ssl_ctx_refcnt;
    SSL* _ssl = nullptr;
    int _refcnt = 0;
};

SSL_CTX* SSLContext::_ssl_ctx = nullptr;
int SSLContext::_ssl_ctx_refcnt = 0;


WiFiClientSecure::WiFiClientSecure()
{
}

WiFiClientSecure::~WiFiClientSecure()
{
    if (_ssl) {
        _ssl->unref();
    }
}

WiFiClientSecure::WiFiClientSecure(const WiFiClientSecure& other)
: WiFiClient(static_cast<const WiFiClient&>(other))
{
    _ssl = other._ssl;
    if (_ssl) {
        _ssl->ref();
    }
}

WiFiClientSecure& WiFiClientSecure::operator=(const WiFiClientSecure& rhs) {
    (WiFiClient&) *this = rhs;
    _ssl = rhs._ssl;
    if (_ssl) {
        _ssl->ref();
    }
    return *this;
}

int WiFiClientSecure::connect(IPAddress ip, uint16_t port) {
    if (!WiFiClient::connect(ip, port))
        return 0;

    return _connectSSL();
}

int WiFiClientSecure::connect(const char* name, uint16_t port) {
    if (!WiFiClient::connect(name, port))
        return 0;
    return 1;
}

int WiFiClientSecure::_connectSSL() {
    if (_ssl) {
        _ssl->unref();
        _ssl = nullptr;
    }

    _ssl = new SSLContext;
    _ssl->ref();
    _ssl->connect(_client);

    auto status = ssl_handshake_status(*_ssl);
    if (status != SSL_OK) {
        _ssl->unref();
        _ssl = nullptr;
        return 0;
    }

    return 1;
}

size_t WiFiClientSecure::write(const uint8_t *buf, size_t size) {
    int rc = ssl_write(*_ssl, buf, size);
    if (rc >= 0)
        return rc;

    return 0;
}

int WiFiClientSecure::read(uint8_t *buf, size_t size) {

    uint8_t* data;
    int rc = ssl_read(*_ssl, &data);
    if (rc <= 0)
        return 0;

    memcpy(buf, data, rc);
    return rc;
}

void WiFiClientSecure::stop() {
    if (_ssl) {
        _ssl->unref();
        _ssl = nullptr;
    }
    return WiFiClient::stop();
}

extern "C" int ax_port_read(int fd, uint8_t* buffer, size_t count) {
    ClientContext* _client = reinterpret_cast<ClientContext*>(fd);
    if (_client->state() != ESTABLISHED && !_client->getSize()) {
        return -1;
        errno = EIO;
    }
    size_t cb = _client->read((char*) buffer, count);
    if (cb != count) {
        errno = EAGAIN;
    }
    if (cb == 0) {
        yield();
        return -1;
    }
    return cb;
}

extern "C" int ax_port_write(int fd, uint8_t* buffer, size_t count) {
    ClientContext* _client = reinterpret_cast<ClientContext*>(fd);
    if (_client->state() != ESTABLISHED) {
        errno = EIO;
        return -1;
    }
    size_t cb = _client->write((const char*) buffer, count);
    if (cb != count) {
        errno = EAGAIN;
    }
    return cb;
}

extern "C" int ax_get_file(const char *filename, uint8_t **buf) {
    *buf = 0;
    return 0;
}

#ifdef DEBUG_TLS_MEM
#define DEBUG_TLS_MEM_PRINT(...) DEBUGV(__VA_ARGS__)
#else
#define DEBUG_TLS_MEM_PRINT(...)
#endif


extern "C" void* ax_port_malloc(size_t size, const char* file, int line) {
    void* result = malloc(size);

    if (result == nullptr) {
        DEBUG_TLS_MEM_PRINT("%s:%d malloc %d failed, left %d\r\n", file, line, size, ESP.getFreeHeap());

        while(true){}
    }
    if (size >= 1024)
        DEBUG_TLS_MEM_PRINT("%s:%d malloc %d, left %d\r\n", file, line, size, ESP.getFreeHeap());
    return result;
}

extern "C" void* ax_port_calloc(size_t size, size_t count, const char* file, int line) {
    void* result = ax_port_malloc(size * count, file, line);
    memset(result, 0, size * count);
    return result;
}

extern "C" void* ax_port_realloc(void* ptr, size_t size, const char* file, int line) {
    void* result = realloc(ptr, size);
    if (result == nullptr) {
        DEBUG_TLS_MEM_PRINT("%s:%d realloc %d failed, left %d\r\n", file, line, size, ESP.getFreeHeap());
        while(true){}
    }
    if (size >= 1024)
        DEBUG_TLS_MEM_PRINT("%s:%d realloc %d, left %d\r\n", file, line, size, ESP.getFreeHeap());
    return result;
}


extern "C" void ax_port_free(void* ptr) {
    free(ptr);
    uint32_t *p = (uint32_t*) ptr;
    size_t size = p[-3];
    if (size >= 1024)
        DEBUG_TLS_MEM_PRINT("free %d, left %d\r\n", p[-3], ESP.getFreeHeap());
}
