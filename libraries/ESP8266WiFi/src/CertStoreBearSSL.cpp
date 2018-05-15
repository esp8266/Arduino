/*
  CertStoreBearSSL.cpp - Library for Arduino ESP8266
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

#include "CertStoreBearSSL.h"
#include <memory>

extern "C" {
  // Callbacks for the x509 decoder
  static void dn_append(void *ctx, const void *buf, size_t len) {
    br_sha256_context *sha1 = (br_sha256_context*)ctx;
    br_sha256_update(sha1, buf, len);
  }
  static void dn_append_null(void *ctx, const void *buf, size_t len) {
    (void) ctx;
    (void) buf;
    (void) len;
  }
}

CertStoreBearSSL::CertInfo CertStoreBearSSL::preprocessCert(const char *fname, const void *raw, size_t sz) {
  CertStoreBearSSL::CertInfo ci;

  // Clear the CertInfo
  memset(&ci, 0, sizeof(ci));

  // Process it using SHA256, same as the hashed_dn
  br_x509_decoder_context *ctx = new br_x509_decoder_context;
  br_sha256_context *sha256 = new br_sha256_context;
  br_sha256_init(sha256);
  br_x509_decoder_init(ctx, dn_append, sha256, nullptr, nullptr);
  br_x509_decoder_push(ctx, (const void*)raw, sz);

  // Copy result to structure
  br_sha256_out(sha256, &ci.sha256);
  strcpy(ci.fname, fname);

  // Clean up allocated memory
  delete sha256;
  delete ctx;

  // Return result
  return ci;
}

br_x509_trust_anchor *CertStoreBearSSL::makeTrustAnchor(const void *der, size_t der_len, const CertInfo *ci) {
  // std::unique_ptr will free dc when we exit scope, automatically
  std::unique_ptr<br_x509_decoder_context> dc(new br_x509_decoder_context);
  br_x509_decoder_init(dc.get(), dn_append_null, nullptr, nullptr, nullptr);
  br_x509_decoder_push(dc.get(), der, der_len);
  br_x509_pkey *pk = br_x509_decoder_get_pkey(dc.get());
  if (!pk) {
    return nullptr;
  }

  br_x509_trust_anchor *ta = (br_x509_trust_anchor*)malloc(sizeof(br_x509_trust_anchor));
  if (!ta) {
    return nullptr;
  }
  memset(ta, 0, sizeof(*ta));
  ta->dn.data = (uint8_t*)malloc(sizeof(ci->sha256));
  if (!ta->dn.data) {
    free(ta);
    return nullptr;
  }
  memcpy(ta->dn.data, ci->sha256, sizeof(ci->sha256));
  ta->dn.len = sizeof(ci->sha256);

  ta->flags = 0;
  if (br_x509_decoder_isCA(dc.get())) {
    ta->flags |= BR_X509_TA_CA;
  }

  switch (pk->key_type) {
    case BR_KEYTYPE_RSA:
      ta->pkey.key_type = BR_KEYTYPE_RSA;
      ta->pkey.key.rsa.n = (uint8_t*)malloc(pk->key.rsa.nlen);
      if (!ta->pkey.key.rsa.n) {
        free(ta->dn.data);
        free(ta);
        return nullptr;
      }
      memcpy(ta->pkey.key.rsa.n, pk->key.rsa.n, pk->key.rsa.nlen);
      ta->pkey.key.rsa.nlen = pk->key.rsa.nlen;
      ta->pkey.key.rsa.e = (uint8_t*)malloc(pk->key.rsa.elen);
      if (!ta->pkey.key.rsa.e) {
        free(ta->pkey.key.rsa.n);
        free(ta->dn.data);
        free(ta);
        return nullptr;
      }
      memcpy(ta->pkey.key.rsa.e, pk->key.rsa.e, pk->key.rsa.elen);
      ta->pkey.key.rsa.elen = pk->key.rsa.elen;
      return ta;
    case BR_KEYTYPE_EC:
      ta->pkey.key_type = BR_KEYTYPE_EC;
      ta->pkey.key.ec.curve = pk->key.ec.curve;
      ta->pkey.key.ec.q = (uint8_t*)malloc(pk->key.ec.qlen);
      if (!ta->pkey.key.ec.q) {
        free(ta->dn.data);
        free(ta);
        return nullptr;
      }
      memcpy(ta->pkey.key.ec.q, pk->key.ec.q, pk->key.ec.qlen);
      ta->pkey.key.ec.qlen = pk->key.ec.qlen;
      return ta;
    default:
      free(ta->dn.data);
      free(ta);
      return nullptr;
  }
}

void CertStoreBearSSL::freeTrustAnchor(const br_x509_trust_anchor *ta) {
  switch (ta->pkey.key_type) {
    case BR_KEYTYPE_RSA:
      free(ta->pkey.key.rsa.e);
      free(ta->pkey.key.rsa.n);
      break;
    case BR_KEYTYPE_EC:
      free(ta->pkey.key.ec.q);
      break;
  }
  free(ta->dn.data);
  free((void*)ta);
}
