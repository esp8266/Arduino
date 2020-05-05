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

#include "EncryptedConnectionData.h"
#include "UtilityFunctions.h"
#include "TypeConversionFunctions.h"
#include "JsonTranslator.h"
#include "MeshCryptoInterface.h"

namespace
{
  using EspnowProtocolInterpreter::hashKeyLength;
  namespace TypeCast = MeshTypeConversionFunctions;
}

EncryptedConnectionData::EncryptedConnectionData(const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint8_t hashKey[hashKeyLength]) 
  : _peerSessionKey(peerSessionKey), _ownSessionKey(ownSessionKey)
{ 
  std::copy_n(peerStaMac, 6, _peerStaMac);
  std::copy_n(peerApMac, 6, _peerApMac);
  std::copy_n(hashKey, hashKeyLength, _hashKey);
}

EncryptedConnectionData::EncryptedConnectionData(const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint32_t duration, const uint8_t hashKey[hashKeyLength]) 
  : EncryptedConnectionData(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, hashKey)
{
  setRemainingDuration(duration);
}

EncryptedConnectionData::EncryptedConnectionData(const EncryptedConnectionData &other)
  : _peerSessionKey(other.getPeerSessionKey()), _ownSessionKey(other.getOwnSessionKey()), _desync(other.desync()),
    _timeTracker(other.temporary() ? new ExpiringTimeTracker(*other.temporary()) : nullptr)
{
  other.getPeerStaMac(_peerStaMac);
  other.getPeerApMac(_peerApMac);
  other.getHashKey(_hashKey);
}

EncryptedConnectionData & EncryptedConnectionData::operator=(const EncryptedConnectionData &other)
{
  if(this != &other)
  {
    other.getPeerStaMac(_peerStaMac);
    other.getPeerApMac(_peerApMac);
    _peerSessionKey = other.getPeerSessionKey();
    _ownSessionKey = other.getOwnSessionKey();
    other.getHashKey(_hashKey);
    _desync = other.desync();
    _timeTracker = std::unique_ptr<ExpiringTimeTracker>(other.temporary() ? new ExpiringTimeTracker(*other.temporary()) : nullptr);
  }
  return *this;
}

uint8_t *EncryptedConnectionData::getEncryptedPeerMac(uint8_t *resultArray) const
{
  return getPeerStaMac(resultArray);
}

uint8_t *EncryptedConnectionData::getUnencryptedPeerMac(uint8_t *resultArray) const
{
  return getPeerApMac(resultArray);
}

uint8_t *EncryptedConnectionData::getPeerStaMac(uint8_t *resultArray) const
{
  std::copy_n(_peerStaMac, 6, resultArray);
  return resultArray;
}

uint8_t *EncryptedConnectionData::getPeerApMac(uint8_t *resultArray) const
{
  std::copy_n(_peerApMac, 6, resultArray);
  return resultArray;
}

void EncryptedConnectionData::setPeerApMac(const uint8_t *peerApMac)
{
  std::copy_n(peerApMac, 6, _peerApMac);
}

bool EncryptedConnectionData::connectedTo(const uint8_t *peerMac) const
{
  if(MeshUtilityFunctions::macEqual(peerMac, _peerStaMac) || MeshUtilityFunctions::macEqual(peerMac, _peerApMac))
  {
    return true;
  }
  
  return false;
}

void EncryptedConnectionData::setHashKey(const uint8_t hashKey[hashKeyLength])
{
  assert(hashKey != nullptr);

  std::copy_n(hashKey, hashKeyLength, _hashKey);
}

uint8_t *EncryptedConnectionData::getHashKey(uint8_t *resultArray) const
{
  std::copy_n(_hashKey, hashKeyLength, resultArray);
  return resultArray;
}

void EncryptedConnectionData::setPeerSessionKey(const uint64_t sessionKey) { _peerSessionKey = sessionKey; }
uint64_t EncryptedConnectionData::getPeerSessionKey() const { return _peerSessionKey; }

