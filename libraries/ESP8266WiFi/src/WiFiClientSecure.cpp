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
#define SSL_DEBUG_OPTS (SSL_DISPLAY_STATES | SSL_DISPLAY_CERTS)
#else
#define SSL_DEBUG_OPTS 0
#endif

class SSLContext
{
public:
    SSLContext()
    {
        if (_ssl_ctx_refcnt == 0) {
            _ssl_ctx = ssl_ctx_new(SSL_SERVER_VERIFY_LATER | SSL_DEBUG_OPTS | SSL_CONNECT_IN_PARTS | SSL_READ_BLOCKING | SSL_NO_DEFAULT_KEY, 0);
        }
        ++_ssl_ctx_refcnt;
    }

    ~SSLContext()
    {
        if (_ssl) {
            ssl_free(_ssl);
            _ssl = nullptr;
        }

        --_ssl_ctx_refcnt;
        if (_ssl_ctx_refcnt == 0) {
            ssl_ctx_free(_ssl_ctx);
        }

        s_io_ctx = nullptr;
    }

    void ref()
    {
        ++_refcnt;
    }

    void unref()
    {
        if (--_refcnt == 0) {
            delete this;
        }
    }

    void connect(ClientContext* ctx, const char* hostName, uint32_t timeout_ms)
    {
        SSL_EXTENSIONS* ext = ssl_ext_new();
        ssl_ext_set_host_name(ext, hostName);
        ssl_ext_set_max_fragment_size(ext, 4096);
        if (_ssl) {
            /* Creating a new TLS session on top of a new TCP connection.
               ssl_free will want to send a close notify alert, but the old TCP connection
               is already gone at this point, so reset s_io_ctx. */
            s_io_ctx = nullptr;
            ssl_free(_ssl);
            _available = 0;
            _read_ptr = nullptr;
        }
        s_io_ctx = ctx;
        _ssl = ssl_client_new(_ssl_ctx, 0, nullptr, 0, ext);
        uint32_t t = millis();

        while (millis() - t < timeout_ms && ssl_handshake_status(_ssl) != SSL_OK) {
            uint8_t* data;
            int rc = ssl_read(_ssl, &data);
            if (rc < SSL_OK) {
                ssl_display_error(rc);
                break;
            }
        }
    }

    void stop()
    {
        s_io_ctx = nullptr;
    }

    bool connected()
    {
        return _ssl != nullptr && ssl_handshake_status(_ssl) == SSL_OK;
    }

