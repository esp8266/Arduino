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

#ifndef __ESPNOWCONNECTIONMANAGER_H__
#define __ESPNOWCONNECTIONMANAGER_H__

#include <Arduino.h>
#include "ConditionalPrinter.h"
#include "EspnowDatabase.h"
#include "EspnowProtocolInterpreter.h"
#include "EncryptedConnectionLog.h"

class EspnowMeshBackend;

enum class ConnectionType
{
  NO_CONNECTION          = 0,
  TEMPORARY_CONNECTION   = 1,
  PERMANENT_CONNECTION   = 2
};

// A value greater than 0 means that an encrypted connection has been established.
enum class EncryptedConnectionStatus
{
  MAX_CONNECTIONS_REACHED_SELF      = -3,
  REQUEST_TRANSMISSION_FAILED       = -2,
  MAX_CONNECTIONS_REACHED_PEER      = -1,
  API_CALL_FAILED                   = 0,
  CONNECTION_ESTABLISHED            = 1,
  SOFT_LIMIT_CONNECTION_ESTABLISHED = 2 // Only used if _encryptedConnectionsSoftLimit is less than 6. See the setEncryptedConnectionsSoftLimit method documentation for details.
};

enum class EncryptedConnectionRemovalOutcome
{
  REMOVAL_REQUEST_FAILED   = -1,
  REMOVAL_FAILED           = 0,
  REMOVAL_SUCCEEDED        = 1,
  REMOVAL_SCHEDULED        = 2
};

class EspnowConnectionManager
{

public:

  using connectionLogIterator = std::vector<EncryptedConnectionLog>::iterator;
  
  EspnowConnectionManager(ConditionalPrinter &conditionalPrinterInstance, EspnowDatabase &databaseInstance);

  static std::vector<EncryptedConnectionLog> & encryptedConnections();
  
  static uint8_t numberOfEncryptedConnections();
  
  static ConnectionType getConnectionInfo(uint8_t *peerMac, uint32_t *remainingDuration = nullptr);
  static ConnectionType getConnectionInfo(const uint32_t connectionIndex, uint32_t *remainingDuration = nullptr, uint8_t *peerMac = nullptr);
  
  static connectionLogIterator connectionLogEndIterator();
  
  void setEspnowEncryptedConnectionKey(const uint8_t espnowEncryptedConnectionKey[EspnowProtocolInterpreter::encryptedConnectionKeyLength]);
  void setEspnowEncryptedConnectionKey(const String &espnowEncryptedConnectionKeySeed);
  const uint8_t *getEspnowEncryptedConnectionKey() const;
  uint8_t *getEspnowEncryptedConnectionKey(uint8_t resultArray[EspnowProtocolInterpreter::encryptedConnectionKeyLength]) const; 
  // Returns false if failed to apply the current KoK (default KoK is used if no KoK provided)
  static bool initializeEncryptionKok();
  static bool setEspnowEncryptionKok(uint8_t espnowEncryptionKok[EspnowProtocolInterpreter::encryptedConnectionKeyLength]);
  static bool setEspnowEncryptionKok(const String &espnowEncryptionKokSeed);
  static const uint8_t *getEspnowEncryptionKok();
  void setEspnowHashKey(const uint8_t espnowHashKey[EspnowProtocolInterpreter::hashKeyLength]);
  void setEspnowHashKey(const String &espnowHashKeySeed);
  const uint8_t *getEspnowHashKey() const;

  static uint8_t *getEncryptedMac(const uint8_t *peerMac, uint8_t *resultArray);

  static EncryptedConnectionLog *getEncryptedConnection(const uint8_t *peerMac);
  static EncryptedConnectionLog *getEncryptedConnection(const uint32_t connectionIndex);
  static EncryptedConnectionLog *getTemporaryEncryptedConnection(const uint8_t *peerMac);
  
