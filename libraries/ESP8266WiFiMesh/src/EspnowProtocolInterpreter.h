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

#ifndef __ESPNOWPROTOCOLINTERPRETER_H__
#define __ESPNOWPROTOCOLINTERPRETER_H__

#include <WString.h>

// The following protocol is used on top of ESP-NOW (for the bits and bytes in each transmission):
// Bit 0-7: Message type. The type for requests must be different from the type for responses if they may require more than one transmission. Otherwise multi-part requests and responses with the same ID may be mixed together.
// Bit 8: Flag for message start.
// Bit 9-15: Transmissions remaining for the message.
// Byte 2-7: Transmission sender MAC address for AP interface. Since we always transmit from the station interface, this ensures both sender MAC addresses are available to the receiver.
// Byte 8-15: Message ID. 32 rightmost bits used for unencrypted messages (the rest is 0). 64 bits used for encrypted messages (with at least one of the leftmost 32 bits set to 1).
//            This distinction based on encryption is required since the ESP-NOW API does not provide information about whether a received transmission is encrypted or not.
// Byte 16-249: The message.
// Each message can be split in up to EspnowMeshBackend::getMaxTransmissionsPerMessage() transmissions, based on message size. (max three transmissions per message is the default)

namespace EspnowProtocolInterpreter
{ 
  const String synchronizationRequestHeader = "Synchronization request.";
  const String encryptionRequestHeader = "AddEC:"; // Add encrypted connection
  const String temporaryEncryptionRequestHeader = "AddTEC:"; // Add temporary encrypted connection
  const String basicConnectionInfoHeader = "BasicCI:"; // Basic connection info
  const String encryptedConnectionInfoHeader = "EncryptedCI:"; // Encrypted connection info
  const String maxConnectionsReachedHeader = "ECS_MAX_CONNECTIONS_REACHED_PEER:";
  const String encryptedConnectionVerificationHeader = "ECVerified:"; // Encrypted connection verified
  const String encryptedConnectionRemovalRequestHeader = "RemoveEC:"; // Remove encrypted connection

  const uint8_t espnowMessageTypeIndex = 0;
  const uint8_t espnowTransmissionsRemainingIndex = 1;
  const uint8_t espnowTransmissionMacIndex = 2;
  const uint8_t espnowMessageIDIndex = 8;
  
  uint8_t espnowProtocolBytesSize();

  const uint8_t espnowEncryptionKeyLength = 16;  // This is restricted to exactly 16 bytes by the ESP-NOW API. It should not be changed unless the ESP-NOW API is changed.
  const uint8_t espnowHashKeyLength = 16; // This can be changed to any value up to 255. Common values are 16 and 32.

  String espnowGetMessageContent(uint8_t *transmission, uint8_t transmissionLength);
  char espnowGetMessageType(const uint8_t *transmissionDataArray);
  uint8_t espnowGetTransmissionsRemaining(const uint8_t *transmissionDataArray);
  bool espnowIsMessageStart(const uint8_t *transmissionDataArray);
  uint64_t espnowGetTransmissionMac(const uint8_t *transmissionDataArray);
  uint8_t *espnowGetTransmissionMac(const uint8_t *transmissionDataArray, uint8_t *resultArray);
  uint64_t espnowGetMessageID(const uint8_t *transmissionDataArray);
  // @return a pointer to transmissionDataArray
  uint8_t *espnowSetMessageID(uint8_t *transmissionDataArray, uint64_t messageID);

  bool usesEncryption(uint64_t messageID);
}

#endif
