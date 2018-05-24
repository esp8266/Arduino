/*
  CertStoreSPIFFSBearSSL.cpp - Library for Arduino ESP8266
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

#include "CertStoreSPIFFSBearSSL.h"
#include <FS.h>

CertStoreSPIFFSBearSSL::CertStoreSPIFFSBearSSL() : CertStoreBearSSL() {
}

CertStoreSPIFFSBearSSL::~CertStoreSPIFFSBearSSL() {
}

CertStoreBearSSL::CertInfo CertStoreSPIFFSBearSSL::preprocessCert(const char *fname) {
  CertStoreBearSSL::CertInfo ci;
  memset(&ci, 0, sizeof(ci));

  // Load the DER into RAM temporarially
  File f = SPIFFS.open(fname, "r");
  if (!f) {
    return ci;
  }
  int sz = f.size();
  uint8_t *buf = new uint8_t[sz];
  if (!buf) {
    f.close();
    return ci;
  }
  f.read(buf, sz);
  f.close();

  ci = CertStoreBearSSL::preprocessCert(fname, buf, sz);

  delete[] buf;

  return ci;
}

int CertStoreSPIFFSBearSSL::initCertStore(const char *subdir) {
  (void) subdir; // ignored prefix, not enough space in filenames
  int count = 0;
  SPIFFS.begin();
  File tbl = SPIFFS.open("/ca_tbl.bin", "w");
  if (!tbl) {
    return 0;
  }
  Dir d = SPIFFS.openDir("");
  while (d.next()) {
    if (!strstr(d.fileName().c_str(), ".der")) {
      continue;
    }
    CertStoreBearSSL::CertInfo ci = preprocessCert(d.fileName().c_str());
    tbl.write((uint8_t*)&ci, sizeof(ci));
    count++;
  }
  tbl.close();
  return count;
}

void CertStoreSPIFFSBearSSL::installCertStore(br_x509_minimal_context *ctx) {
  br_x509_minimal_set_dynamic(ctx, /* no context needed */nullptr, findHashedTA, freeHashedTA);
}

const br_x509_trust_anchor *CertStoreSPIFFSBearSSL::findHashedTA(void *ctx, void *hashed_dn, size_t len) {
  (void) ctx; // not needed
  CertInfo ci;

  if (len != sizeof(ci.sha256) || !SPIFFS.exists("/ca_tbl.bin")) {
    return nullptr;
  }

  File f = SPIFFS.open("/ca_tbl.bin", "r");
  if (!f) {
    return nullptr;
  }
  while (f.read((uint8_t*)&ci, sizeof(ci)) == sizeof(ci)) {
    if (!memcmp(ci.sha256, hashed_dn, sizeof(ci.sha256))) {
      // This could be the one!
      f.close();
      File d = SPIFFS.open(ci.fname, "r");
      if (!d) {
        return nullptr;
      }
      size_t der_len = d.size();
      uint8_t *der = (uint8_t*)malloc(der_len);
      if (!der) {
        d.close();
        return nullptr;
      }
      if (d.read(der, der_len) != der_len) {
        d.close();
        free(der);
        return nullptr;
      }
      d.close();

      br_x509_trust_anchor *ta = CertStoreBearSSL::makeTrustAnchor(der, der_len, &ci);
      free(der);

      return ta;
    }
  }
  f.close();
  return nullptr;
}

void CertStoreSPIFFSBearSSL::freeHashedTA(void *ctx, const br_x509_trust_anchor *ta) {
  (void) ctx; // not needed
  CertStoreBearSSL::freeTrustAnchor(ta);
}
