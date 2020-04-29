/*
 * Copyright (C) 2019 Anders Löfgren
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __ESPNOWPEERREQUESTLOG_H__
#define __ESPNOWPEERREQUESTLOG_H__

#include "EncryptedConnectionData.h"
#include "EspnowProtocolInterpreter.h"

class PeerRequestLog : public EncryptedConnectionData {
  
public:

  PeerRequestLog(const uint64_t requestID, const bool requestEncrypted, const String &authenticationPassword, const uint8_t encryptedConnectionsSoftLimit, const String &peerRequestNonce, 
                 const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint8_t hashKey[EspnowProtocolInterpreter::espnowHashKeyLength]);
  PeerRequestLog(const uint64_t requestID, const bool requestEncrypted, const String &authenticationPassword, const uint8_t encryptedConnectionsSoftLimit, const String &peerRequestNonce,
                 const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint64_t peerSessionKey, const uint64_t ownSessionKey, 
                 const uint8_t hashKey[EspnowProtocolInterpreter::espnowHashKeyLength]);

  void setRequestID(const uint64_t requestID);
  uint64_t getRequestID() const;

  void setRequestEncrypted(const bool requestEncrypted);
  bool requestEncrypted() const;

  void setAuthenticationPassword(const String &password);
  String getAuthenticationPassword() const;

  void setEncryptedConnectionsSoftLimit(const uint8_t softLimit);
  uint8_t getEncryptedConnectionsSoftLimit() const;
  
  void setPeerRequestNonce(const String &nonce);
  String getPeerRequestNonce() const;

private:

  uint64_t _requestID;
  bool _requestEncrypted;
  String _authenticationPassword;
  uint8_t _encryptedConnectionsSoftLimit;
  String _peerRequestNonce;
};

#endif
