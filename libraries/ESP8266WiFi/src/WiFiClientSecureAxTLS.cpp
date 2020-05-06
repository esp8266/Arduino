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

#include "debug.h"
#include "ESP8266WiFi.h"
#include "WiFiClientSecure.h"
#include "WiFiClient.h"

#ifdef DEBUG_ESP_SSL
#define DEBUG_SSL
#endif

#ifdef DEBUG_SSL
#define SSL_DEBUG_OPTS (SSL_DISPLAY_STATES | SSL_DISPLAY_CERTS)
#else
#define SSL_DEBUG_OPTS 0
#endif


// Pull in required classes
#include "include/SSLContext.h"
#include "include/ClientContext.h"

namespace axTLS {

SSL_CTX* SSLContext::_ssl_client_ctx = nullptr;
int SSLContext::_ssl_client_ctx_refcnt = 0;
SSL_CTX* SSLContext::_ssl_svr_ctx = nullptr;
int SSLContext::_ssl_svr_ctx_refcnt = 0;

WiFiClientSecure::WiFiClientSecure()
{
    // TLS handshake may take more than the 5 second default timeout
    _timeout = 15000;
}

WiFiClientSecure::~WiFiClientSecure()
{
   _ssl = nullptr;
}

// Only called by the WifiServerSecure, need to get the keys/certs loaded before beginning
WiFiClientSecure::WiFiClientSecure(ClientContext* client, bool usePMEM,
                                   const uint8_t *rsakey, int rsakeyLen,
                                   const uint8_t *cert, int certLen)
{
    // TLS handshake may take more than the 5 second default timeout
    _timeout = 15000;

    // We've been given the client context from the available() call
    _client = client;
    _client->ref();

    // Make the "_ssl" SSLContext, in the constructor there should be none yet
    SSLContext *_new_ssl = new SSLContext(true);
    std::shared_ptr<SSLContext> _new_ssl_shared(_new_ssl);
    _ssl = _new_ssl_shared;

    if (usePMEM) {
        if (rsakey && rsakeyLen) {
            _ssl->loadObject_P(SSL_OBJ_RSA_KEY, rsakey, rsakeyLen);
        }
        if (cert && certLen) {
            _ssl->loadObject_P(SSL_OBJ_X509_CERT, cert, certLen);
        }
    } else {
        if (rsakey && rsakeyLen) {
            _ssl->loadObject(SSL_OBJ_RSA_KEY, rsakey, rsakeyLen);
        }
        if (cert && certLen) {
            _ssl->loadObject(SSL_OBJ_X509_CERT, cert, certLen);
        }
    }
    _ssl->connectServer(client, _timeout);
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

int WiFiClientSecure::connect(const String& host, uint16_t port)
{
    return connect(host.c_str(), port);
}

int WiFiClientSecure::_connectSSL(const char* hostName)
{
    if (!_ssl) {
        _ssl = std::make_shared<SSLContext>();
	if (!_ssl) {
          return 0;
        }
    }
    _ssl->connect(_client, hostName, _timeout);

    auto status = ssl_handshake_status(*_ssl);
    if (status != SSL_OK) {
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

    int rc = _ssl->write(buf, size);
    if (rc >= 0) {
        return rc;
    }

    if (rc != SSL_CLOSE_NOTIFY) {
        _ssl = nullptr;
    }

    return 0;
}

size_t WiFiClientSecure::write_P(PGM_P buf, size_t size)
{
    // Copy to RAM and call normal send. alloca() auto-frees on return
    uint8_t *copy = (uint8_t*)alloca(size);
    memcpy_P(copy, buf, size);
    return write(copy, size);
}

// The axTLS bare libs don't understand anything about Arduino Streams,
// so we have to manually read and send individual chunks.
size_t WiFiClientSecure::write(Stream& stream)
{
    size_t totalSent = 0;
    size_t countRead;
    size_t countSent;
    if (!_ssl) {
        return 0;
    }
    do {
        uint8_t temp[256]; // Temporary chunk size same as ClientContext
        countSent = 0;
        countRead = stream.readBytes(temp, sizeof(temp));
        if (countRead) {
            countSent = write(temp, countRead);
            totalSent += countSent;
        }
        yield(); // Feed the WDT
    } while ( (countSent == countRead) && (countSent > 0) );
    return totalSent;
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
        if (_ssl->hasData()) {
            return true;
        }
        if (_client && _client->state() == ESTABLISHED && _ssl->connected()) {
            return true;
        }
    }
    return false;
}

bool WiFiClientSecure::stop(unsigned int maxWaitMs)
{
    if (_ssl) {
        _ssl->stop();
    }
    return WiFiClient::stop(maxWaitMs);
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

    const char* san = nullptr;
    int i = 0;
    while ((san = ssl_get_cert_subject_alt_dnsname(*_ssl, i)) != nullptr) {
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
        _ssl = std::make_shared<SSLContext>();
    }
}

bool WiFiClientSecure::setCACert(const uint8_t* pk, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject(SSL_OBJ_X509_CACERT, pk, size) : false;
}

bool WiFiClientSecure::setCertificate(const uint8_t* pk, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject(SSL_OBJ_X509_CERT, pk, size) : false;
}

bool WiFiClientSecure::setPrivateKey(const uint8_t* pk, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject(SSL_OBJ_RSA_KEY, pk, size) : false;
}

bool WiFiClientSecure::setCACert_P(PGM_VOID_P pk, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject_P(SSL_OBJ_X509_CACERT, pk, size) : false;
}

bool WiFiClientSecure::setCertificate_P(PGM_VOID_P pk, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject_P(SSL_OBJ_X509_CERT, pk, size) : false;
}

bool WiFiClientSecure::setPrivateKey_P(PGM_VOID_P pk, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject_P(SSL_OBJ_RSA_KEY, pk, size) : false;
}

bool WiFiClientSecure::loadCACert(Stream& stream, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject(SSL_OBJ_X509_CACERT, stream, size) : false;
}

bool WiFiClientSecure::loadCertificate(Stream& stream, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject(SSL_OBJ_X509_CERT, stream, size) : false;
}

bool WiFiClientSecure::loadPrivateKey(Stream& stream, size_t size)
{
    _initSSLContext();
    return _ssl ? _ssl->loadObject(SSL_OBJ_RSA_KEY, stream, size) : false;
}

void WiFiClientSecure::allowSelfSignedCerts()
{
    _initSSLContext();
    if (_ssl) {
        _ssl->allowSelfSignedCerts();
    }
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
extern "C" int ax_port_read(int fd, uint8_t* buffer, size_t count) __attribute__ ((weak, alias("__ax_port_read")));

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
extern "C" int ax_port_write(int fd, uint8_t* buffer, size_t count) __attribute__ ((weak, alias("__ax_port_write")));

extern "C" int __ax_get_file(const char *filename, uint8_t **buf)
{
    (void) filename;
    *buf = 0;
    return 0;
}
extern "C" int ax_get_file(const char *filename, uint8_t **buf) __attribute__ ((weak, alias("__ax_get_file")));

extern "C" void __ax_wdt_feed()
{
    optimistic_yield(10000);
}
extern "C" void ax_wdt_feed() __attribute__ ((weak, alias("__ax_wdt_feed")));

};
