/*
  CertStoreSDBearSSL.cpp - Library for Arduino ESP8266
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

#include <SD.h>
#include "CertStoreSDBearSSL.h"

CertStoreSDBearSSL::CertStoreSDBearSSL() : CertStoreBearSSL() {
  path = "";
}

CertStoreSDBearSSL::~CertStoreSDBearSSL() {
}

CertStoreBearSSL::CertInfo CertStoreSDBearSSL::preprocessCert(File *f) {
  CertStoreBearSSL::CertInfo ci;
  memset(&ci, 0, sizeof(ci));

  // Load the DER into RAM temporarially
  if (!f) {
    return ci;
  }

  int sz = f->size();
  uint8_t *buf = new uint8_t[sz];
  if (!buf) {
    return ci;
  }
  f->read(buf, sz);

  ci = CertStoreBearSSL::preprocessCert(f->name(), buf, sz);

  delete buf;

  return ci;
}

int CertStoreSDBearSSL::initCertStore(const char *subdir) {
  int count = 0;

  // We want path to have a leading slash and a trailing one
  path = subdir;
  if (path[0] != '/') {
    path = "/" + path;
  }
  if (!path.endsWith("/")) {
    path += "/";
  }

  String tblName = path + "ca_tbl.bin";

  File tbl = SD.open(tblName, FILE_WRITE);
  if (!tbl) {
    return 0;
  }
  File d = SD.open(path);
  while (true) {
    File nextFile = d.openNextFile();
    if (!nextFile) {
      break;
    }
    if (!strstr(nextFile.name(), ".der")) {
      continue;
    }
    CertStoreBearSSL::CertInfo ci = preprocessCert(&nextFile);
    nextFile.close();
    tbl.write((uint8_t*)&ci, sizeof(ci));
    count++;
  }
  tbl.close();
  return count;
}

void CertStoreSDBearSSL::installCertStore(br_x509_minimal_context *ctx) {
  br_x509_minimal_set_dynamic(ctx, (void*)this, findHashedTA, freeHashedTA);
}

const br_x509_trust_anchor *CertStoreSDBearSSL::findHashedTA(void *ctx, void *hashed_dn, size_t len) {
  CertStoreSDBearSSL *cs = static_cast<CertStoreSDBearSSL*>(ctx);
  CertInfo ci;

  String tblName = cs->path + "ca_tbl.bin";

  if (len != sizeof(ci.sha256) || !SD.exists(tblName)) {
    return nullptr;
  }

  File f = SD.open(tblName, FILE_READ);
  if (!f) {
    return nullptr;
  }
  while (f.read((uint8_t*)&ci, sizeof(ci)) == sizeof(ci)) {
    if (!memcmp(ci.sha256, hashed_dn, sizeof(ci.sha256))) {
      // This could be the one!
      f.close();
      File d = SD.open(ci.fname, FILE_READ);
      if (!d) {
        return nullptr;
      }
      size_t der_len = d.size();
      uint8_t *der = (uint8_t*)malloc(der_len);
      if (!der) {
        d.close();
        return nullptr;
      }
      if (d.read(der, der_len) != (int)der_len) {
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

void CertStoreSDBearSSL::freeHashedTA(void *ctx, const br_x509_trust_anchor *ta) {
  (void) ctx; // not needed
  CertStoreBearSSL::freeTrustAnchor(ta);
}
