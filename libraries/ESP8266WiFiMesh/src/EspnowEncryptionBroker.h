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

#ifndef __ESPNOWENCRYPTIONBROKER_H__
#define __ESPNOWENCRYPTIONBROKER_H__

#include <Arduino.h>
#include "ConditionalPrinter.h"
#include "EspnowDatabase.h"
#include "EspnowConnectionManager.h"
#include "EspnowTransmitter.h"

class EspnowMeshBackend;
  
class EspnowEncryptionBroker
{

public:
  
  EspnowEncryptionBroker(ConditionalPrinter &conditionalPrinterInstance, EspnowDatabase &databaseInstance, EspnowConnectionManager &connectionManagerInstance, EspnowTransmitter &transmitterInstance);

  static void handlePeerRequest(const uint8_t *macaddr, uint8_t *dataArray, const uint8_t len, const uint64_t uint64StationMac, const uint64_t receivedMessageID);
  static void handlePeerRequestConfirmation(uint8_t *macaddr, uint8_t *dataArray, const uint8_t len);
  
  /*
   * @param estimatedMaxDurationTracker A pointer to an ExpiringTimeTracker initialized with the desired max duration for the method. If set to nullptr there is no duration limit. 
   *                                    Note that setting the estimatedMaxDuration too low may result in missed ESP-NOW transmissions because of too little time for maintenance.
   *                                    Also note that although the method will try to respect the max duration limit, there is no guarantee. Overshoots by tens of milliseconds are possible.                            
   */
  static void sendPeerRequestConfirmations(const ExpiringTimeTracker *estimatedMaxDurationTracker = nullptr);
  
  EncryptedConnectionStatus requestEncryptedConnection(const uint8_t *peerMac, EspnowMeshBackend &espnowInstance);
  EncryptedConnectionStatus requestTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t durationMs, EspnowMeshBackend &espnowInstance);
  EncryptedConnectionStatus requestFlexibleTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t minDurationMs, EspnowMeshBackend &espnowInstance);
  EncryptedConnectionRemovalOutcome requestEncryptedConnectionRemoval(const uint8_t *peerMac);

  static bool encryptedConnectionEstablished(const EncryptedConnectionStatus connectionStatus);

  static bool verifyPeerSessionKey(const uint64_t sessionKey, const uint8_t *peerMac, const char messageType);
  static bool verifyPeerSessionKey(const uint64_t sessionKey, const EncryptedConnectionLog &encryptedConnection, const uint64_t uint64PeerMac, const char messageType);

  static bool synchronizePeerSessionKey(const uint64_t sessionKey, const uint8_t *peerMac);
  static bool synchronizePeerSessionKey(const uint64_t sessionKey, EncryptedConnectionLog &encryptedConnection);
  
  /**
   * Set whether the most recently received ESP-NOW request, response or broadcast is presented as having been sent over an encrypted connection or not
   * 
   * @param receivedEncryptedTransmission If true, the request, response or broadcast is presented as having been sent over an encrypted connection.
   */
  void setReceivedEncryptedTransmission(const bool receivedEncryptedTransmission);
  bool receivedEncryptedTransmission() const;
  
  /**
   * reservedEncryptedConnections never underestimates but sometimes temporarily overestimates.
   * numberOfEncryptedConnections sometimes temporarily underestimates but never overestimates.
   * 
   * @return The current number of encrypted ESP-NOW connections, but with an encrypted connection immediately reserved if required while making a peer request.
   */
  static uint8_t reservedEncryptedConnections();

  EncryptedConnectionStatus initiateAutoEncryptingConnection(const EspnowNetworkInfo &recipientInfo, const bool requestPermanentConnection, uint8_t *targetBSSID, EncryptedConnectionLog **existingEncryptedConnection, EspnowMeshBackend &espnowInstance);
  void finalizeAutoEncryptingConnection(const uint8_t *targetBSSID, const EncryptedConnectionLog *existingEncryptedConnection, const bool requestPermanentConnection);

private:

  ConditionalPrinter & _conditionalPrinter;
  EspnowDatabase & _database;
  EspnowConnectionManager & _connectionManager;
  EspnowTransmitter & _transmitter;

  bool _receivedEncryptedTransmission = false;

  using encryptionRequestBuilderType = std::function<String(const String &, const ExpiringTimeTracker &)>;
  static String defaultEncryptionRequestBuilder(const String &requestHeader, const uint32_t durationMs, const uint8_t *hashKey, const String &requestNonce, const ExpiringTimeTracker &existingTimeTracker);
  static String flexibleEncryptionRequestBuilder(const uint32_t minDurationMs, const uint8_t *hashKey, const String &requestNonce, const ExpiringTimeTracker &existingTimeTracker);

  /**
   * Contains the core logic used for requesting an encrypted connection to a peerMac.
   * 
   * @param peerMac The MAC of the node with which an encrypted connection should be established.
   * @param encryptionRequestBuilder A function which is responsible for constructing the request message to send. 
   * Called twice when the request is successful. First to build the initial request message and then to build the connection verification message.
   * The request message should typically be of the form found in Serializer::createEncryptionRequestHmacMessage.
   * @param espnowInstance The EspnowMeshBackend instance that is requesting the encrypted connection.
   * @return The ultimate status of the requested encrypted connection, as EncryptedConnectionStatus.
   */
  EncryptedConnectionStatus requestEncryptedConnectionKernel(const uint8_t *peerMac, const encryptionRequestBuilderType &encryptionRequestBuilder, EspnowMeshBackend &espnowInstance);

  static bool verifyEncryptionRequestHmac(const String &encryptionRequestHmacMessage, const uint8_t *requesterStaMac, const uint8_t *requesterApMac, const uint8_t *hashKey, const uint8_t hashKeyLength);
};

#endif
