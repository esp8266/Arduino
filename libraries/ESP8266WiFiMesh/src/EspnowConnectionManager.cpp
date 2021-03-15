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

#include <ESP8266WiFi.h>
extern "C" {
  #include <espnow.h>
}

#include "EspnowConnectionManager.h"
#include "JsonTranslator.h"
#include "MeshCryptoInterface.h"
#include "Serializer.h"
#include "EspnowTransmitter.h"

namespace
{
  using EspnowProtocolInterpreter::encryptedConnectionKeyLength;
  using EspnowProtocolInterpreter::hashKeyLength;
  using EspnowProtocolInterpreter::maxEncryptedConnections;
    
  namespace TypeCast = MeshTypeConversionFunctions;

  std::vector<EncryptedConnectionLog> _encryptedConnections = {};
  
  uint32_t _unsynchronizedMessageID = 0;
 
  uint8_t _espnowEncryptionKok[encryptedConnectionKeyLength] = { 0 };
  bool _espnowEncryptionKokSet = false;
}

EspnowConnectionManager::EspnowConnectionManager(ConditionalPrinter &conditionalPrinterInstance, EspnowDatabase &databaseInstance) 
                                                  : _conditionalPrinter(conditionalPrinterInstance), _database(databaseInstance)
{
  // Reserve the maximum possible usage early on to prevent heap fragmentation later.
  encryptedConnections().reserve(maxEncryptedConnections);
}

std::vector<EncryptedConnectionLog> & EspnowConnectionManager::encryptedConnections() { return _encryptedConnections; }

uint8_t EspnowConnectionManager::numberOfEncryptedConnections()
{
  return encryptedConnections().size();
}

ConnectionType EspnowConnectionManager::getConnectionInfo(uint8_t *peerMac, uint32_t *remainingDuration)
{
  EncryptedConnectionLog *encryptedConnection = nullptr;
  
  if(peerMac)
    encryptedConnection = getEncryptedConnection(peerMac);
  
  return getConnectionInfoHelper(encryptedConnection, remainingDuration);
}

ConnectionType EspnowConnectionManager::getConnectionInfo(const uint32_t connectionIndex, uint32_t *remainingDuration, uint8_t *peerMac)
{
  EncryptedConnectionLog *encryptedConnection = nullptr;

  if(connectionIndex < numberOfEncryptedConnections())
    encryptedConnection = &encryptedConnections()[connectionIndex];

  return getConnectionInfoHelper(encryptedConnection, remainingDuration, peerMac);
}

EspnowConnectionManager::connectionLogIterator EspnowConnectionManager::connectionLogEndIterator()
{
  return encryptedConnections().end();
}

void EspnowConnectionManager::setEspnowEncryptedConnectionKey(const uint8_t espnowEncryptedConnectionKey[encryptedConnectionKeyLength])
{
  assert(espnowEncryptedConnectionKey != nullptr);
   
  for(int i = 0; i < encryptedConnectionKeyLength; ++i)
  {
    _espnowEncryptedConnectionKey[i] = espnowEncryptedConnectionKey[i];
  }
}

void EspnowConnectionManager::setEspnowEncryptedConnectionKey(const String &espnowEncryptedConnectionKeySeed)
{
  MeshCryptoInterface::initializeKey(_espnowEncryptedConnectionKey, encryptedConnectionKeyLength, espnowEncryptedConnectionKeySeed);
}

const uint8_t *EspnowConnectionManager::getEspnowEncryptedConnectionKey() const
{
  return _espnowEncryptedConnectionKey;
}

uint8_t *EspnowConnectionManager::getEspnowEncryptedConnectionKey(uint8_t resultArray[encryptedConnectionKeyLength]) const
{
  std::copy_n(_espnowEncryptedConnectionKey, encryptedConnectionKeyLength, resultArray);
  return resultArray;
}

bool EspnowConnectionManager::initializeEncryptionKok()
{
  // esp_now_set_kok returns 0 on success.
  return !(_espnowEncryptionKokSet && esp_now_set_kok(_espnowEncryptionKok, encryptedConnectionKeyLength));
}

