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

#ifndef __ESPNOWMESSAGEDATA_H__
#define __ESPNOWMESSAGEDATA_H__

#include "TimeTracker.h"
#include <Arduino.h>

class MessageData : public TimeTracker {

public:

  MessageData(uint8_t *initialTransmission, uint8_t transmissionLength, uint32_t creationTimeMs = millis());
  /**
   * @transmission A string of characters, including initial protocol bytes.
   * @transmissionLength Length of transmission.
   */
  bool addToMessage(uint8_t *transmission, uint8_t transmissionLength);
  uint8_t getTransmissionsReceived();
  uint8_t getTransmissionsExpected();
  uint8_t getTransmissionsRemaining();
  String getTotalMessage();

private:

  uint8_t _transmissionsReceived = 0;
  uint8_t _transmissionsExpected;
  String _totalMessage = "";

};

#endif
