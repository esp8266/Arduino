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

#ifndef __ESPNOWRESPONSEDATA_H__
#define __ESPNOWRESPONSEDATA_H__

#include "TimeTracker.h"
#include <Arduino.h>

class ResponseData {
  
public:

  ResponseData(const String &message, const uint8_t recipientMac[6], const uint64_t requestID, const uint32_t creationTimeMs = millis());
  ResponseData(const ResponseData &other);
  ResponseData & operator=(const ResponseData &other);
  // No need for explicit destructor with current class design

  void setRecipientMac(const uint8_t recipientMac[6]);
  const uint8_t *getRecipientMac() const;

  void setMessage(const String &message);
  String getMessage() const;

  void setRequestID(const uint64_t requestID);
  uint64_t getRequestID() const;

  const TimeTracker &getTimeTracker() const;

private:

  void storeRecipientMac(const uint8_t newRecipientMac[6]);

  TimeTracker _timeTracker;
  
  uint8_t *_recipientMac = nullptr;
  String _message;
  uint64_t _requestID = 0;
  uint8_t _recipientMacArray[6] {0};
};

#endif
