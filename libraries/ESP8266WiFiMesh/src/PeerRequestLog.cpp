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

#include "PeerRequestLog.h"
#include "EspnowMeshBackend.h"

namespace
{
  using EspnowProtocolInterpreter::hashKeyLength;
}

PeerRequestLog::PeerRequestLog(const uint64_t requestID, const bool requestEncrypted, const String &authenticationPassword, const uint8_t encryptedConnectionsSoftLimit, 
                               const String &peerRequestNonce, const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint8_t hashKey[hashKeyLength])
 : EncryptedConnectionData(peerStaMac, peerApMac, 0, 0, EspnowMeshBackend::getEncryptionRequestTimeout(), hashKey), 
   _requestID(requestID), _requestEncrypted(requestEncrypted), _authenticationPassword(authenticationPassword), 
   _encryptedConnectionsSoftLimit(encryptedConnectionsSoftLimit), _peerRequestNonce(peerRequestNonce)
{ }

PeerRequestLog::PeerRequestLog(const uint64_t requestID, const bool requestEncrypted, const String &authenticationPassword, const uint8_t encryptedConnectionsSoftLimit, const String &peerRequestNonce, 
                               const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint8_t hashKey[hashKeyLength])
 : EncryptedConnectionData(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, EspnowMeshBackend::getEncryptionRequestTimeout(), hashKey), 
   _requestID(requestID), _requestEncrypted(requestEncrypted), _authenticationPassword(authenticationPassword), 
   _encryptedConnectionsSoftLimit(encryptedConnectionsSoftLimit), _peerRequestNonce(peerRequestNonce)
{ }

void PeerRequestLog::setRequestID(const uint64_t requestID) { _requestID = requestID; }
uint64_t PeerRequestLog::getRequestID() const { return _requestID; }

void PeerRequestLog::setRequestEncrypted(const bool requestEncrypted) { _requestEncrypted = requestEncrypted; }
bool PeerRequestLog::requestEncrypted() const { return _requestEncrypted; }

void PeerRequestLog::setAuthenticationPassword(const String &password) { _authenticationPassword = password; }
String PeerRequestLog::getAuthenticationPassword() const { return _authenticationPassword; }

void PeerRequestLog::setEncryptedConnectionsSoftLimit(const uint8_t softLimit) { _encryptedConnectionsSoftLimit = softLimit; }
uint8_t PeerRequestLog::getEncryptedConnectionsSoftLimit() const { return _encryptedConnectionsSoftLimit; }

void PeerRequestLog::setPeerRequestNonce(const String &nonce) { _peerRequestNonce = nonce; }
String PeerRequestLog::getPeerRequestNonce() const { return _peerRequestNonce; }
