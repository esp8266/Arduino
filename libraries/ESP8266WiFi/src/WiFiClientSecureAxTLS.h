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


namespace axTLS {

class SSLContext;

class WiFiClientSecure : public WiFiClient {
public:
  WiFiClientSecure() __attribute__((deprecated("Upgrade to BearSSL is advised, check https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecure.h#L25-L99")));
  ~WiFiClientSecure() override;

  int connect(IPAddress ip, uint16_t port) override;
  int connect(const String& host, uint16_t port) override;
  int connect(const char* name, uint16_t port) override;

  bool verify(const char* fingerprint, const char* domain_name);
  bool verifyCertChain(const char* domain_name);

  uint8_t connected() override;
  size_t write(const uint8_t *buf, size_t size) override;
  size_t write_P(PGM_P buf, size_t size) override;
  size_t write(Stream& stream); // Note this is not virtual
  int read(uint8_t *buf, size_t size) override;
  int available() override;
  int read() override;
  int peek() override;
  size_t peekBytes(uint8_t *buffer, size_t length) override;
  void stop() override { (void)stop(0); }
  bool stop(unsigned int maxWaitMs);

  bool setCACert(const uint8_t* pk, size_t size);
  bool setCertificate(const uint8_t* pk, size_t size);
  bool setPrivateKey(const uint8_t* pk, size_t size);

  bool setCACert_P(PGM_VOID_P pk, size_t size);
  bool setCertificate_P(PGM_VOID_P pk, size_t size);
  bool setPrivateKey_P(PGM_VOID_P pk, size_t size);

  bool loadCACert(Stream& stream, size_t size);
  bool loadCertificate(Stream& stream, size_t size);
  bool loadPrivateKey(Stream& stream, size_t size);

  void allowSelfSignedCerts();

  template<typename TFile>
  bool loadCertificate(TFile& file) {
    return loadCertificate(file, file.size());
  }

  template<typename TFile>
  bool loadPrivateKey(TFile& file) {
    return loadPrivateKey(file, file.size());
  }
  
  template<typename TFile>
  bool loadCACert(TFile& file) {
    return loadCACert(file, file.size());
  }

  // disallow buffered peek API
  virtual bool peekBufferAPI () const override { return false; }

friend class WiFiServerSecure; // Needs access to custom constructor below
protected:
  // Only called by WiFiServerSecure
  WiFiClientSecure(ClientContext* client, bool usePMEM, const uint8_t *rsakey, int rsakeyLen, const uint8_t *cert, int certLen);

protected:
    void _initSSLContext();
    int _connectSSL(const char* hostName);
    bool _verifyDN(const char* name);

    std::shared_ptr<SSLContext> _ssl = nullptr;
};

};

#endif //wificlientsecure_h
