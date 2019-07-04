/*
  WiFiServerSecure.h - Library for Arduino ESP8266
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

#ifndef wifiserversecure_h
#define wifiserversecure_h

#include "WiFiServer.h"

namespace axTLS {

class WiFiClientSecure;

class WiFiServerSecure : public WiFiServer {
public:
  WiFiServerSecure(IPAddress addr, uint16_t port);
  WiFiServerSecure(uint16_t port);
  void setServerKeyAndCert(const uint8_t *key, int keyLen, const uint8_t *cert, int certLen);
  void setServerKeyAndCert_P(const uint8_t *key, int keyLen, const uint8_t *cert, int certLen);
  virtual ~WiFiServerSecure() {}
  WiFiClientSecure available(uint8_t* status = NULL);
  using ClientType = WiFiClientSecure;

private:
  bool usePMEM = false;
  const uint8_t *rsakey = nullptr;
  int rsakeyLen = 0;
  const uint8_t *cert = nullptr;
  int certLen = 0;
};

};

#endif

