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

#include "EncryptedConnectionLog.h"

namespace
{
  using EspnowProtocolInterpreter::hashKeyLength;
}

EncryptedConnectionLog::EncryptedConnectionLog(const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint8_t hashKey[hashKeyLength]) 
  : EncryptedConnectionData(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, hashKey)
{ }

EncryptedConnectionLog::EncryptedConnectionLog(const uint8_t peerStaMac[6], const uint8_t peerApMac[6], const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint32_t duration, const uint8_t hashKey[hashKeyLength]) 
  : EncryptedConnectionData(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, duration, hashKey)
{ }

std::unique_ptr<ExpiringTimeTracker> EncryptedConnectionLog::_soonestExpiringConnectionTracker = nullptr;

bool EncryptedConnectionLog::_newRemovalsScheduled = false;

void EncryptedConnectionLog::setRemainingDuration(const uint32_t remainingDuration)
{  
  EncryptedConnectionData::setRemainingDuration(remainingDuration);
  
  setScheduledForRemoval(false);

  updateSoonestExpiringConnectionTracker(remainingDuration);
}

void EncryptedConnectionLog::removeDuration()
{
  EncryptedConnectionData::removeDuration();
  setScheduledForRemoval(false);
}

void EncryptedConnectionLog::scheduleForRemoval()
{
  // When we give the connection 0 remaining duration it will be removed during the next performEspnowMaintenance() call.
  // Duration must be changed before setting the scheduledForRemoval flag to true, since the flag is otherwise cleared.
  setRemainingDuration(0);
  setScheduledForRemoval(true);
}

void EncryptedConnectionLog::setScheduledForRemoval(const bool scheduledForRemoval) 
{ 
  _scheduledForRemoval = scheduledForRemoval;

  if(scheduledForRemoval)
    setNewRemovalsScheduled(true);
}
bool EncryptedConnectionLog::removalScheduled() const { return _scheduledForRemoval; }

void EncryptedConnectionLog::setNewRemovalsScheduled(const bool newRemovalsScheduled) { _newRemovalsScheduled = newRemovalsScheduled; }
bool EncryptedConnectionLog::newRemovalsScheduled( ) { return _newRemovalsScheduled; }

const ExpiringTimeTracker *EncryptedConnectionLog::getSoonestExpiringConnectionTracker()
{
  return _soonestExpiringConnectionTracker.get();
}

void EncryptedConnectionLog::updateSoonestExpiringConnectionTracker(const uint32_t remainingDuration)
{
  if(!getSoonestExpiringConnectionTracker() || remainingDuration < getSoonestExpiringConnectionTracker()->remainingDuration())
  {
    _soonestExpiringConnectionTracker = std::unique_ptr<ExpiringTimeTracker>(new ExpiringTimeTracker(remainingDuration));  // TODO: Change to std::make_unique<ExpiringTimeTracker>(remainingDuration); once compiler fully supports C++14
  }
}

void EncryptedConnectionLog::clearSoonestExpiringConnectionTracker()
{
  _soonestExpiringConnectionTracker = nullptr;
}
