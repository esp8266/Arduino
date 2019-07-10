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

#ifndef __ESPNOWENCRYPTEDCONNECTIONLOG_H__
#define __ESPNOWENCRYPTEDCONNECTIONLOG_H__

#include "EncryptedConnectionData.h"
#include "EspnowProtocolInterpreter.h"

class EncryptedConnectionLog : public EncryptedConnectionData {
  
public:

  EncryptedConnectionLog(const uint8_t peerStaMac[6], const uint8_t peerApMac[6], uint64_t peerSessionKey, uint64_t ownSessionKey, 
                         const uint8_t hashKey[EspnowProtocolInterpreter::espnowHashKeyLength]);
  EncryptedConnectionLog(const uint8_t peerStaMac[6], const uint8_t peerApMac[6], uint64_t peerSessionKey, uint64_t ownSessionKey, 
                         uint32_t duration, const uint8_t hashKey[EspnowProtocolInterpreter::espnowHashKeyLength]);

  // Only guaranteed to expire at the latest when the soonestExpiringConnection does. Can expire before the soonestExpiringConnection since it is not updated on connection removal.
  // Needs to be a copy to avoid invalidation during operations on temporaryEncryptedConnections.
  static std::unique_ptr<ExpiringTimeTracker> _soonestExpiringConnectionTracker;
  
  // Only indicates if at least one removal was scheduled since the flag was last cleared, not if the removal is still scheduled to happen.
  // Canceling a removal will not update the flag.
  static bool _newRemovalsScheduled;

  // Can be used to set a duration both for temporary and permanent encrypted connections (transforming the latter into a temporary connection in the process).
  void setRemainingDuration(uint32_t remainingDuration) override;
  void removeDuration() override;

  void scheduleForRemoval();
  bool removalScheduled() const;

  static void setNewRemovalsScheduled(bool newRemovalsScheduled);
  static bool newRemovalsScheduled();

  static const ExpiringTimeTracker *getSoonestExpiringConnectionTracker();
  static void updateSoonestExpiringConnectionTracker(uint32_t remainingDuration);
  static void clearSoonestExpiringConnectionTracker();

private:

  bool _scheduledForRemoval = false;
  void setScheduledForRemoval(bool scheduledForRemoval);
};

#endif
