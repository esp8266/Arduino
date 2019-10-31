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

using EspnowProtocolInterpreter::espnowHashKeyLength;

PeerRequestLog::PeerRequestLog(uint64_t requestID, bool requestEncrypted, const String &authenticationPassword, uint8_t encryptedConnectionsSoftLimit, 
                               const String &peerRequestNonce, const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint8_t hashKey[espnowHashKeyLength])
 : EncryptedConnectionData(peerStaMac, peerApMac, 0, 0, EspnowMeshBackend::getEncryptionRequestTimeout(), hashKey), 
   _requestID(requestID), _requestEncrypted(requestEncrypted), _authenticationPassword(authenticationPassword), 
   _encryptedConnectionsSoftLimit(encryptedConnectionsSoftLimit), _peerRequestNonce(peerRequestNonce)
{ }

PeerRequestLog::PeerRequestLog(uint64_t requestID, bool requestEncrypted, const String &authenticationPassword, uint8_t encryptedConnectionsSoftLimit, const String &peerRequestNonce, 
                               const uint8_t peerStaMac[6], const uint8_t peerApMac[6], uint64_t peerSessionKey, uint64_t ownSessionKey, const uint8_t hashKey[espnowHashKeyLength])
 : EncryptedConnectionData(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, EspnowMeshBackend::getEncryptionRequestTimeout(), hashKey), 
   _requestID(requestID), _requestEncrypted(requestEncrypted), _authenticationPassword(authenticationPassword), 
   _encryptedConnectionsSoftLimit(encryptedConnectionsSoftLimit), _peerRequestNonce(peerRequestNonce)
{ }

void PeerRequestLog::setRequestID(uint64_t requestID) { _requestID = requestID; }
uint64_t PeerRequestLog::getRequestID() { return _requestID; }

void PeerRequestLog::setRequestEncrypted(bool requestEncrypted) { _requestEncrypted = requestEncrypted; }
bool PeerRequestLog::requestEncrypted() { return _requestEncrypted; }

void PeerRequestLog::setAuthenticationPassword(const String &password) { _authenticationPassword = password; }
String PeerRequestLog::getAuthenticationPassword() { return _authenticationPassword; }

void PeerRequestLog::setEncryptedConnectionsSoftLimit(uint8_t softLimit) { _encryptedConnectionsSoftLimit = softLimit; }
uint8_t PeerRequestLog::getEncryptedConnectionsSoftLimit() { return _encryptedConnectionsSoftLimit; }

void PeerRequestLog::setPeerRequestNonce(const String &nonce) { _peerRequestNonce = nonce; }
String PeerRequestLog::getPeerRequestNonce() { return _peerRequestNonce; }
