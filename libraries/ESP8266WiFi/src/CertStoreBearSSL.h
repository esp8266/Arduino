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

// Base class for the certificate stores, which allow use
// of a large set of certificates stored on SPIFFS of SD card to
// be dynamically used when validating a X509 certificate

namespace BearSSL
{

// Subclass this and provide virtual functions appropriate for your storage.
// Required because there are conflicting definitions for a "File" in the
// Arduino setup, and there is no simple way to work around the minor
// differences.
// See the examples for implementations to use in your own code.
//
// NOTE: This virtual class may migrate to a templated model in a future
// release.  Expect some changes to the interface, no matter what, as the
// SD and SPIFFS filesystem get unified.
class CertStoreFile
{
public:
    CertStoreFile() {};
    virtual ~CertStoreFile() {};

    // The main API
    virtual bool open(bool write = false) = 0;
    virtual bool seek(size_t absolute_pos) = 0;
    virtual ssize_t read(void *dest, size_t bytes) = 0;
    virtual ssize_t write(void *dest, size_t bytes) = 0;
    virtual void close() = 0;
};


class CertStore
{
public:
    CertStore() { };
    ~CertStore() { };

    // Set the file interface instances, do preprocessing
    int initCertStore(CertStoreFile *index, CertStoreFile *data);

    // Installs the cert store into the X509 decoder (normally via static function callbacks)
    void installCertStore(br_x509_minimal_context *ctx);

protected:
    CertStoreFile *_index = nullptr;
    CertStoreFile *_data = nullptr;
    X509List *_x509 = nullptr;

    // These need to be static as they are callbacks from BearSSL C code
    static const br_x509_trust_anchor *findHashedTA(void *ctx, void *hashed_dn, size_t len);
    static void freeHashedTA(void *ctx, const br_x509_trust_anchor *ta);

    // The binary format of the index file
    class CertInfo
    {
    public:
        uint8_t sha256[32];
        uint32_t offset;
        uint32_t length;
    };
    static CertInfo _preprocessCert(uint32_t length, uint32_t offset, const void *raw);

};

};

#endif

