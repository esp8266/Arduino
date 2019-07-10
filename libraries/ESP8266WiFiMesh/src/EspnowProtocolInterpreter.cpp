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

#include "EspnowProtocolInterpreter.h"
#include "TypeConversionFunctions.h"
#include <algorithm>

namespace EspnowProtocolInterpreter
{ 
  const uint64_t uint64LeftmostBits = 0xFFFFFFFF00000000;
  
  uint8_t espnowProtocolBytesSize()
  {
    return 16;
  }
  
  String espnowGetMessageContent(uint8_t *transmission, uint8_t transmissionLength)
  {
    if(transmissionLength < espnowProtocolBytesSize())
    {
      return "";
    }
    else
    {
      // Ensure we have a NULL terminated character array so the String() constructor knows where to stop.
      uint8_t bufferedTransmission[transmissionLength + 1];
      std::copy_n(transmission, transmissionLength, bufferedTransmission);
      bufferedTransmission[transmissionLength] = 0;
      return String((char *)(bufferedTransmission + espnowProtocolBytesSize()));
    }
  }
  
  char espnowGetMessageType(const uint8_t *transmissionDataArray)
  {
    return char(transmissionDataArray[espnowMessageTypeIndex]);
  }

  uint8_t espnowGetTransmissionsRemaining(const uint8_t *transmissionDataArray)
  {
    return (transmissionDataArray[espnowTransmissionsRemainingIndex] & 0x7F);
  }

  bool espnowIsMessageStart(const uint8_t *transmissionDataArray)
  {
    return (transmissionDataArray[espnowTransmissionsRemainingIndex] & 0x80); // If MSB is one we have messageStart
  }

  uint64_t espnowGetTransmissionMac(const uint8_t *transmissionDataArray)
  {
    return macToUint64(transmissionDataArray + espnowTransmissionMacIndex);
  }

  uint8_t *espnowGetTransmissionMac(const uint8_t *transmissionDataArray, uint8_t *resultArray)
  {
    std::copy_n((transmissionDataArray + espnowTransmissionMacIndex), 6, resultArray);
    return resultArray;
  }

  uint64_t espnowGetMessageID(const uint8_t *transmissionDataArray)
  {
    uint64_t outcome = 0;
    for(int shiftingFortune = 56; shiftingFortune >= 0; shiftingFortune -= 8)
    {
      outcome |= ((uint64_t)transmissionDataArray[espnowMessageIDIndex + 7 - shiftingFortune/8] << shiftingFortune);
    }

    return outcome;
  }

  uint8_t *espnowSetMessageID(uint8_t *transmissionDataArray, uint64_t messageID)
  {
    for(int shiftingFortune = 56; shiftingFortune >= 0; shiftingFortune -= 8)
    {
      transmissionDataArray[espnowMessageIDIndex + 7 - shiftingFortune/8] = messageID >> shiftingFortune & 0xFF;
    }
    return transmissionDataArray;
  }

  bool usesEncryption(uint64_t messageID)
  {
    // At least one of the leftmost half of bits in messageID is 1 if the transmission is encrypted.
    return messageID & uint64LeftmostBits;
  }
}
