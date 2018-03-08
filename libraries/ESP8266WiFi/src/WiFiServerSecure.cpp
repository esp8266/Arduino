/*
  WiFiServerSecure.cpp - SSL  server for esp8266, mostly compatible
                   with Arduino WiFi shield library

  Copyright (c) 2017 Earle F. Philhower, III

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

#include "debug.h"
#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "lwip/opt.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "include/ClientContext.h"
#include "WiFiServerSecure.h"

WiFiServerSecure::WiFiServerSecure(IPAddress addr, uint16_t port) : WiFiServer(addr, port)
{
}

WiFiServerSecure::WiFiServerSecure(uint16_t port) : WiFiServer(port)
{
}

void WiFiServerSecure::setServerKeyAndCert(const uint8_t *key, int keyLen, const uint8_t *cert, int certLen)
{
    this->usePMEM = false;
    this->rsakey = key;
    this->rsakeyLen = keyLen;
    this->cert = cert;
    this->certLen = certLen;
}

void WiFiServerSecure::setServerKeyAndCert_P(const uint8_t *key, int keyLen, const uint8_t *cert, int certLen)
{
    this->usePMEM = true;
    this->rsakey = key;
    this->rsakeyLen = keyLen;
    this->cert = cert;
    this->certLen = certLen;
}

WiFiClientSecure WiFiServerSecure::available(uint8_t* status)
{
    (void) status; // Unused
    if (_unclaimed) {
        WiFiClientSecure result(_unclaimed, usePMEM, rsakey, rsakeyLen, cert, certLen);
        _unclaimed = _unclaimed->next();
        result.setNoDelay(_noDelay);
        DEBUGV("WS:av\r\n");
        return result;
    }

    optimistic_yield(1000);
    return WiFiClientSecure();
}

