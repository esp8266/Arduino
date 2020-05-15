/*
  Copyright (C) 2020 Anders Löfgren
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __ESPNOWTRANSMITTER_H__
#define __ESPNOWTRANSMITTER_H__

#include <Arduino.h>
#include "ExpiringTimeTracker.h"
#include "EspnowDatabase.h"
#include "EspnowConnectionManager.h"
#include "ConditionalPrinter.h"
#include "CryptoInterface.h"

class EspnowMeshBackend;

class EspnowTransmitter
{

public:

  using responseTransmittedHookType = std::function<bool(const String &, const uint8_t *, uint32_t, EspnowMeshBackend &)>;

  EspnowTransmitter(ConditionalPrinter &conditionalPrinterInstance, EspnowDatabase &databaseInstance, EspnowConnectionManager &connectionManagerInstance);

  static void espnowSendCallback(uint8_t* mac, uint8_t sendStatus);

  /**
   * Send an ESP-NOW message to the ESP8266 that has the MAC address specified in targetBSSID.
   * 
   * @param messageType The identifier character for the type of message to send. Choices are 'Q' for question (request), 
   * 'A' for answer (response), 'B' for broadcast, 'S' for synchronization request, 'P' for peer request and 'C' for peer request confirmation.
   * @return The transmission status for the transmission.
   */
  // Send a message to the node having targetBSSID as mac, changing targetBSSID to the mac of the encrypted connection if it exists and ensuring such an encrypted connection is synchronized.
  static TransmissionStatusType espnowSendToNode(const String &message, const uint8_t *targetBSSID, const char messageType, EspnowMeshBackend *espnowInstance = nullptr);
  // Send a message using exactly the arguments given, without consideration for any encrypted connections.
  static TransmissionStatusType espnowSendToNodeUnsynchronized(const String message, const uint8_t *targetBSSID, const char messageType, const uint64_t messageID, EspnowMeshBackend *espnowInstance = nullptr);

  // Send a PeerRequestConfirmation using exactly the arguments given, without consideration for any encrypted connections.
  static TransmissionStatusType espnowSendPeerRequestConfirmationsUnsynchronized(const String message, const uint8_t *targetBSSID, const char messageType, EspnowMeshBackend *espnowInstance = nullptr);
  
  TransmissionStatusType sendRequest(const String &message, const uint8_t *targetBSSID, EspnowMeshBackend *espnowInstance);
  TransmissionStatusType sendResponse(const String &message, const uint64_t requestID, const uint8_t *targetBSSID, EspnowMeshBackend *espnowInstance);
  
  static void setUseEncryptedMessages(const bool useEncryptedMessages);
  static bool useEncryptedMessages();
  static void setEspnowMessageEncryptionKey(const uint8_t espnowMessageEncryptionKey[CryptoInterface::ENCRYPTION_KEY_LENGTH]);
  static void setEspnowMessageEncryptionKey(const String &espnowMessageEncryptionKeySeed);
  static const uint8_t *getEspnowMessageEncryptionKey();

  void setBroadcastTransmissionRedundancy(const uint8_t redundancy);
  uint8_t getBroadcastTransmissionRedundancy() const;
  void setResponseTransmittedHook(const responseTransmittedHookType responseTransmittedHook);
  responseTransmittedHookType getResponseTransmittedHook() const;
  static void setMaxTransmissionsPerMessage(const uint8_t maxTransmissionsPerMessage);
  static uint8_t getMaxTransmissionsPerMessage();
  static uint32_t getMaxMessageLength();
  static void setEspnowTransmissionTimeout(const uint32_t timeoutMs);
  static uint32_t getEspnowTransmissionTimeout();
  static void setEspnowRetransmissionInterval(const uint32_t intervalMs);
  static uint32_t getEspnowRetransmissionInterval();
  static double getTransmissionFailRate();
  static void resetTransmissionFailRate();

  /*
   * @param estimatedMaxDurationTracker A pointer to an ExpiringTimeTracker initialized with the desired max duration for the method. If set to nullptr there is no duration limit. 
   *                                    Note that setting the estimatedMaxDuration too low may result in missed ESP-NOW transmissions because of too little time for maintenance.
   *                                    Also note that although the method will try to respect the max duration limit, there is no guarantee. Overshoots by tens of milliseconds are possible.                            
   */
  static void sendEspnowResponses(const ExpiringTimeTracker *estimatedMaxDurationTracker = nullptr);

  /** 
   * Will be captured if a transmission initiated by a public method is in progress.
   */
  static MutexTracker captureEspnowTransmissionMutex();
  static MutexTracker captureEspnowTransmissionMutex(const std::function<void()> destructorHook);
  
  /**
   * Check if there is an ongoing ESP-NOW transmission in the library. Used to avoid interrupting transmissions.
   * 
   * @return True if a transmission initiated by a public method is in progress.
   */
  static bool transmissionInProgress();

private:

  ConditionalPrinter & _conditionalPrinter;
  EspnowDatabase & _database;
  EspnowConnectionManager & _connectionManager;

  uint8_t _broadcastTransmissionRedundancy = 1;

  responseTransmittedHookType _responseTransmittedHook = [](const String &, const uint8_t *, uint32_t, EspnowMeshBackend &){ return true; };
};

#endif
