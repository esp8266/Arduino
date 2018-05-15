/*
  WiFiClientBearSSL- SSL client/server for esp8266 using BearSSL libraries
  - Mostly compatible with Arduino WiFi shield library and standard
    WiFiClient/ServerSecure (except for certificate handling).

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

#ifndef _BEARSSLHELPERS_H
#define _BEARSSLHELPERS_H

#include <bearssl/bearssl.h>

// Internal opaque structures, not needed by user applications
namespace brssl {
  class public_key;
  class private_key;
};

// Holds either a single public RSA or EC key for use when BearSSL wants a pubkey.
// Copies all associated data so no need to keep input PEM/DER keys.
// All inputs can be either in RAM or PROGMEM.
class BearSSLPublicKey {
  public:
    BearSSLPublicKey();
    BearSSLPublicKey(const char *pemKey);
    BearSSLPublicKey(const uint8_t *derKey, size_t derLen);
    ~BearSSLPublicKey();

    bool parse(const char *pemKey);
    bool parse(const uint8_t *derKey, size_t derLen);

    // Accessors for internal use, not needed by apps
    bool isRSA() const;
    bool isEC() const;
    const br_rsa_public_key *getRSA() const;
    const br_ec_public_key *getEC() const;

    // Disable the copy constructor, we're pointer based
    BearSSLPublicKey(const BearSSLPublicKey& that) = delete;

  private:
    brssl::public_key *_key;
};

// Holds either a single private RSA or EC key for use when BearSSL wants a secretkey.
// Copies all associated data so no need to keep input PEM/DER keys.
// All inputs can be either in RAM or PROGMEM.
class BearSSLPrivateKey {
  public:
    BearSSLPrivateKey();
    BearSSLPrivateKey(const char *pemKey);
    BearSSLPrivateKey(const uint8_t *derKey, size_t derLen);
    ~BearSSLPrivateKey();

    bool parse(const char *pemKey);
    bool parse(const uint8_t *derKey, size_t derLen);

    // Accessors for internal use, not needed by apps
    bool isRSA() const;
    bool isEC() const;
    const br_rsa_private_key *getRSA() const;
    const br_ec_private_key *getEC() const;

    // Disable the copy constructor, we're pointer based
    BearSSLPrivateKey(const BearSSLPrivateKey& that) = delete;

  private:
    brssl::private_key *_key;
};

// Holds one or more X.509 certificates and associated trust anchors for
// use whenever BearSSL needs a cert or TA.  May want to have multiple
// certs for things like a series of trusted CAs (but check the CertStore class
// for a more memory efficient way).
// Copies all associated data so no need to keep input PEM/DER certs.
// All inputs can be either in RAM or PROGMEM.
class BearSSLX509List {
  public:
    BearSSLX509List();
    BearSSLX509List(const char *pemCert);
    BearSSLX509List(const uint8_t *derCert, size_t derLen);
    ~BearSSLX509List();

    bool append(const char *pemCert);
    bool append(const uint8_t *derCert, size_t derLen);

    // Accessors
    size_t getCount() const {
      return _count;
    }
    const br_x509_certificate *getX509Certs() const {
      return _cert;
    }
    const br_x509_trust_anchor *getTrustAnchors() const {
      return _ta;
    }

    // Disable the copy constructor, we're pointer based
    BearSSLX509List(const BearSSLX509List& that) = delete;

  private:
    size_t _count;
    br_x509_certificate *_cert;
    br_x509_trust_anchor *_ta;
};

#endif
