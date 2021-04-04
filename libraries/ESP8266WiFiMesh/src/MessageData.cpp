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

#include "MessageData.h"
#include "EspnowProtocolInterpreter.h"
#include "EspnowMeshBackend.h"
#include <assert.h>

MessageData::MessageData(const String &message, const uint8_t transmissionsRemaining, const uint32_t creationTimeMs) :
  _timeTracker(creationTimeMs)
{
  _transmissionsExpected = transmissionsRemaining + 1;
  _totalMessage += message;
  ++_transmissionsReceived;
}

MessageData::MessageData(uint8_t *initialTransmission, const uint8_t transmissionLength, const uint32_t creationTimeMs) :
  _timeTracker(creationTimeMs)
{
  _transmissionsExpected = EspnowProtocolInterpreter::getTransmissionsRemaining(initialTransmission) + 1;
  addToMessage(initialTransmission, transmissionLength);
}

bool MessageData::addToMessage(uint8_t *transmission, const uint8_t transmissionLength)
{
  if(EspnowProtocolInterpreter::getTransmissionsRemaining(transmission) == getTransmissionsRemaining() - 1)
  {
    String message = EspnowProtocolInterpreter::getHashKeyLength(transmission, transmissionLength);
    assert(message.length() <= EspnowMeshBackend::getMaxMessageBytesPerTransmission()); // Should catch some cases where transmission is not null terminated.
    _totalMessage += message;
    ++_transmissionsReceived;
    return true;
  }

  return false;
}

uint8_t MessageData::getTransmissionsReceived() const
{
  return _transmissionsReceived;
}

uint8_t MessageData::getTransmissionsExpected() const
{
  return _transmissionsExpected;
}

uint8_t MessageData::getTransmissionsRemaining() const
{
  return getTransmissionsExpected() - getTransmissionsReceived();
}

String MessageData::getTotalMessage() const
{
  return _totalMessage;
}

const TimeTracker &MessageData::getTimeTracker() const { return _timeTracker; }
