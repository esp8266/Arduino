/*
  WiFiServerBearSSL.h - Library for Arduino ESP8266
  Copyright (c) 2018 Earle F. Philhower, III

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

#ifndef wifiserverbearssl_h
#define wifiserverbearssl_h

#include "WiFiServer.h"
#include "WiFiClientSecureBearSSL.h"
#include "BearSSLHelpers.h"
#include <bearssl/bearssl.h>

namespace BearSSL {

class WiFiClientSecure;

class WiFiServerSecure : public WiFiServer {
  public:
    WiFiServerSecure(IPAddress addr, uint16_t port);
    WiFiServerSecure(uint16_t port);
    WiFiServerSecure(const WiFiServerSecure &rhs);
    virtual ~WiFiServerSecure();

    // Override the default buffer sizes, if you know what you're doing...
    void setBufferSizes(int recv, int xmit) {
      _iobuf_in_size = recv;
      _iobuf_out_size = xmit;
    }

    // Set the server's RSA key and x509 certificate (required, pick one).
    // Caller needs to preserve the chain and key throughout the life of the server.
    void setRSACert(const X509List *chain, const PrivateKey *sk);
    // Set the server's EC key and x509 certificate (required, pick one)
    // Caller needs to preserve the chain and key throughout the life of the server.
    void setECCert(const X509List *chain, unsigned cert_issuer_key_type, const PrivateKey *sk);

    // Require client certificates validated against the passed in x509 trust anchor
    // Caller needs to preserve the cert throughout the life of the server.
    void setClientTrustAnchor(const X509List *client_CA_ta) {
      _client_CA_ta = client_CA_ta;
    }

    // If awaiting connection available and authenticated (i.e. client cert), return it.
    WiFiClientSecure available(uint8_t* status = NULL);

    // Compatibility with axTLS interface
    void setServerKeyAndCert(const uint8_t *key, int keyLen, const uint8_t *cert, int certLen);
    void setServerKeyAndCert_P(const uint8_t *key, int keyLen, const uint8_t *cert, int certLen);

  private:
    const X509List *_chain = nullptr;
    unsigned _cert_issuer_key_type = 0;
    const PrivateKey *_sk = nullptr;
    int _iobuf_in_size = BR_SSL_BUFSIZE_INPUT;
    int _iobuf_out_size = 837;
    const X509List *_client_CA_ta = nullptr;

    // axTLS compat
    std::shared_ptr<X509List>   _axtls_chain;
    std::shared_ptr<PrivateKey> _axtls_sk;

};

};

#endif