  //@return iterator to connection in connectionContainer, or connectionContainer.end() if element not found
  template <typename T>
  static typename T::iterator getEncryptedConnectionIterator(const uint8_t *peerMac, T &connectionContainer);
  static bool getEncryptedConnectionIterator(const uint8_t *peerMac, connectionLogIterator &iterator);
  // @return true if an encrypted connection to peerMac is found and the found connection is temporary. Only changes iterator if true is returned.
  static bool getTemporaryEncryptedConnectionIterator(const uint8_t *peerMac, connectionLogIterator &iterator);
    
  void setEncryptedConnectionsSoftLimit(const uint8_t softLimit);
  uint8_t encryptedConnectionsSoftLimit() const;
  
  static bool addUnencryptedConnection(const String &serializedConnectionState);
  EncryptedConnectionStatus addEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey);
  EncryptedConnectionStatus addEncryptedConnection(const String &serializedConnectionState, const bool ignoreDuration = false);
  EncryptedConnectionStatus addTemporaryEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint32_t duration);
  EncryptedConnectionStatus addTemporaryEncryptedConnection(const String &serializedConnectionState, const uint32_t duration);
  
  static EncryptedConnectionRemovalOutcome removeEncryptedConnection(const uint8_t *peerMac);
  
  // Note that removing an encrypted connection while there are encrypted responses scheduled for transmission to the encrypted peer will cause these encrypted responses to be removed without being sent.
  // Also note that removing an encrypted connection while there is encrypted data to be received will make the node unable to decrypt that data (although an ack will still be sent to confirm data reception).
  // In other words, it is good to use these methods with care and to make sure that both nodes in an encrypted pair are in a state where it is safe for the encrypted connection to be removed before using them.
  // Consider using getScheduledResponseRecipient and similar methods for this preparation.
  // Should only be used when there is no transmissions in progress. In practice when _espnowTransmissionMutex is free.
  // @param resultingIterator Will be set to the iterator position after the removed element, if an element to remove was found. Otherwise no change will occur.
  static EncryptedConnectionRemovalOutcome removeEncryptedConnectionUnprotected(const uint8_t *peerMac, std::vector<EncryptedConnectionLog>::iterator *resultingIterator = nullptr);
  static EncryptedConnectionRemovalOutcome removeEncryptedConnectionUnprotected(connectionLogIterator &connectionIterator, std::vector<EncryptedConnectionLog>::iterator *resultingIterator);

  static bool temporaryEncryptedConnectionToPermanent(const uint8_t *peerMac);

  static String serializeUnencryptedConnection();  
  static String serializeEncryptedConnection(const uint8_t *peerMac);
  static String serializeEncryptedConnection(const uint32_t connectionIndex);
  
  static void handlePostponedRemovals();
  
  // Should only be used when there is no transmissions in progress, so it is safe to remove encrypted connections. In practice when _espnowTransmissionMutex is free.
  // @param scheduledRemovalOnly If true, only deletes encrypted connections where removalScheduled() is true. This means only connections which have been requested for removal will be deleted,
  // not other connections which have expired.
  static void updateTemporaryEncryptedConnections(const bool scheduledRemovalOnly = false);
  
  /**
   * Generate a new message ID to be used when making a data transmission. The generated ID will be different depending on whether an encrypted connection exists or not.
   * 
   * @param encryptedConnection A pointer to the EncryptedConnectionLog of the encrypted connection. Can be set to nullptr if the connection is unecrypted.
   * @return The generated message ID.
   */
  static uint64_t generateMessageID(const EncryptedConnectionLog *encryptedConnection);

private:

  ConditionalPrinter & _conditionalPrinter;
  EspnowDatabase & _database;
  
  static ConnectionType getConnectionInfoHelper(const EncryptedConnectionLog *encryptedConnection, uint32_t *remainingDuration, uint8_t *peerMac = nullptr);

  uint8_t _espnowEncryptedConnectionKey[EspnowProtocolInterpreter::encryptedConnectionKeyLength] {0};
  uint8_t _espnowHashKey[EspnowProtocolInterpreter::hashKeyLength] {0};
  
  uint8_t _encryptedConnectionsSoftLimit = 6;
};

#endif