bool EspnowConnectionManager::setEspnowEncryptionKok(uint8_t espnowEncryptionKok[encryptedConnectionKeyLength])
{
  if(espnowEncryptionKok == nullptr || esp_now_set_kok(espnowEncryptionKok, encryptedConnectionKeyLength)) // esp_now_set_kok failed if not == 0
    return false;
  
  for(int i = 0; i < encryptedConnectionKeyLength; ++i)
  {
    _espnowEncryptionKok[i] = espnowEncryptionKok[i];
  }

  _espnowEncryptionKokSet = true;
  
  return true;
}

bool EspnowConnectionManager::setEspnowEncryptionKok(const String &espnowEncryptionKokSeed)
{
  uint8_t espnowEncryptionKok[encryptedConnectionKeyLength] {};
  MeshCryptoInterface::initializeKey(espnowEncryptionKok, encryptedConnectionKeyLength, espnowEncryptionKokSeed);

  return setEspnowEncryptionKok(espnowEncryptionKok);
}

const uint8_t *EspnowConnectionManager::getEspnowEncryptionKok()
{
  if(_espnowEncryptionKokSet)
    return _espnowEncryptionKok;
  else
    return nullptr;
}

void EspnowConnectionManager::setEspnowHashKey(const uint8_t espnowHashKey[hashKeyLength])
{
  assert(espnowHashKey != nullptr);

  for(int i = 0; i < hashKeyLength; ++i)
  {
    _espnowHashKey[i] = espnowHashKey[i];
  }
}

void EspnowConnectionManager::setEspnowHashKey(const String &espnowHashKeySeed)
{
  MeshCryptoInterface::initializeKey(_espnowHashKey, hashKeyLength, espnowHashKeySeed);
}

const uint8_t *EspnowConnectionManager::getEspnowHashKey() const
{
  return _espnowHashKey;
}

uint8_t *EspnowConnectionManager::getEncryptedMac(const uint8_t *peerMac, uint8_t *resultArray)
{
  if(EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerMac))
  {
    return encryptedConnection->getEncryptedPeerMac(resultArray);
  }
  
  return nullptr;
}

EncryptedConnectionLog *EspnowConnectionManager::getEncryptedConnection(const uint8_t *peerMac)
{
  auto connectionIterator = getEncryptedConnectionIterator(peerMac, encryptedConnections());
  if(connectionIterator != encryptedConnections().end())
  {
    return &(*connectionIterator);
  }
  
  return nullptr;
}

EncryptedConnectionLog *EspnowConnectionManager::getEncryptedConnection(const uint32_t connectionIndex)
{
  if(connectionIndex < numberOfEncryptedConnections())
    return &encryptedConnections()[connectionIndex];
    
  return nullptr;
}

EncryptedConnectionLog *EspnowConnectionManager::getTemporaryEncryptedConnection(const uint8_t *peerMac)
{
  connectionLogIterator connectionIterator = connectionLogEndIterator();
  if(getTemporaryEncryptedConnectionIterator(peerMac, connectionIterator))
  {
    return &(*connectionIterator);
  }
  
  return nullptr;
}

template <typename T>
typename T::iterator EspnowConnectionManager::getEncryptedConnectionIterator(const uint8_t *peerMac, T &connectionContainer)
{
  typename T::iterator connectionIterator = connectionContainer.begin();
  
  while(connectionIterator != connectionContainer.end())
  {
    if(connectionIterator->connectedTo(peerMac))
      break; 
    else
      ++connectionIterator;
  }

  return connectionIterator;
}

bool EspnowConnectionManager::getEncryptedConnectionIterator(const uint8_t *peerMac, connectionLogIterator &iterator)
{
  connectionLogIterator result = getEncryptedConnectionIterator(peerMac, encryptedConnections());

  if(result != connectionLogEndIterator())
  {
    iterator = result;
    return true;
  }
  
  return false;
}

bool EspnowConnectionManager::getTemporaryEncryptedConnectionIterator(const uint8_t *peerMac, connectionLogIterator &iterator)
{
  connectionLogIterator result = connectionLogEndIterator();
  
  if(getEncryptedConnectionIterator(peerMac, result) && result->temporary())
  {
    iterator = result;
    return true;
  }
  
  return false;
}

ConnectionType EspnowConnectionManager::getConnectionInfoHelper(const EncryptedConnectionLog *encryptedConnection, uint32_t *remainingDuration, uint8_t *peerMac)
{
  if(!encryptedConnection)
  {
    return ConnectionType::NO_CONNECTION;
  }
  
  if(peerMac)
    encryptedConnection->getEncryptedPeerMac(peerMac);
  
  if(const ExpiringTimeTracker *timeTracker = encryptedConnection->temporary())
  {
    if(remainingDuration)
      *remainingDuration = timeTracker->remainingDuration();

    return ConnectionType::TEMPORARY_CONNECTION;
  }
  
  return ConnectionType::PERMANENT_CONNECTION;
}


