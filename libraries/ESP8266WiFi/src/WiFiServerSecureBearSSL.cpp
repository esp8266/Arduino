/*
  WiFiServerBearSSL.cpp - SSL  server for esp8266, mostly compatible
                   with Arduino WiFi shield library

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

#define LWIP_INTERNAL

extern "C" {
#include "osapi.h"
#include "ets_sys.h"
}

#include <StackThunk.h>
#include "debug.h"
#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "lwip/opt.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include <include/ClientContext.h>
#include "WiFiServerSecureBearSSL.h"

namespace BearSSL {

// Only need to call the standard server constructor
WiFiServerSecure::WiFiServerSecure(IPAddress addr, uint16_t port) : WiFiServer(addr, port) {
  stack_thunk_add_ref();
}

// Only need to call the standard server constructor
WiFiServerSecure::WiFiServerSecure(uint16_t port) : WiFiServer(port) {
  stack_thunk_add_ref();
}

WiFiServerSecure::WiFiServerSecure(const WiFiServerSecure &rhs) : WiFiServer(rhs) {
  *this = rhs;
  stack_thunk_add_ref();
}

WiFiServerSecure::~WiFiServerSecure() {
  stack_thunk_del_ref();
}

// Specify a RSA-signed certificate and key for the server.  Only copies the pointer, the
// caller needs to preserve this chain and key for the life of the object.
void WiFiServerSecure::setRSACert(const X509List *chain, const PrivateKey *sk) {
  _chain = chain;
  _sk = sk;
}

// Specify a EC-signed certificate and key for the server.  Only copies the pointer, the
// caller needs to preserve this chain and key for the life of the object.
void WiFiServerSecure::setECCert(const X509List *chain, unsigned cert_issuer_key_type, const PrivateKey *sk) {
  _chain = chain;
  _cert_issuer_key_type = cert_issuer_key_type;
  _sk = sk;
}

// Return a client if there's an available connection waiting.  If one is returned,
// then any validation (i.e. client cert checking) will have succeeded.
WiFiClientSecure WiFiServerSecure::available(uint8_t* status) {
  (void) status; // Unused
  if (_unclaimed) {
    if (_sk && _sk->isRSA()) {
      WiFiClientSecure result(_unclaimed, _chain, _sk, _iobuf_in_size, _iobuf_out_size, _client_CA_ta);
      _unclaimed = _unclaimed->next();
      result.setNoDelay(_noDelay);
      DEBUGV("WS:av\r\n");
      return result;
    } else if (_sk && _sk->isEC()) {
      WiFiClientSecure result(_unclaimed, _chain, _cert_issuer_key_type, _sk, _iobuf_in_size, _iobuf_out_size, _client_CA_ta);
      _unclaimed = _unclaimed->next();
      result.setNoDelay(_noDelay);
      DEBUGV("WS:av\r\n");
      return result;
    } else {
      // No key was defined, so we can't actually accept and attempt accept() and SSL handshake.
      DEBUGV("WS:nokey\r\n");
    }
  }

  // Something weird, return a no-op object
  optimistic_yield(1000);
  return WiFiClientSecure();
}

};
