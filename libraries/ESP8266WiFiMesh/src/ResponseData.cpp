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

#include "ResponseData.h"

ResponseData::ResponseData(const String &message, const uint8_t recipientMac[6], const uint64_t requestID, const uint32_t creationTimeMs) : 
  _timeTracker(creationTimeMs), _message(message), _requestID(requestID)
{      
  storeRecipientMac(recipientMac);
}

ResponseData::ResponseData(const ResponseData &other) 
  : _timeTracker(other.getTimeTracker()), _message(other.getMessage()), _requestID(other.getRequestID())
{
  storeRecipientMac(other.getRecipientMac());
}

ResponseData & ResponseData::operator=(const ResponseData &other)
{
  if(this != &other)
  {
    _timeTracker = other.getTimeTracker();
    _message = other.getMessage();
    _requestID = other.getRequestID();
    storeRecipientMac(other.getRecipientMac());
  }
  
  return *this;
}

void ResponseData::storeRecipientMac(const uint8_t newRecipientMac[6])
{
  if(newRecipientMac == nullptr)
  {
    _recipientMac = nullptr;
    return;
  }
  
  if(_recipientMac == nullptr)
  {
    _recipientMac = _recipientMacArray;
  }
  
  for(int i = 0; i < 6; ++i)
  {
    _recipientMac[i] = newRecipientMac[i];
  }
}

void ResponseData::setRecipientMac(const uint8_t recipientMac[6]) { storeRecipientMac(recipientMac); }
const uint8_t *ResponseData::getRecipientMac() const { return _recipientMac; }

void ResponseData::setMessage(const String &message) { _message = message; }
String ResponseData::getMessage() const { return _message; }

void ResponseData::setRequestID(const uint64_t requestID) { _requestID = requestID; }
uint64_t ResponseData::getRequestID() const { return _requestID; }

const TimeTracker &ResponseData::getTimeTracker() const { return _timeTracker; }
