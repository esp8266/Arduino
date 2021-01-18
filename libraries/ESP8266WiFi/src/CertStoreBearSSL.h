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
#include <BearSSLHelpers.h>
#include <bearssl/bearssl.h>
#include <FS.h>

// Base class for the certificate stores, which allow use
// of a large set of certificates stored on FS or SD card to
// be dynamically used when validating a X509 certificate

namespace BearSSL {

class CertStoreBase {
  public:
    virtual ~CertStoreBase() {}

    // Installs the cert store into the X509 decoder (normally via static function callbacks)
    virtual void installCertStore(br_x509_minimal_context *ctx) = 0;
};

class CertStore: public CertStoreBase {
  public:
    CertStore() { };
    ~CertStore();

    // Set the file interface instances, do preprocessing
    int initCertStore(fs::FS &fs, const char *indexFileName, const char *dataFileName);

    // Installs the cert store into the X509 decoder (normally via static function callbacks)
    void installCertStore(br_x509_minimal_context *ctx);

  protected:
    fs::FS *_fs = nullptr;
    char *_indexName = nullptr;
    char *_dataName = nullptr;
    X509List *_x509 = nullptr;

    // These need to be static as they are callbacks from BearSSL C code
    static const br_x509_trust_anchor *findHashedTA(void *ctx, void *hashed_dn, size_t len);
    static void freeHashedTA(void *ctx, const br_x509_trust_anchor *ta);

    // The binary format of the index file
    class CertInfo {
    public:
      uint8_t sha256[32];
      uint32_t offset;
      uint32_t length;
    };
    static CertInfo _preprocessCert(uint32_t length, uint32_t offset, const void *raw);

};

};

#endif

