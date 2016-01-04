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
#include "include/ClientContext.h"
#include "c_types.h"

#ifdef DEBUG_ESP_SSL
#define DEBUG_SSL
#endif

#ifdef DEBUG_SSL
#define SSL_DEBUG_OPTS SSL_DISPLAY_STATES
#else
#define SSL_DEBUG_OPTS 0
#endif

uint8_t* default_private_key = 0;
uint32_t default_private_key_len = 0;
static bool default_private_key_dynamic = false;
static int s_pk_refcnt = 0;
uint8_t* default_certificate = 0;
uint32_t default_certificate_len = 0;
static bool default_certificate_dynamic = false;

static void clear_private_key();
static void clear_certificate();


class SSLContext {
public:
    SSLContext() {
        if (_ssl_ctx_refcnt == 0) {
            _ssl_ctx = ssl_ctx_new(SSL_SERVER_VERIFY_LATER | SSL_DEBUG_OPTS, 0);
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

    int read(uint8_t* dst, size_t size) {
        if (!_available) {
            if (!_readAll())
                return 0;
        }
        size_t will_copy = (_available < size) ? _available : size;
        memcpy(dst, _read_ptr, will_copy);
        _read_ptr += will_copy;
        _available -= will_copy;
        if (_available == 0) {
            _read_ptr = nullptr;
        }
        return will_copy;
    }

    int read() {
        if (!_available) {
            if (!_readAll())
                return -1;
        }
        int result = _read_ptr[0];
        ++_read_ptr;
        --_available;
        if (_available == 0) {
            _read_ptr = nullptr;
        }
        return result;
    }

    int peek() {
        if (!_available) {
            if (!_readAll())
                return -1;
        }
        return _read_ptr[0];
    }

    size_t peekBytes(char *dst, size_t size) {
        if(!_available) {
            if(!_readAll())
                return -1;
        }

        size_t will_copy = (_available < size) ? _available : size;
        memcpy(dst, _read_ptr, will_copy);
        return will_copy;
    }

    int available() {
        auto cb = _available;
        if (cb == 0) {
            cb = _readAll();
        } else {
            optimistic_yield(100);
        }
        return cb;
    }

    operator SSL*() {
        return _ssl;
    }

protected:
    int _readAll() {
        if (!_ssl)
            return 0;

        optimistic_yield(100);

        uint8_t* data;
        int rc = ssl_read(_ssl, &data);
        if (rc <= 0) {
            if (rc < SSL_OK && rc != SSL_CLOSE_NOTIFY && rc != SSL_ERROR_CONN_LOST) {
                ssl_free(_ssl);
                _ssl = nullptr;
            }
            return 0;
        }
        DEBUGV(":wcs ra %d", rc);
        _read_ptr = data;
        _available = rc;
        return _available;
    }

    static SSL_CTX* _ssl_ctx;
    static int _ssl_ctx_refcnt;
    SSL* _ssl = nullptr;
    int _refcnt = 0;
    const uint8_t* _read_ptr = nullptr;
    size_t _available = 0;
};

SSL_CTX* SSLContext::_ssl_ctx = nullptr;
int SSLContext::_ssl_ctx_refcnt = 0;


WiFiClientSecure::WiFiClientSecure() {
    ++s_pk_refcnt;
}

WiFiClientSecure::~WiFiClientSecure() {
    if (_ssl) {
        _ssl->unref();
    }
    if (--s_pk_refcnt == 0) {
        clear_private_key();
        clear_certificate();
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
    if (!_ssl)
        return 0;

    int rc = ssl_write(*_ssl, buf, size);
    if (rc >= 0)
        return rc;

    return 0;
}

int WiFiClientSecure::read(uint8_t *buf, size_t size) {
    if (!_ssl)
        return 0;

    return _ssl->read(buf, size);
}

int WiFiClientSecure::read() {
    if (!_ssl)
        return -1;

    return _ssl->read();
}

int WiFiClientSecure::peek() {
    if (!_ssl)
        return -1;

    return _ssl->peek();
}

size_t WiFiClientSecure::peekBytes(uint8_t *buffer, size_t length) {
    size_t count = 0;

    if(!_ssl) {
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

    return _ssl->peekBytes((char *)buffer, count);
}

int WiFiClientSecure::available() {
    if (!_ssl)
        return 0;

    return _ssl->available();
}

uint8_t WiFiClientSecure::connected() {
    if (!_client)
        return 0;

    if (_client->state() == ESTABLISHED)
        return 1;

    if (!_ssl)
        return 0;

    return _ssl->available() > 0;
}

void WiFiClientSecure::stop() {
    if (_ssl) {
        _ssl->unref();
        _ssl = nullptr;
    }
    return WiFiClient::stop();
}

static bool parseHexNibble(char pb, uint8_t* res) {
    if (pb >= '0' && pb <= '9') {
        *res = (uint8_t) (pb - '0'); return true;
    }
    else if (pb >= 'a' && pb <= 'f') {
        *res = (uint8_t) (pb - 'a' + 10); return true;
    }
    else if (pb >= 'A' && pb <= 'F') {
        *res = (uint8_t) (pb - 'A' + 10); return true;
    }
    return false;
}

bool WiFiClientSecure::verify(const char* fp, const char* url) {
    if (!_ssl)
        return false;

    uint8_t sha1[20];
    int len = strlen(fp);
    int pos = 0;
    for (size_t i = 0; i < sizeof(sha1); ++i) {
        while (pos < len && ((fp[pos] == ' ') || (fp[pos] == ':'))) {
            ++pos;
        }
        if (pos > len - 2) {
            DEBUGV("pos:%d len:%d fingerprint too short\r\n", pos, len);
            return false;
        }
        uint8_t high, low;
        if (!parseHexNibble(fp[pos], &high) || !parseHexNibble(fp[pos+1], &low)) {
            DEBUGV("pos:%d len:%d invalid hex sequence: %c%c\r\n", pos, len, fp[pos], fp[pos+1]);
            return false;
        }
        pos += 2;
        sha1[i] = low | (high << 4);
    }
    if (ssl_match_fingerprint(*_ssl, sha1) != 0) {
        DEBUGV("fingerprint doesn't match\r\n");
        return false;
    }

    //TODO: check URL against certificate

    return true;
}

void WiFiClientSecure::setCertificate(const uint8_t* cert_data, size_t size) {
  clear_certificate();
  default_certificate = (uint8_t*) cert_data;
  default_certificate_len = size;
}

void WiFiClientSecure::setPrivateKey(const uint8_t* pk, size_t size) {
  clear_private_key();
  default_private_key = (uint8_t*) pk;
  default_private_key_len = size;
}

bool WiFiClientSecure::loadCertificate(Stream& stream, size_t size) {
  clear_certificate();
  default_certificate = new uint8_t[size];
  if (!default_certificate) {
    return false;
  }
  if (stream.readBytes(default_certificate, size) != size) {
    delete[] default_certificate;
    return false;
  }
  default_certificate_dynamic = true;
  default_certificate_len = size;
  return true;
}

bool WiFiClientSecure::loadPrivateKey(Stream& stream, size_t size) {
  clear_private_key();
  default_private_key = new uint8_t[size];
  if (!default_private_key) {
    return false;
  }
  if (stream.readBytes(default_private_key, size) != size) {
    delete[] default_private_key;
    return false;
  }
  default_private_key_dynamic = true;
  default_private_key_len = size;
  return true;
}

static void clear_private_key() {
  if (default_private_key && default_private_key_dynamic) {
    delete[] default_private_key;
    default_private_key_dynamic = false;
  }
  default_private_key = 0;
  default_private_key_len = 0;
}

static void clear_certificate() {
  if (default_certificate && default_certificate_dynamic) {
    delete[] default_certificate;
    default_certificate_dynamic = false;
  }
  default_certificate = 0;
  default_certificate_len = 0;
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
        optimistic_yield(100);
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
        panic();
    }
    if (size >= 1024) {
        DEBUG_TLS_MEM_PRINT("%s:%d malloc %d, left %d\r\n", file, line, size, ESP.getFreeHeap());
    }
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
        panic();
    }
    if (size >= 1024) {
        DEBUG_TLS_MEM_PRINT("%s:%d realloc %d, left %d\r\n", file, line, size, ESP.getFreeHeap());
    }
    return result;
}

extern "C" void ax_port_free(void* ptr) {
    free(ptr);
    uint32_t *p = (uint32_t*) ptr;
    size_t size = p[-3];
    if (size >= 1024) {
        DEBUG_TLS_MEM_PRINT("free %d, left %d\r\n", p[-3], ESP.getFreeHeap());
    }
}
