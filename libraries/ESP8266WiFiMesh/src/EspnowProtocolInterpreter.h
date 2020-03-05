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
  constexpr char synchronizationRequestHeader[] PROGMEM = "Synchronization request.";
  constexpr char encryptionRequestHeader[] PROGMEM = "AddEC:"; // Add encrypted connection
  constexpr char temporaryEncryptionRequestHeader[] PROGMEM = "AddTEC:"; // Add temporary encrypted connection
  constexpr char basicConnectionInfoHeader[] PROGMEM = "BasicCI:"; // Basic connection info
  constexpr char encryptedConnectionInfoHeader[] PROGMEM = "EncryptedCI:"; // Encrypted connection info
  constexpr char softLimitEncryptedConnectionInfoHeader[] PROGMEM = "SLEncryptedCI:"; // Soft limit encrypted connection info
  constexpr char maxConnectionsReachedHeader[] PROGMEM = "MAX_CONNECTIONS_REACHED_PEER:";
  constexpr char encryptedConnectionVerificationHeader[] PROGMEM = "ECVerified:"; // Encrypted connection verified
  constexpr char encryptedConnectionRemovalRequestHeader[] PROGMEM = "RemoveEC:"; // Remove encrypted connection

  constexpr uint8_t espnowMessageTypeIndex = 0;
  constexpr uint8_t espnowTransmissionsRemainingIndex = 1;
  constexpr uint8_t espnowTransmissionMacIndex = 2;
  constexpr uint8_t espnowMessageIDIndex = 8;

  constexpr uint8_t espnowProtocolBytesSize = 16;
  constexpr uint8_t aeadMetadataSize = 28;
  uint8_t espnowMetadataSize();

  constexpr uint8_t espnowEncryptedConnectionKeyLength = 16;  // This is restricted to exactly 16 bytes by the ESP-NOW API. It should not be changed unless the ESP-NOW API is changed.
  constexpr uint8_t espnowHashKeyLength = 16; // This can be changed to any value up to 255. Common values are 16 and 32.

  constexpr uint64_t uint64LeftmostBits = 0xFFFFFFFF00000000;

  String espnowGetMessageContent(uint8_t *transmissionDataArray, uint8_t transmissionLength);
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
