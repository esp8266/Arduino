/*
  CertStoreSPIFFSBearSSL.h - Library for Arduino ESP8266
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

#ifndef _CERTSTORESPIFFS_BEARSSL_H
#define _CERTSTORESPIFFS_BEARSSL_H

#include "CertStoreBearSSL.h"
#include <FS.h>

// SPIFFS cert stores stored in root directory due to filename length limits
class CertStoreSPIFFSBearSSL : public CertStoreBearSSL {
  public:
    CertStoreSPIFFSBearSSL();
    virtual ~CertStoreSPIFFSBearSSL();

    virtual int initCertStore(const char *dir = "") override;  // ignores dir
    virtual void installCertStore(br_x509_minimal_context *ctx) override;

  private:
    CertInfo preprocessCert(const char *fname);
    // These need to be static as they are callbacks from BearSSL C code
    static const br_x509_trust_anchor *findHashedTA(void *ctx, void *hashed_dn, size_t len);
    static void freeHashedTA(void *ctx, const br_x509_trust_anchor *ta);
};

#endif

