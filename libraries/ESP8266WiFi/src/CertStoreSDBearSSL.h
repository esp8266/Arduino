/*
  CertStoreSDBearSSL.h - Library for Arduino ESP8266
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

#ifndef _CERTSTORESD_BEARSSL_H
#define _CERTSTORESD_BEARSSL_H

#include "CertStoreBearSSL.h"

class File;

// SD cert store can be in a subdirectory as there are fewer limits
// Note that SD.begin() MUST be called before doing initCertStore because
// there are different options for the CS and other pins you need to
// specify it in your own code.
class CertStoreSDBearSSL : public CertStoreBearSSL {
  public:
    CertStoreSDBearSSL();
    virtual ~CertStoreSDBearSSL();

    virtual int initCertStore(const char *dir = "/") override;
    virtual void installCertStore(br_x509_minimal_context *ctx) override;

  private:
    String path;
    CertInfo preprocessCert(File *f);
    // These need to be static as they are callbacks from BearSSL C code
    static const br_x509_trust_anchor *findHashedTA(void *ctx, void *hashed_dn, size_t len);
    static void freeHashedTA(void *ctx, const br_x509_trust_anchor *ta);
};

#endif