void EncryptedConnectionData::setOwnSessionKey(const uint64_t sessionKey) { _ownSessionKey = sessionKey; }
uint64_t EncryptedConnectionData::getOwnSessionKey() const { return _ownSessionKey; }

uint64_t EncryptedConnectionData::incrementSessionKey(const uint64_t sessionKey, const uint8_t *hashKey, const uint8_t hashKeyLength)
{
  uint8_t inputArray[8] {0};
  uint8_t hmacArray[CryptoInterface::SHA256_NATURAL_LENGTH] {0};
  CryptoInterface::sha256Hmac(TypeCast::uint64ToUint8Array(sessionKey, inputArray), 8, hashKey, hashKeyLength, hmacArray, CryptoInterface::SHA256_NATURAL_LENGTH);

  /* HMAC truncation should be OK since hmac sha256 is a PRF and we are truncating to the leftmost (MSB) bits.
  PRF: https://crypto.stackexchange.com/questions/26410/whats-the-gcm-sha-256-of-a-tls-protocol/26434#26434
  Truncate to leftmost bits: https://tools.ietf.org/html/rfc2104#section-5 */
  uint64_t newLeftmostBits = TypeCast::uint8ArrayToUint64(hmacArray) & EspnowProtocolInterpreter::uint64LeftmostBits;
  
  if(newLeftmostBits == 0)
    newLeftmostBits = ((uint64_t)RANDOM_REG32 | (1 << 31)) << 32; // We never want newLeftmostBits == 0 since that would indicate an unencrypted transmission.
  
  uint64_t newRightmostBits = (uint32_t)(sessionKey + 1);

  return newLeftmostBits | newRightmostBits;
}

void EncryptedConnectionData::incrementOwnSessionKey()
{
  setOwnSessionKey(incrementSessionKey(getOwnSessionKey(), _hashKey, EspnowProtocolInterpreter::hashKeyLength));
}

void EncryptedConnectionData::setDesync(const bool desync) { _desync = desync; }
bool EncryptedConnectionData::desync() const { return _desync; }

String EncryptedConnectionData::serialize() const
{
  // Returns: {"connectionState":{"duration":"123","password":"abc","ownSK":"1A2","peerSK":"3B4","peerStaMac":"F2","peerApMac":"E3"}}
  
  return 
  String(FPSTR(JsonTranslator::jsonConnectionState))
  + (temporary() ? String(FPSTR(JsonTranslator::jsonDuration)) + '\"' + String(temporary()->remainingDuration()) + F("\",") : emptyString)
  + FPSTR(JsonTranslator::jsonDesync) + '\"' + String(desync()) + F("\",") 
  + FPSTR(JsonTranslator::jsonOwnSessionKey) + '\"' + TypeCast::uint64ToString(getOwnSessionKey()) + F("\",") 
  + FPSTR(JsonTranslator::jsonPeerSessionKey) + '\"' + TypeCast::uint64ToString(getPeerSessionKey()) + F("\",") 
  + FPSTR(JsonTranslator::jsonPeerStaMac) + '\"' + TypeCast::macToString(_peerStaMac) + F("\",") 
  + FPSTR(JsonTranslator::jsonPeerApMac) + '\"' + TypeCast::macToString(_peerApMac) +  F("\"}}");
}

const ExpiringTimeTracker *EncryptedConnectionData::temporary() const
{
  return _timeTracker.get();
}

void EncryptedConnectionData::setRemainingDuration(const uint32_t remainingDuration)
{
  if(!_timeTracker)
  {
    _timeTracker = std::unique_ptr<ExpiringTimeTracker>(new ExpiringTimeTracker(remainingDuration)); // TODO: Change to std::make_unique<ExpiringTimeTracker>(remainingDuration); once compiler fully supports C++14
  }
  else
  {
    _timeTracker->setRemainingDuration(remainingDuration);
  }
}

void EncryptedConnectionData::removeDuration()
{
  _timeTracker = nullptr;
}
