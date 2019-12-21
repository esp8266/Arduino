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
#include "EspnowMeshBackend.h"

namespace TypeCast = MeshTypeConversionFunctions;

namespace EspnowProtocolInterpreter
{   
  uint8_t espnowMetadataSize()
  {
    return espnowProtocolBytesSize + (EspnowMeshBackend::useEncryptedMessages() ? aeadMetadataSize : 0);
  }
  
  String espnowGetMessageContent(uint8_t *transmissionDataArray, uint8_t transmissionLength)
  {
    String messageContent = emptyString;
    
    if(transmissionLength >= espnowMetadataSize())
    {
      uint8_t messageSize = transmissionLength - espnowMetadataSize();
      
      messageContent = TypeCast::uint8ArrayToMultiString(transmissionDataArray + espnowMetadataSize(), messageSize);
    }

    return messageContent;
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
    return TypeCast::macToUint64(transmissionDataArray + espnowTransmissionMacIndex);
  }

  uint8_t *espnowGetTransmissionMac(const uint8_t *transmissionDataArray, uint8_t *resultArray)
  {
    std::copy_n((transmissionDataArray + espnowTransmissionMacIndex), 6, resultArray);
    return resultArray;
  }

  uint64_t espnowGetMessageID(const uint8_t *transmissionDataArray)
  {
    return TypeCast::uint8ArrayToUint64(transmissionDataArray + espnowMessageIDIndex);
  }

  uint8_t *espnowSetMessageID(uint8_t *transmissionDataArray, uint64_t messageID)
  {
    return TypeCast::uint64ToUint8Array(messageID, transmissionDataArray + espnowMessageIDIndex);
  }

  bool usesEncryption(uint64_t messageID)
  {
    // At least one of the leftmost half of bits in messageID is 1 if the transmission is encrypted.
    return messageID & uint64LeftmostBits;
  }
}
