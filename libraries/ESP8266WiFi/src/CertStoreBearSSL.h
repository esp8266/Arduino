/*
  CertStoreBearSSL.h - Library for Arduino ESP8266
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

#ifndef _CERTSTORE_BEARSSL_H
#define _CERTSTORE_BEARSSL_H

#include <Arduino.h>
#include <bearssl/bearssl.h>

// Virtual base class for the certificate stores, which allow use
// of a large set of certificates stored on SPIFFS of SD card to
// be dynamically used when validating a X509 certificate

// Templates for child classes not possible due to the difference in SD
// and FS in terms of directory parsing and interating.  Dir doesn't
// exist in SD, everything is a file (which might support get-next-entry()
// or not).

// This class should not be instantiated directly, only via its children.
class CertStoreBearSSL {
  public:
    CertStoreBearSSL() {}
    virtual ~CertStoreBearSSL() {}

    // Preprocess the certs from the flash, returns number parsed
    virtual int initCertStore(const char *dir) = 0;

    // Installs the cert store into the X509 decoder (normally via static function callbacks)
    virtual void installCertStore(br_x509_minimal_context *ctx) = 0;

  protected:
    // The binary format of the pre-computed file
    class CertInfo {
    public:
      uint8_t sha256[32];
      char    fname[64];
    };

    CertInfo preprocessCert(const char *fname, const void *raw, size_t sz);
    static br_x509_trust_anchor *makeTrustAnchor(const void *der, size_t der_len, const CertInfo *ci);
    static void freeTrustAnchor(const br_x509_trust_anchor *ta);
};

#endif