void EspnowConnectionManager::setEncryptedConnectionsSoftLimit(const uint8_t softLimit) 
{ 
  assert(softLimit <= 6); // Valid values are 0 to 6, but uint8_t is always at least 0.
  _encryptedConnectionsSoftLimit = softLimit; 
}

uint8_t EspnowConnectionManager::encryptedConnectionsSoftLimit() const { return _encryptedConnectionsSoftLimit; }

bool EspnowConnectionManager::addUnencryptedConnection(const String &serializedConnectionState)
{
  return JsonTranslator::getUnsynchronizedMessageID(serializedConnectionState, _unsynchronizedMessageID);
}

EncryptedConnectionStatus EspnowConnectionManager::addEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey)
{
  assert(encryptedConnections().size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library

  uint8_t encryptionKeyArray[encryptedConnectionKeyLength] = { 0 };
  
  if(EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerStaMac))
  {
    // Encrypted connection with MAC already exists, so no need to replace it, just updating is enough.
    temporaryEncryptedConnectionToPermanent(peerStaMac);
    encryptedConnection->setPeerSessionKey(peerSessionKey);
    encryptedConnection->setOwnSessionKey(ownSessionKey);
    esp_now_set_peer_key(peerStaMac, getEspnowEncryptedConnectionKey(encryptionKeyArray), encryptedConnectionKeyLength);
    encryptedConnection->setHashKey(getEspnowHashKey());
    
    return EncryptedConnectionStatus::CONNECTION_ESTABLISHED;
  }

  if(encryptedConnections().size() == maxEncryptedConnections)
  {
    // No capacity for more encrypted connections.
    return EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_SELF;
  }
  // returns 0 on success: int esp_now_add_peer(u8 *mac_addr, u8 role, u8 channel, u8 *key, u8 key_len)
  // Only MAC, encryption key and key length (16) actually matter. The rest is not used by ESP-NOW.
  else if(0 == esp_now_add_peer(peerStaMac, ESP_NOW_ROLE_CONTROLLER, _database.getWiFiChannel(), getEspnowEncryptedConnectionKey(encryptionKeyArray), encryptedConnectionKeyLength))
  {
    encryptedConnections().emplace_back(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, getEspnowHashKey());
    return EncryptedConnectionStatus::CONNECTION_ESTABLISHED;
  }
  else
  {
    return EncryptedConnectionStatus::API_CALL_FAILED;
  }
}

EncryptedConnectionStatus EspnowConnectionManager::addEncryptedConnection(const String &serializedConnectionState, const bool ignoreDuration)
{
  uint32_t duration = 0;
  bool desync = false;
  uint64_t ownSessionKey = 0;
  uint64_t peerSessionKey = 0;
  uint8_t peerStaMac[6] = { 0 };
  uint8_t peerApMac[6] = { 0 };
  
  if(JsonTranslator::getDesync(serializedConnectionState, desync)
    && JsonTranslator::getOwnSessionKey(serializedConnectionState, ownSessionKey) && JsonTranslator::getPeerSessionKey(serializedConnectionState, peerSessionKey)
    && JsonTranslator::getPeerStaMac(serializedConnectionState, peerStaMac) && JsonTranslator::getPeerApMac(serializedConnectionState, peerApMac))
  {    
    EncryptedConnectionStatus result = EncryptedConnectionStatus::API_CALL_FAILED;
    
    if(!ignoreDuration && JsonTranslator::getDuration(serializedConnectionState, duration))
    {
      result = addTemporaryEncryptedConnection(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, duration);
    }
    else
    {
      result = addEncryptedConnection(peerStaMac, peerApMac, peerSessionKey, ownSessionKey);
    }

    if(result == EncryptedConnectionStatus::CONNECTION_ESTABLISHED)
    {
      EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerStaMac);
      encryptedConnection->setDesync(desync);
    }

    return result;
  }
  
  return EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
}

