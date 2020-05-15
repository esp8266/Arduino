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

  constexpr uint8_t messageTypeIndex = 0;
  constexpr uint8_t transmissionsRemainingIndex = 1;
  constexpr uint8_t transmissionMacIndex = 2;
  constexpr uint8_t messageIDIndex = 8;

  constexpr uint8_t protocolBytesSize = 16;
  constexpr uint8_t aeadMetadataSize = 28;
  uint8_t metadataSize();
  uint32_t getMaxBytesPerTransmission();
  uint32_t getMaxMessageBytesPerTransmission();
  
  constexpr uint8_t broadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  constexpr uint64_t uint64BroadcastMac = 0xFFFFFFFFFFFF;
    
  constexpr uint8_t maxEncryptedConnections = 6; // This is limited by the ESP-NOW API. Max 6 in AP or AP+STA mode. Max 10 in STA mode. See "ESP-NOW User Guide" for more info. 

  constexpr uint8_t encryptedConnectionKeyLength = 16;  // This is restricted to exactly 16 bytes by the ESP-NOW API. It should not be changed unless the ESP-NOW API is changed.
  constexpr uint8_t hashKeyLength = 16; // This can be changed to any value up to 255. Common values are 16 and 32.

  constexpr uint64_t uint64LeftmostBits = 0xFFFFFFFF00000000;
  constexpr uint64_t uint64MSB = 0x8000000000000000;

  String getHashKeyLength(uint8_t *transmissionDataArray, const uint8_t transmissionLength);
  char getMessageType(const uint8_t *transmissionDataArray);
  uint8_t getTransmissionsRemaining(const uint8_t *transmissionDataArray);
  bool isMessageStart(const uint8_t *transmissionDataArray);
  uint64_t getTransmissionMac(const uint8_t *transmissionDataArray);
  uint8_t *getTransmissionMac(const uint8_t *transmissionDataArray, uint8_t *resultArray);
  uint64_t getMessageID(const uint8_t *transmissionDataArray);
  // @return a pointer to transmissionDataArray
  uint8_t *setMessageID(uint8_t *transmissionDataArray, const uint64_t messageID);

  bool usesEncryption(const uint64_t messageID);
  bool usesConstantSessionKey(const char messageType);

  /**
   * Create a new session key for an encrypted connection using the built in RANDOM_REG32 of the ESP8266. 
   * Should only be used when initializing a new connection. 
   * Use generateMessageID instead when the encrypted connection is already initialized to keep the connection synchronized.
   * 
   * @return A uint64_t containing a new session key for an encrypted connection.
   */
  uint64_t createSessionKey();

  enum class macAndType_td : uint64_t {};
  using messageID_td = uint64_t;
  using peerMac_td = uint64_t;
  
  macAndType_td createMacAndTypeValue(const uint64_t uint64Mac, const char messageType);
  uint64_t macAndTypeToUint64Mac(const macAndType_td &macAndTypeValue);
}

#endif
