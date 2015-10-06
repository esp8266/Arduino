/*
  WiFiClientSecure.h - Variant of WiFiClient with TLS support
  Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.


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

#ifndef wificlientsecure_h
#define wificlientsecure_h
#include "WiFiClient.h"
#include "include/ssl.h"


class SSLContext;

class WiFiClientSecure : public WiFiClient {
public:
  WiFiClientSecure();
  ~WiFiClientSecure() override;
  WiFiClientSecure(const WiFiClientSecure&);
  WiFiClientSecure& operator=(const WiFiClientSecure&);

  int connect(IPAddress ip, uint16_t port) override;
  int connect(const char* name, uint16_t port) override;

  bool verify(const char* fingerprint, const char* url);

  uint8_t connected() override;
  size_t write(const uint8_t *buf, size_t size) override;
  int read(uint8_t *buf, size_t size) override;
  int available() override;
  int read() override;
  int peek() override;
  void stop() override;

protected:
    int _connectSSL();

    SSLContext* _ssl = nullptr;
};

#endif //wificlientsecure_h
