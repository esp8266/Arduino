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

namespace
{
  namespace TypeCast = MeshTypeConversionFunctions;
}

namespace EspnowProtocolInterpreter
{   
  uint8_t metadataSize()
  {
    return protocolBytesSize + (EspnowMeshBackend::useEncryptedMessages() ? aeadMetadataSize : 0);
  }
  
  String getHashKeyLength(uint8_t *transmissionDataArray, const uint8_t transmissionLength)
  {
    String messageContent = emptyString;
    
    if(transmissionLength >= metadataSize())
    {
      uint8_t messageSize = transmissionLength - metadataSize();
      
      messageContent = TypeCast::uint8ArrayToMultiString(transmissionDataArray + metadataSize(), messageSize);
    }

    return messageContent;
  }
  
  char getMessageType(const uint8_t *transmissionDataArray)
  {
    return char(transmissionDataArray[messageTypeIndex]);
  }

  uint8_t getTransmissionsRemaining(const uint8_t *transmissionDataArray)
  {
    return (transmissionDataArray[transmissionsRemainingIndex] & 0x7F);
  }

  bool isMessageStart(const uint8_t *transmissionDataArray)
  {
    return (transmissionDataArray[transmissionsRemainingIndex] & 0x80); // If MSB is one we have messageStart
  }

  uint64_t getTransmissionMac(const uint8_t *transmissionDataArray)
  {
    return TypeCast::macToUint64(transmissionDataArray + transmissionMacIndex);
  }

  uint8_t *getTransmissionMac(const uint8_t *transmissionDataArray, uint8_t *resultArray)
  {
    std::copy_n((transmissionDataArray + transmissionMacIndex), 6, resultArray);
    return resultArray;
  }

  uint64_t getMessageID(const uint8_t *transmissionDataArray)
  {
    return TypeCast::uint8ArrayToUint64(transmissionDataArray + messageIDIndex);
  }

  uint8_t *setMessageID(uint8_t *transmissionDataArray, const uint64_t messageID)
  {
    return TypeCast::uint64ToUint8Array(messageID, transmissionDataArray + messageIDIndex);
  }

  bool usesEncryption(const uint64_t messageID)
  {
    // At least one of the leftmost half of bits in messageID is 1 if the transmission is encrypted.
    return messageID & uint64LeftmostBits;
  }
}