EncryptedConnectionStatus EspnowConnectionManager::addTemporaryEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint32_t duration)
{  
  assert(encryptedConnections().size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library

  uint8_t encryptionKeyArray[encryptedConnectionKeyLength] = { 0 };

  connectionLogIterator encryptedConnection = connectionLogEndIterator();
    
  if(getEncryptedConnectionIterator(peerStaMac, encryptedConnection))
  {
    // There is already an encrypted connection to this mac, so no need to replace it, just updating is enough.
    encryptedConnection->setPeerSessionKey(peerSessionKey);
    encryptedConnection->setOwnSessionKey(ownSessionKey);
    esp_now_set_peer_key(peerStaMac, getEspnowEncryptedConnectionKey(encryptionKeyArray), encryptedConnectionKeyLength);
    encryptedConnection->setHashKey(getEspnowHashKey());

    if(encryptedConnection->temporary())
    {
      encryptedConnection->setRemainingDuration(duration);
    }
    
    return EncryptedConnectionStatus::CONNECTION_ESTABLISHED;
  }
  
  EncryptedConnectionStatus result = addEncryptedConnection(peerStaMac, peerApMac, peerSessionKey, ownSessionKey);

  if(result == EncryptedConnectionStatus::CONNECTION_ESTABLISHED)
  {
    if(!getEncryptedConnectionIterator(peerStaMac, encryptedConnection))
      assert(false && String(F("No connection found despite being added in addTemporaryEncryptedConnection.")));
      
    encryptedConnection->setRemainingDuration(duration);
  }

  return result;
}

EncryptedConnectionStatus EspnowConnectionManager::addTemporaryEncryptedConnection(const String &serializedConnectionState, const uint32_t duration)
{
  bool desync = false;
  uint64_t ownSessionKey = 0;
  uint64_t peerSessionKey = 0;
  uint8_t peerStaMac[6] = { 0 };
  uint8_t peerApMac[6] = { 0 };
  
  if(JsonTranslator::getDesync(serializedConnectionState, desync)
    && JsonTranslator::getOwnSessionKey(serializedConnectionState, ownSessionKey) && JsonTranslator::getPeerSessionKey(serializedConnectionState, peerSessionKey)
    && JsonTranslator::getPeerStaMac(serializedConnectionState, peerStaMac) && JsonTranslator::getPeerApMac(serializedConnectionState, peerApMac))
  {        
    EncryptedConnectionStatus result = addTemporaryEncryptedConnection(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, duration);

    if(result == EncryptedConnectionStatus::CONNECTION_ESTABLISHED)
    {
      EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerStaMac);
      encryptedConnection->setDesync(desync);
    }

    return result;
  }
    
  return EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
}

EncryptedConnectionRemovalOutcome EspnowConnectionManager::removeEncryptedConnection(const uint8_t *peerMac)
{
  auto connectionIterator = getEncryptedConnectionIterator(peerMac, encryptedConnections());
  if(connectionIterator != encryptedConnections().end())
  {
    MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex());
    if(!mutexTracker.mutexCaptured())
    {
      // We should not remove an encrypted connection while there is a transmission in progress, since that may cause encrypted data to be sent unencrypted.
      // Thus when a transmission is in progress we just schedule the encrypted connection for removal, so it will be removed during the next updateTemporaryEncryptedConnections() call.
      connectionIterator->scheduleForRemoval();
      return EncryptedConnectionRemovalOutcome::REMOVAL_SCHEDULED;
    }
    else
    {     
      return removeEncryptedConnectionUnprotected(peerMac);
    }
  }
  
  // peerMac is already removed
  return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED;
}


EncryptedConnectionRemovalOutcome EspnowConnectionManager::removeEncryptedConnectionUnprotected(const uint8_t *peerMac, std::vector<EncryptedConnectionLog>::iterator *resultingIterator)
{
  connectionLogIterator connectionIterator = getEncryptedConnectionIterator(peerMac, encryptedConnections());
  return removeEncryptedConnectionUnprotected(connectionIterator, resultingIterator);
}