    int read(uint8_t* dst, size_t size)
    {
        if (!_available) {
            if (!_readAll()) {
                return 0;
            }
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

    int read()
    {
        if (!_available) {
            if (!_readAll()) {
                return -1;
            }
        }
        int result = _read_ptr[0];
        ++_read_ptr;
        --_available;
        if (_available == 0) {
            _read_ptr = nullptr;
        }
        return result;
    }

    int peek()
    {
        if (!_available) {
            if (!_readAll()) {
                return -1;
            }
        }
        return _read_ptr[0];
    }

    size_t peekBytes(char *dst, size_t size)
    {
        if (!_available) {
            if (!_readAll()) {
                return -1;
            }
        }

        size_t will_copy = (_available < size) ? _available : size;
        memcpy(dst, _read_ptr, will_copy);
        return will_copy;
    }

    int available()
    {
        auto cb = _available;
        if (cb == 0) {
            cb = _readAll();
        } else {
            optimistic_yield(100);
        }
        return cb;
    }

    bool loadObject(int type, Stream& stream, size_t size)
    {
        std::unique_ptr<uint8_t[]> buf(new uint8_t[size]);
        if (!buf.get()) {
            DEBUGV("loadObject: failed to allocate memory\n");
            return false;
        }

        size_t cb = stream.readBytes(buf.get(), size);
        if (cb != size) {
            DEBUGV("loadObject: reading %u bytes, got %u\n", size, cb);
            return false;
        }

        return loadObject(type, buf.get(), size);
    }

    bool loadObject_P(int type, PGM_VOID_P data, size_t size)
    {
        std::unique_ptr<uint8_t[]> buf(new uint8_t[size]);
        memcpy_P(buf.get(),data, size);
        return loadObject(type, buf.get(), size);
    }


    bool loadObject(int type, const uint8_t* data, size_t size)
    {
        int rc = ssl_obj_memory_load(_ssl_ctx, type, data, static_cast<int>(size), nullptr);
        if (rc != SSL_OK) {
            DEBUGV("loadObject: ssl_obj_memory_load returned %d\n", rc);
            return false;
        }
        return true;
    }

    bool verifyCert()
    {
        int rc = ssl_verify_cert(_ssl);
        if (_allowSelfSignedCerts && rc == SSL_X509_ERROR(X509_VFY_ERROR_SELF_SIGNED)) {
            DEBUGV("Allowing self-signed certificate\n");
            return true;
        } else if (rc != SSL_OK) {
            DEBUGV("ssl_verify_cert returned %d\n", rc);
            ssl_display_error(rc);
            return false;
        }
        return true;
    }

    void allowSelfSignedCerts()
    {
        _allowSelfSignedCerts = true;
    }

    operator SSL*()
    {
        return _ssl;
    }

    static ClientContext* getIOContext(int fd)
    {
        (void) fd;
        return s_io_ctx;
    }

protected:
    int _readAll()
    {
        if (!_ssl) {
            return 0;
        }

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
        DEBUGV(":wcs ra %d\r\n", rc);
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
    bool _allowSelfSignedCerts = false;
    static ClientContext* s_io_ctx;
};

SSL_CTX* SSLContext::_ssl_ctx = nullptr;
int SSLContext::_ssl_ctx_refcnt = 0;
ClientContext* SSLContext::s_io_ctx = nullptr;

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

WiFiClientSecure& WiFiClientSecure::operator=(const WiFiClientSecure& rhs)
{
    (WiFiClient&) *this = rhs;
    _ssl = rhs._ssl;
    if (_ssl) {
        _ssl->ref();
    }
    return *this;
}

int WiFiClientSecure::connect(IPAddress ip, uint16_t port)
{
    if (!WiFiClient::connect(ip, port)) {
        return 0;
    }

    return _connectSSL(nullptr);
}

int WiFiClientSecure::connect(const char* name, uint16_t port)
{
    IPAddress remote_addr;
    if (!WiFi.hostByName(name, remote_addr)) {
        return 0;
    }
    if (!WiFiClient::connect(remote_addr, port)) {
        return 0;
    }
    return _connectSSL(name);
}

int WiFiClientSecure::_connectSSL(const char* hostName)
{
    if (!_ssl) {
        _ssl = new SSLContext;
        _ssl->ref();
    }
    _ssl->connect(_client, hostName, 5000);

    auto status = ssl_handshake_status(*_ssl);
    if (status != SSL_OK) {
        _ssl->unref();
        _ssl = nullptr;
        return 0;
    }

    return 1;
}

size_t WiFiClientSecure::write(const uint8_t *buf, size_t size)
{
    if (!_ssl) {
        return 0;
    }

    int rc = ssl_write(*_ssl, buf, size);
    if (rc >= 0) {
        return rc;
    }

    if (rc != SSL_CLOSE_NOTIFY) {
        _ssl->unref();
        _ssl = nullptr;
    }

    return 0;
}

int WiFiClientSecure::read(uint8_t *buf, size_t size)
{
    if (!_ssl) {
        return 0;
    }

    return _ssl->read(buf, size);
}

int WiFiClientSecure::read()
{
    if (!_ssl) {
        return -1;
    }

    return _ssl->read();
}

int WiFiClientSecure::peek()
{
    if (!_ssl) {
        return -1;
    }

    return _ssl->peek();
}

size_t WiFiClientSecure::peekBytes(uint8_t *buffer, size_t length)
{
    size_t count = 0;

    if (!_ssl) {
        return 0;
    }

    _startMillis = millis();
    while ((available() < (int) length) && ((millis() - _startMillis) < _timeout)) {
        yield();
    }

    if (!_ssl) {
        return 0;
    }

    if (available() < (int) length) {
        count = available();
    } else {
        count = length;
    }

    return _ssl->peekBytes((char *)buffer, count);
}

int WiFiClientSecure::available()
{
    if (!_ssl) {
        return 0;
    }

    return _ssl->available();
}


/*
SSL     TCP     RX data     connected
null    x       x           N
!null   x       Y           Y
Y       Y       x           Y
x       N       N           N
err     x       N           N
*/
uint8_t WiFiClientSecure::connected()
{
    if (_ssl) {
        if (_ssl->available()) {
            return true;
        }
        if (_client && _client->state() == ESTABLISHED && _ssl->connected()) {
            return true;
        }
    }
    return false;
}

void WiFiClientSecure::stop()
{
    if (_ssl) {
        _ssl->stop();
    }
    WiFiClient::stop();
}

static bool parseHexNibble(char pb, uint8_t* res)
{
    if (pb >= '0' && pb <= '9') {
        *res = (uint8_t) (pb - '0'); return true;
    } else if (pb >= 'a' && pb <= 'f') {
        *res = (uint8_t) (pb - 'a' + 10); return true;
    } else if (pb >= 'A' && pb <= 'F') {
        *res = (uint8_t) (pb - 'A' + 10); return true;
    }
    return false;
}

// Compare a name from certificate and domain name, return true if they match
static bool matchName(const String& name, const String& domainName)
{
    int wildcardPos = name.indexOf('*');
    if (wildcardPos == -1) {
        // Not a wildcard, expect an exact match
        return name == domainName;
    }
    int firstDotPos = name.indexOf('.');
    if (wildcardPos > firstDotPos) {
        // Wildcard is not part of leftmost component of domain name
        // Do not attempt to match (rfc6125 6.4.3.1)
        return false;
    }
    if (wildcardPos != 0 || firstDotPos != 1) {
        // Matching of wildcards such as baz*.example.com and b*z.example.com
        // is optional. Maybe implement this in the future?
        return false;
    }
    int domainNameFirstDotPos = domainName.indexOf('.');
    if (domainNameFirstDotPos < 0) {
        return false;
    }
    return domainName.substring(domainNameFirstDotPos) == name.substring(firstDotPos);
}

bool WiFiClientSecure::verify(const char* fp, const char* domain_name)
{
    if (!_ssl) {
        return false;
    }

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

    return _verifyDN(domain_name);
}

bool WiFiClientSecure::_verifyDN(const char* domain_name)
{
    DEBUGV("domain name: '%s'\r\n", (domain_name)?domain_name:"(null)");
    String domain_name_str(domain_name);
    domain_name_str.toLowerCase();

    const char* san = NULL;
    int i = 0;
    while ((san = ssl_get_cert_subject_alt_dnsname(*_ssl, i)) != NULL) {
        String san_str(san);
        san_str.toLowerCase();
        if (matchName(san_str, domain_name_str)) {
            return true;
        }
        DEBUGV("SAN %d: '%s', no match\r\n", i, san);
        ++i;
    }
    const char* common_name = ssl_get_cert_dn(*_ssl, SSL_X509_CERT_COMMON_NAME);
    String common_name_str(common_name);
    common_name_str.toLowerCase();
    if (common_name && matchName(common_name_str, domain_name_str)) {
        return true;
    }
    DEBUGV("CN: '%s', no match\r\n", (common_name)?common_name:"(null)");

    return false;
}

bool WiFiClientSecure::verifyCertChain(const char* domain_name)
{
    if (!_ssl) {
        return false;
    }
    if (!_ssl->verifyCert()) {
        return false;
    }
    return _verifyDN(domain_name);
}

void WiFiClientSecure::_initSSLContext()
{
    if (!_ssl) {
        _ssl = new SSLContext;
        _ssl->ref();
    }
}

bool WiFiClientSecure::setCACert(const uint8_t* pk, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject(SSL_OBJ_X509_CACERT, pk, size);
}

bool WiFiClientSecure::setCertificate(const uint8_t* pk, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject(SSL_OBJ_X509_CERT, pk, size);
}

bool WiFiClientSecure::setPrivateKey(const uint8_t* pk, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject(SSL_OBJ_RSA_KEY, pk, size);
}

bool WiFiClientSecure::setCACert_P(PGM_VOID_P pk, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject_P(SSL_OBJ_X509_CACERT, pk, size);
}

bool WiFiClientSecure::setCertificate_P(PGM_VOID_P pk, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject_P(SSL_OBJ_X509_CERT, pk, size);
}

bool WiFiClientSecure::setPrivateKey_P(PGM_VOID_P pk, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject_P(SSL_OBJ_RSA_KEY, pk, size);
}

bool WiFiClientSecure::loadCACert(Stream& stream, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject(SSL_OBJ_X509_CACERT, stream, size);
}

bool WiFiClientSecure::loadCertificate(Stream& stream, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject(SSL_OBJ_X509_CERT, stream, size);
}

bool WiFiClientSecure::loadPrivateKey(Stream& stream, size_t size)
{
    _initSSLContext();
    return _ssl->loadObject(SSL_OBJ_RSA_KEY, stream, size);
}

void WiFiClientSecure::allowSelfSignedCerts()
{
    _initSSLContext();
    _ssl->allowSelfSignedCerts();
}

extern "C" int __ax_port_read(int fd, uint8_t* buffer, size_t count)
{
    ClientContext* _client = SSLContext::getIOContext(fd);
    if (!_client || (_client->state() != ESTABLISHED && !_client->getSize())) {
        errno = EIO;
        return -1;
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
extern "C" void ax_port_read() __attribute__ ((weak, alias("__ax_port_read")));

extern "C" int __ax_port_write(int fd, uint8_t* buffer, size_t count)
{
    ClientContext* _client = SSLContext::getIOContext(fd);
    if (!_client || _client->state() != ESTABLISHED) {
        errno = EIO;
        return -1;
    }

    size_t cb = _client->write(buffer, count);
    if (cb != count) {
        errno = EAGAIN;
    }
    return cb;
}
extern "C" void ax_port_write() __attribute__ ((weak, alias("__ax_port_write")));

extern "C" int __ax_get_file(const char *filename, uint8_t **buf)
{
    (void) filename;
    *buf = 0;
    return 0;
}
extern "C" void ax_get_file() __attribute__ ((weak, alias("__ax_get_file")));

extern "C" void __ax_wdt_feed()
{
    optimistic_yield(10000);
}
extern "C" void ax_wdt_feed() __attribute__ ((weak, alias("__ax_wdt_feed")));
