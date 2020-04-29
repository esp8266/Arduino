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

#ifndef __ESPNOWENCRYPTEDCONNECTIONDATA_H__
#define __ESPNOWENCRYPTEDCONNECTIONDATA_H__

#include "ExpiringTimeTracker.h"
#include "EspnowProtocolInterpreter.h"
#include <WString.h>
#include <memory>

class EncryptedConnectionData {
  
public:

  virtual ~EncryptedConnectionData() = default; 

  EncryptedConnectionData(const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint64_t peerSessionKey, const uint64_t ownSessionKey,
                          const uint8_t hashKey[EspnowProtocolInterpreter::espnowHashKeyLength]);
  EncryptedConnectionData(const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint64_t peerSessionKey, const uint64_t ownSessionKey,
                          const uint32_t duration, const uint8_t hashKey[EspnowProtocolInterpreter::espnowHashKeyLength]);
  
  EncryptedConnectionData(const EncryptedConnectionData &other);

  EncryptedConnectionData & operator=(const EncryptedConnectionData &other);

  /**
   * @param resultArray An uint8_t array with at least size 6.
   * 
   * @return The interface MAC used for communicating with the peer.
   */
  uint8_t *getEncryptedPeerMac(uint8_t *resultArray) const;
  uint8_t *getUnencryptedPeerMac(uint8_t *resultArray) const;

  // @param resultArray At least size 6.
  uint8_t *getPeerStaMac(uint8_t *resultArray) const;
  void setPeerStaMac(const uint8_t *peerStaMac) = delete; // A method for setPeerStaMac would sometimes require interacting with the ESP-NOW API to change encrypted connections, so it is not implemented.
  uint8_t *getPeerApMac(uint8_t *resultArray) const;
  void setPeerApMac(const uint8_t *peerApMac);

  bool connectedTo(const uint8_t *peerMac) const;

  void setHashKey(const uint8_t hashKey[EspnowProtocolInterpreter::espnowHashKeyLength]);
  // @param resultArray At least size espnowHashKeyLength. 
  uint8_t *getHashKey(uint8_t *resultArray) const;
  
  void setPeerSessionKey(const uint64_t sessionKey);
  uint64_t getPeerSessionKey() const;
  void setOwnSessionKey(const uint64_t sessionKey);
  uint64_t getOwnSessionKey() const;

  static uint64_t incrementSessionKey(const uint64_t sessionKey, const uint8_t *hashKey, const uint8_t hashKeyLength);
  void incrementOwnSessionKey();

  void setDesync(const bool desync);
  bool desync() const;

  // Note that the espnowEncryptedConnectionKey, espnowEncryptionKok, espnowHashKey and espnowMessageEncryptionKey are not serialized. 
  // These will be set to the values of the EspnowMeshBackend instance that is adding the serialized encrypted connection.
  String serialize() const;

  const ExpiringTimeTracker *temporary() const;
  virtual void setRemainingDuration(const uint32_t remainingDuration);
  virtual void removeDuration();
    
private:

  uint8_t _peerStaMac[6] {0};
  uint8_t _peerApMac[6] {0};
  uint64_t _peerSessionKey;
  uint64_t _ownSessionKey;
  uint8_t _hashKey[EspnowProtocolInterpreter::espnowHashKeyLength] {0};
  bool _desync = false;
  std::unique_ptr<ExpiringTimeTracker> _timeTracker = nullptr;
};

#endif