EncryptedConnectionRemovalOutcome EspnowConnectionManager::removeEncryptedConnectionUnprotected(connectionLogIterator &connectionIterator, std::vector<EncryptedConnectionLog>::iterator *resultingIterator)
{
  assert(encryptedConnections().size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library

  if(connectionIterator != connectionLogEndIterator())
  {
    uint8_t encryptedMac[6] {0};
    connectionIterator->getEncryptedPeerMac(encryptedMac);
    ConditionalPrinter::staticVerboseModePrint(String(F("Removing connection ")) + TypeCast::macToString(encryptedMac) + String(F("... ")), false);
    bool removalSucceeded = esp_now_del_peer(encryptedMac) == 0;
    
    if(removalSucceeded)
    {
      if(resultingIterator != nullptr)
      {
        *resultingIterator = encryptedConnections().erase(connectionIterator);
      }
      else
      {
        encryptedConnections().erase(connectionIterator);
      }
      ConditionalPrinter::staticVerboseModePrint(String(F("Removal succeeded")));
      
      // Not deleting encrypted responses here would cause them to be sent unencrypted, 
      // exposing the peer session key which can be misused later if the encrypted connection is re-established.
      EspnowDatabase::deleteScheduledResponsesByRecipient(encryptedMac, true);
      
      // Not deleting these entries here may cause issues if the encrypted connection is quickly re-added
      // and happens to get the same session keys as before (e.g. requestReceived() could then give false positives).
      EspnowDatabase::deleteEntriesByMac(EspnowDatabase::receivedEspnowTransmissions(), encryptedMac, true);
      EspnowDatabase::deleteEntriesByMac(EspnowDatabase::sentRequests(), encryptedMac, true);
      EspnowDatabase::deleteEntriesByMac(EspnowDatabase::receivedRequests(), encryptedMac, true);
      
      return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED;
    }
    else
    {
      ConditionalPrinter::staticVerboseModePrint(String(F("Removal failed")));
      return EncryptedConnectionRemovalOutcome::REMOVAL_FAILED;
    }
  }

  // connection is already removed
  return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED;
}

bool EspnowConnectionManager::temporaryEncryptedConnectionToPermanent(const uint8_t *peerMac)
{
  if(EncryptedConnectionLog *temporaryConnection = getTemporaryEncryptedConnection(peerMac))
  {
    temporaryConnection->removeDuration();
    return true;
  }
  
  return false;
}

String EspnowConnectionManager::serializeUnencryptedConnection()
{  
  return Serializer::serializeUnencryptedConnection(String(_unsynchronizedMessageID));
}

String EspnowConnectionManager::serializeEncryptedConnection(const uint8_t *peerMac)
{
  String serializedConnection(emptyString);
  
  EncryptedConnectionLog *encryptedConnection = nullptr;
  
  if(peerMac)
    encryptedConnection = getEncryptedConnection(peerMac);
  
  if(encryptedConnection)
    serializedConnection = encryptedConnection->serialize();

  return serializedConnection;
}

String EspnowConnectionManager::serializeEncryptedConnection(const uint32_t connectionIndex)
{
  String serializedConnection(emptyString);

  if(EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(connectionIndex))
     serializedConnection = encryptedConnection->serialize();
  
  return serializedConnection;
}

void EspnowConnectionManager::handlePostponedRemovals()
{
  MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex());
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call handlePostponedRemovals from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }
  
  if(EncryptedConnectionLog::newRemovalsScheduled())
  {
    updateTemporaryEncryptedConnections(true);
  }
}

void EspnowConnectionManager::updateTemporaryEncryptedConnections(const bool scheduledRemovalOnly)
{  
  EncryptedConnectionLog::clearSoonestExpiringConnectionTracker();
  
  for(auto connectionIterator = encryptedConnections().begin(); connectionIterator != encryptedConnections().end(); ) 
  {
    if(auto timeTrackerPointer = connectionIterator->temporary()) 
    {
      if(timeTrackerPointer->expired() && (!scheduledRemovalOnly || connectionIterator->removalScheduled())) 
      {
        uint8_t macArray[6] = { 0 };
        removeEncryptedConnectionUnprotected(connectionIterator->getEncryptedPeerMac(macArray), &connectionIterator);
        continue;
      } 
      else 
      {        
        EncryptedConnectionLog::updateSoonestExpiringConnectionTracker(timeTrackerPointer->remainingDuration());
      }
    }
    assert(!connectionIterator->removalScheduled()); // timeTracker should always exist and be expired if removal is scheduled.
    
    ++connectionIterator;   
  }

  EncryptedConnectionLog::setNewRemovalsScheduled(false);
}

uint64_t EspnowConnectionManager::generateMessageID(const EncryptedConnectionLog *encryptedConnection)
{
  if(encryptedConnection)
  {
    return encryptedConnection->getOwnSessionKey();
  }

  return _unsynchronizedMessageID++;
}
