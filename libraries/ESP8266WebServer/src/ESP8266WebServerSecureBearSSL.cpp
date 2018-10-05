/*
  ESP8266WebServerSecure.cpp - Dead simple HTTPS web-server.
  Supports only one simultaneous client, knows how to handle GET and POST.

  Copyright (c) 2017 Earle F. Philhower, III. All rights reserved.

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
  Modified 8 May 2015 by Hristo Gochkov (proper post and file upload handling)
*/


#include <Arduino.h>
#include <libb64/cencode.h>
#include "WiFiServer.h"
#include "WiFiClient.h"
#include "ESP8266WebServerSecureBearSSL.h"

//#define DEBUG_ESP_HTTP_SERVER
#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serial
#endif

namespace BearSSL {

ESP8266WebServerSecure::ESP8266WebServerSecure(IPAddress addr, int port) 
: _serverSecure(addr, port)
{
}

ESP8266WebServerSecure::ESP8266WebServerSecure(int port)
: _serverSecure(port)
{
}

void ESP8266WebServerSecure::setRSACert(const BearSSLX509List *chain, const BearSSLPrivateKey *sk)
{
  _serverSecure.setRSACert(chain, sk);
}

void ESP8266WebServerSecure::setECCert(const BearSSLX509List *chain, unsigned cert_issuer_key_type, const BearSSLPrivateKey *sk)
{
  _serverSecure.setECCert(chain, cert_issuer_key_type, sk);
}

void ESP8266WebServerSecure::setBufferSizes(int recv, int xmit)
{
  _serverSecure.setBufferSizes(recv, xmit);
}

ESP8266WebServerSecure::~ESP8266WebServerSecure() {
  // Nothing to do here.
  // Base class's destructor will be called to clean up itself
}

// We need to basically cut-n-paste these from WebServer because of the problem
// of object slicing. The class uses assignment operators like "WiFiClient x=y;"
// When this happens, even if "y" is a WiFiClientSecure, the main class is 
// already compiled down into code which will only copy the WiFiClient superclass
// and not the extra bits for our own class (since when it was compiled it needed
// to know the size of memory to allocate on the stack for this local variable
// there's not realy anything else it could do).

void ESP8266WebServerSecure::begin() {
  _currentStatus = HC_NONE;
  _serverSecure.begin();
  if(!_headerKeysCount)
    collectHeaders(0, 0);
}

void ESP8266WebServerSecure::handleClient() {
  if (_currentStatus == HC_NONE) {
    BearSSL::WiFiClientSecure client = _serverSecure.available();
    if (!client) {
      return;
    }

#ifdef DEBUG_ESP_HTTP_SERVER
    DEBUG_OUTPUT.println("New secure client");
#endif

    _currentClientSecure = client;
    _currentStatus = HC_WAIT_READ;
    _statusChange = millis();
  }

  bool keepCurrentClient = false;
  bool callYield = false;

  if (_currentClientSecure.connected()) {
    switch (_currentStatus) {
    case HC_NONE:
      // No-op to avoid C++ compiler warning
      break;
    case HC_WAIT_READ:
      // Wait for data from client to become available
      if (_currentClientSecure.available()) {
        if (_parseRequest(_currentClientSecure)) {
          _currentClientSecure.setTimeout(HTTP_MAX_SEND_WAIT);
          _contentLength = CONTENT_LENGTH_NOT_SET;
          _handleRequest();

          if (_currentClientSecure.connected()) {
            _currentStatus = HC_WAIT_CLOSE;
            _statusChange = millis();
            keepCurrentClient = true;
          }
        }
      } else { // !_currentClient.available()
        if (millis() - _statusChange <= HTTP_MAX_DATA_WAIT) {
          keepCurrentClient = true;
        }
        callYield = true;
      }
      break;
    case HC_WAIT_CLOSE:
      // Wait for client to close the connection
      if (millis() - _statusChange <= HTTP_MAX_CLOSE_WAIT) {
        keepCurrentClient = true;
        callYield = true;
      }
    }
  }

  if (!keepCurrentClient) {
    _currentClientSecure = BearSSL::WiFiClientSecure();
    _currentStatus = HC_NONE;
    _currentUpload.reset();
  }

  if (callYield) {
    yield();
  }
}

void ESP8266WebServerSecure::close() {
  _currentClientSecure.flush();
  _currentClientSecure.stop();
  _serverSecure.close();
}


void ESP8266WebServerSecure::setServerKeyAndCert_P(const uint8_t *key, int keyLen, const uint8_t *cert, int certLen) {
  _serverSecure.setServerKeyAndCert_P(key, keyLen, cert, certLen);
}

void ESP8266WebServerSecure::setServerKeyAndCert(const uint8_t *key, int keyLen, const uint8_t *cert, int certLen)
{
  _serverSecure.setServerKeyAndCert(key, keyLen, cert, certLen);
}

};
