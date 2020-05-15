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

#include "EspnowEncryptionBroker.h"
#include "EspnowMeshBackend.h"
#include "JsonTranslator.h"
#include "UtilityFunctions.h"
#include "Serializer.h"
#include "MeshCryptoInterface.h"

namespace
{
  using EspnowProtocolInterpreter::encryptedConnectionKeyLength;
  using EspnowProtocolInterpreter::hashKeyLength;
  using EspnowProtocolInterpreter::maxEncryptedConnections;
  
  using connectionLogIterator = EspnowConnectionManager::connectionLogIterator;
  
  namespace TypeCast = MeshTypeConversionFunctions;
  
  String _ongoingPeerRequestNonce;
  uint8_t _ongoingPeerRequestMac[6] = {0};
  EspnowMeshBackend *_ongoingPeerRequester = nullptr;
  EncryptedConnectionStatus _ongoingPeerRequestResult = EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_SELF;
  ExpiringTimeTracker _ongoingPeerRequestEncryptionTimeout([](){ return EspnowDatabase::getEncryptionRequestTimeout(); });
  bool _reciprocalPeerRequestConfirmation = false;
}

EspnowEncryptionBroker::EspnowEncryptionBroker(ConditionalPrinter &conditionalPrinterInstance, EspnowDatabase &databaseInstance, EspnowConnectionManager &connectionManagerInstance, EspnowTransmitter &transmitterInstance)
   : _conditionalPrinter(conditionalPrinterInstance), _database(databaseInstance), _connectionManager(connectionManagerInstance), _transmitter(transmitterInstance)
{
}

void EspnowEncryptionBroker::handlePeerRequest(const uint8_t *macaddr, uint8_t *dataArray, const uint8_t len, const uint64_t uint64StationMac, const uint64_t receivedMessageID)
{  
  // Pairing process ends when encryptedConnectionVerificationHeader is received, maxConnectionsReachedHeader is sent or timeout is reached.
  // Pairing process stages for request receiver:
  // Receive: encryptionRequestHeader or temporaryEncryptionRequestHeader.
  // Send: maxConnectionsReachedHeader / basicConnectionInfoHeader -> encryptedConnectionInfoHeader or softLimitEncryptedConnectionInfoHeader or maxConnectionsReachedHeader.
  // Receive: encryptedConnectionVerificationHeader.
  
  using namespace EspnowProtocolInterpreter;
  
  if(!EspnowDatabase::requestReceived(uint64StationMac, receivedMessageID))
  {
    EspnowDatabase::storeReceivedRequest(uint64StationMac, receivedMessageID, TimeTracker(millis()));
    
    bool encryptedCorrectly = synchronizePeerSessionKey(receivedMessageID, macaddr);
    String message = getHashKeyLength(dataArray, len);
    int32_t messageHeaderEndIndex = message.indexOf(':');
    String messageHeader = message.substring(0, messageHeaderEndIndex + 1);

    if(messageHeader == FPSTR(encryptedConnectionVerificationHeader))
    {
      if(encryptedCorrectly)
      {
        int32_t connectionRequestTypeEndIndex = message.indexOf(':', messageHeaderEndIndex + 1);
        String connectionRequestType = message.substring(messageHeaderEndIndex + 1, connectionRequestTypeEndIndex + 1);
        connectionLogIterator encryptedConnection = EspnowConnectionManager::connectionLogEndIterator();
        if(!EspnowConnectionManager::getEncryptedConnectionIterator(macaddr, encryptedConnection))
          assert(false && String(F("We must have an encrypted connection if we received an encryptedConnectionVerificationHeader which was encryptedCorrectly.")));
        
        if(connectionRequestType == FPSTR(encryptionRequestHeader))
        {
          EspnowConnectionManager::temporaryEncryptedConnectionToPermanent(macaddr);
        }
        else if(connectionRequestType == FPSTR(temporaryEncryptionRequestHeader))
        {
          if(encryptedConnection->temporary()) // Should not change duration for existing permanent connections.
          {
            uint32_t connectionDuration = 0;
            if(JsonTranslator::getDuration(message, connectionDuration))
            {
              encryptedConnection->setRemainingDuration(connectionDuration);
            }
          }
        }
        else
        {
          assert(false && String(F("Unknown P-type verification message received!")));
        }
      }
    }
    else if(messageHeader == FPSTR(encryptionRequestHeader) || messageHeader == FPSTR(temporaryEncryptionRequestHeader))
    {
      // If there is a espnowRequestManager, get it
      if(EspnowMeshBackend *currentEspnowRequestManager = EspnowMeshBackend::getEspnowRequestManager())
      {
        String requestNonce;

        if(JsonTranslator::getNonce(message, requestNonce) && requestNonce.length() >= 12) // The destination MAC address requires 12 characters.
        {
          uint8_t destinationMac[6] = {0};
          TypeCast::stringToMac(requestNonce, destinationMac);
        
          uint8_t apMac[6] {0};
          WiFi.softAPmacAddress(apMac);

          bool correctDestination = false;
          if(MeshUtilityFunctions::macEqual(destinationMac, apMac))
          {
            correctDestination = true;
          }
          else
          {
            uint8_t staMac[6] {0};
            WiFi.macAddress(staMac);
            
            if(MeshUtilityFunctions::macEqual(destinationMac, staMac))
            {
              correctDestination = true;
            }
          }

          uint8_t apMacArray[6] = { 0 };
          if(correctDestination && verifyEncryptionRequestHmac(message, macaddr, getTransmissionMac(dataArray, apMacArray), currentEspnowRequestManager->getEspnowHashKey(), hashKeyLength))
            EspnowDatabase::peerRequestConfirmationsToSend().emplace_back(receivedMessageID, encryptedCorrectly, currentEspnowRequestManager->getMeshPassword(), currentEspnowRequestManager->encryptedConnectionsSoftLimit(), 
                                                        requestNonce, macaddr, apMacArray, currentEspnowRequestManager->getEspnowHashKey());
        }
      }
    }
    else if(messageHeader == FPSTR(encryptedConnectionRemovalRequestHeader))
    {
      if(encryptedCorrectly)
        EspnowConnectionManager::removeEncryptedConnection(macaddr);
    }
    else
    {
      assert(false && String(F("Unknown P-type message received!")));
    }
  }
}

void EspnowEncryptionBroker::handlePeerRequestConfirmation(uint8_t *macaddr, uint8_t *dataArray, const uint8_t len)
{
  // Pairing process ends when _ongoingPeerRequestNonce == "" or timeout is reached.
  // Pairing process stages for request sender:
  // Send: encryptionRequestHeader or temporaryEncryptionRequestHeader.
  // Receive: maxConnectionsReachedHeader / basicConnectionInfoHeader -> encryptedConnectionInfoHeader or softLimitEncryptedConnectionInfoHeader or maxConnectionsReachedHeader.
  // Send: encryptedConnectionVerificationHeader.
  
  using namespace EspnowProtocolInterpreter;
  
  if(!_ongoingPeerRequestNonce.isEmpty())
  {
    String message = getHashKeyLength(dataArray, len);
    String requestNonce;

    if(JsonTranslator::getNonce(message, requestNonce) && requestNonce == _ongoingPeerRequestNonce)
    {      
      int32_t messageHeaderEndIndex = message.indexOf(':');
      String messageHeader = message.substring(0, messageHeaderEndIndex + 1);
      String messageBody = message.substring(messageHeaderEndIndex + 1);
      uint8_t apMacArray[6] = { 0 };
      getTransmissionMac(dataArray, apMacArray);

      if(messageHeader == FPSTR(basicConnectionInfoHeader))
      {
        // encryptedConnectionEstablished(_ongoingPeerRequestResult) means we have already received a basicConnectionInfoHeader
        if(!encryptedConnectionEstablished(_ongoingPeerRequestResult) &&
           verifyEncryptionRequestHmac(message, macaddr, apMacArray, _ongoingPeerRequester->getEspnowHashKey(), hashKeyLength))
        {
           _ongoingPeerRequestEncryptionTimeout.reset();
  
          connectionLogIterator existingEncryptedConnection = EspnowConnectionManager::connectionLogEndIterator();
          
          if(!EspnowConnectionManager::getEncryptedConnectionIterator(macaddr, existingEncryptedConnection))
          {
            // Although the newly created session keys are normally never used (they are replaced with synchronized ones later), the session keys must still be randomized to prevent attacks until replaced.
            _ongoingPeerRequestResult = _ongoingPeerRequester->addTemporaryEncryptedConnection(macaddr, apMacArray, createSessionKey(), createSessionKey(), EspnowDatabase::getEncryptionRequestTimeout());
          }
          else 
          {
            // Encrypted connection already exists
            _ongoingPeerRequestResult = EncryptedConnectionStatus::CONNECTION_ESTABLISHED;
            
            if(auto timeTrackerPointer = existingEncryptedConnection->temporary())
            {
              if(timeTrackerPointer->remainingDuration() < EspnowDatabase::getEncryptionRequestTimeout()) // Should only extend duration for existing connections.
              {
                existingEncryptedConnection->setRemainingDuration(EspnowDatabase::getEncryptionRequestTimeout());
              }
            }
          }
  
          if(!encryptedConnectionEstablished(_ongoingPeerRequestResult))
          {
            // Adding connection failed, abort ongoing peer request.
            _ongoingPeerRequestNonce.clear();
          }
        }
      }
      else if(messageHeader == FPSTR(encryptedConnectionInfoHeader) || messageHeader == FPSTR(softLimitEncryptedConnectionInfoHeader))
      {
        String messagePassword;
        
        if(JsonTranslator::getPassword(messageBody, messagePassword) && messagePassword == _ongoingPeerRequester->getMeshPassword())
        {
          // The mesh password is only shared via encrypted messages, so now we know this message is valid since it was encrypted and contained the correct nonce.
          
          EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(macaddr);
          uint64_t peerSessionKey = 0;
          uint64_t ownSessionKey = 0;
          if(encryptedConnection && JsonTranslator::getPeerSessionKey(messageBody, peerSessionKey) && JsonTranslator::getOwnSessionKey(messageBody, ownSessionKey))
          {
            encryptedConnection->setPeerSessionKey(peerSessionKey);
            encryptedConnection->setOwnSessionKey(ownSessionKey);

            if(messageHeader == FPSTR(encryptedConnectionInfoHeader))
              _ongoingPeerRequestResult = EncryptedConnectionStatus::CONNECTION_ESTABLISHED;
            else if(messageHeader == FPSTR(softLimitEncryptedConnectionInfoHeader))
              _ongoingPeerRequestResult = EncryptedConnectionStatus::SOFT_LIMIT_CONNECTION_ESTABLISHED;
            else
              assert(false && String(F("Unknown _ongoingPeerRequestResult!")));
          }
          else
          {
            _ongoingPeerRequestResult = EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
          }

          _ongoingPeerRequestNonce.clear();
        }
      }
      else if(messageHeader == FPSTR(maxConnectionsReachedHeader))
      {
        if(verifyEncryptionRequestHmac(message, macaddr, apMacArray, _ongoingPeerRequester->getEspnowHashKey(), hashKeyLength))
        {
          _ongoingPeerRequestResult = EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_PEER;
          _ongoingPeerRequestNonce.clear();
        }
      }
      else
      {
        assert(messageHeader == FPSTR(basicConnectionInfoHeader) || messageHeader == FPSTR(encryptedConnectionInfoHeader) || 
               messageHeader == FPSTR(softLimitEncryptedConnectionInfoHeader) || messageHeader == FPSTR(maxConnectionsReachedHeader));
      }
    }
  }
}

void EspnowEncryptionBroker::sendPeerRequestConfirmations(const ExpiringTimeTracker *estimatedMaxDurationTracker)
{
  uint32_t bufferedCriticalHeapLevel = EspnowDatabase::criticalHeapLevel() + EspnowDatabase::criticalHeapLevelBuffer(); // We preferably want to start clearing the logs a bit before things get critical.
  // _ongoingPeerRequestNonce can change during every delay(), but we need to remember the initial value to know from where sendPeerRequestConfirmations was called.
  String initialOngoingPeerRequestNonce = _ongoingPeerRequestNonce;
  
  for(std::list<PeerRequestLog>::iterator confirmationsIterator = EspnowDatabase::peerRequestConfirmationsToSend().begin(); confirmationsIterator != EspnowDatabase::peerRequestConfirmationsToSend().end(); )
  {     
    using namespace EspnowProtocolInterpreter;

    // True if confirmationsIterator contains a peer request received from the same node we are currently sending a peer request to.
    bool reciprocalPeerRequest = !initialOngoingPeerRequestNonce.isEmpty() && confirmationsIterator->connectedTo(_ongoingPeerRequestMac);
         
    auto timeTrackerPointer = confirmationsIterator->temporary();
    assert(timeTrackerPointer); // peerRequestConfirmations should always expire and so should always have a timeTracker
    if(timeTrackerPointer->elapsedTime() > EspnowDatabase::getEncryptionRequestTimeout() 
       || (reciprocalPeerRequest && confirmationsIterator->getPeerRequestNonce() <= initialOngoingPeerRequestNonce))
    {
      // The peer request has expired, 
      // or the peer request comes from the node we are currently making a peer request to ourselves and we are supposed to wait in this event to avoid simultaneous session key transfer.
      ++confirmationsIterator;
      continue;
    }

    uint8_t defaultBSSID[6] {0};
    confirmationsIterator->getEncryptedPeerMac(defaultBSSID);
    uint8_t unencryptedBSSID[6] {0};
    confirmationsIterator->getUnencryptedPeerMac(unencryptedBSSID);
    uint8_t hashKey[hashKeyLength] {0};
    confirmationsIterator->getHashKey(hashKey);

    EncryptedConnectionLog *existingEncryptedConnection = EspnowConnectionManager::getEncryptedConnection(defaultBSSID);
    
    // If we receive a non-encrypted request for encrypted connection from a node that already exists as an encrypted peer for us we cannot send a response to the encrypted MAC 
    // since that transmission will then be encrypted and impossible for the request sender to read. Of course, removing the existing encrypted connection would also work,
    // but make it very simple for a third party to disrupt an encrypted connection by just sending random requests for encrypted connection.
    bool sendToDefaultBSSID = confirmationsIterator->requestEncrypted() || !existingEncryptedConnection;
    
    // Note that callbacks can be called during delay time, so it is possible to receive a transmission during espnowSendToNode
    // (which may add an element to the peerRequestConfirmationsToSend list).

    if(!existingEncryptedConnection && 
       ((reciprocalPeerRequest && EspnowConnectionManager::encryptedConnections().size() >= maxEncryptedConnections) || (!reciprocalPeerRequest && reservedEncryptedConnections() >= maxEncryptedConnections)))
    {
      EspnowTransmitter::espnowSendPeerRequestConfirmationsUnsynchronized(Serializer::createEncryptionRequestHmacMessage(FPSTR(maxConnectionsReachedHeader), 
                                                                     confirmationsIterator->getPeerRequestNonce(), hashKey, hashKeyLength),
                                                                     defaultBSSID, 'C'); // Generates a new message ID to avoid sending encrypted sessionKeys over unencrypted connections.
                                                        
      confirmationsIterator = EspnowDatabase::peerRequestConfirmationsToSend().erase(confirmationsIterator);
    }
    else if(EspnowTransmitter::espnowSendPeerRequestConfirmationsUnsynchronized(Serializer::createEncryptionRequestHmacMessage(FPSTR(basicConnectionInfoHeader),
                                                                            confirmationsIterator->getPeerRequestNonce(), hashKey, hashKeyLength),
                                                                            sendToDefaultBSSID ? defaultBSSID : unencryptedBSSID, 'C') // Generates a new message ID to avoid sending encrypted sessionKeys over unencrypted connections.
                                                              == TransmissionStatusType::TRANSMISSION_COMPLETE)
    {            
      // Try to add encrypted connection. If connection added send confirmation with encryptedConnection->getOwnSessionKey() as session key and C type message (won't increment key). Then proceed with next request (no need to wait for answer).
      if(existingEncryptedConnection)
      {
        if(auto timeTrackerPointer = existingEncryptedConnection->temporary())
        {
          if(EspnowDatabase::getEncryptionRequestTimeout() > timeTrackerPointer->remainingDuration())
          {
            existingEncryptedConnection->setRemainingDuration(EspnowDatabase::getEncryptionRequestTimeout());
          }
        }
      }
      else if(EspnowMeshBackend *currentEspnowRequestManager = EspnowMeshBackend::getEspnowRequestManager())
      {
        uint8_t staMacArray[6] = { 0 };
        uint8_t apMacArray[6] = { 0 };
        currentEspnowRequestManager->addTemporaryEncryptedConnection(confirmationsIterator->getPeerStaMac(staMacArray), confirmationsIterator->getPeerApMac(apMacArray),
                                                                     createSessionKey(), createSessionKey(), EspnowDatabase::getEncryptionRequestTimeout());
        existingEncryptedConnection = EspnowConnectionManager::getEncryptedConnection(defaultBSSID);
      }
      else
      {
        ConditionalPrinter::warningPrint(String(F("WARNING! Ignoring received encrypted connection request since no EspnowRequestManager is assigned.")));
      }
      
      if(!existingEncryptedConnection)
      {
        // Send "node full" message
        EspnowTransmitter::espnowSendPeerRequestConfirmationsUnsynchronized(Serializer::createEncryptionRequestHmacMessage(FPSTR(maxConnectionsReachedHeader), 
                                                                        confirmationsIterator->getPeerRequestNonce(), hashKey, hashKeyLength), 
                                                                        defaultBSSID, 'C'); // Generates a new message ID to avoid sending encrypted sessionKeys over unencrypted connections.
      }
      else
      {
        if(reciprocalPeerRequest)
          _reciprocalPeerRequestConfirmation = true;
        
        delay(5); // Give some time for the peer to add an encrypted connection
        
        assert(esp_now_is_peer_exist(defaultBSSID) > 0 && String(F("ERROR! Attempting to send content marked as encrypted via unencrypted connection!")));

        String messageHeader;

        if(existingEncryptedConnection->temporary() && // Should never change permanent connections
          ((reciprocalPeerRequest && EspnowConnectionManager::encryptedConnections().size() > confirmationsIterator->getEncryptedConnectionsSoftLimit()) 
            || (!reciprocalPeerRequest && reservedEncryptedConnections() > confirmationsIterator->getEncryptedConnectionsSoftLimit())))
        {
          messageHeader = FPSTR(softLimitEncryptedConnectionInfoHeader);
        }
        else
        {
          messageHeader = FPSTR(encryptedConnectionInfoHeader);
        }
        
        // Send password and keys.
        // Probably no need to know which connection type to use, that is stored in request node and will be sent over for finalization.
        EspnowTransmitter::espnowSendPeerRequestConfirmationsUnsynchronized(Serializer::createEncryptedConnectionInfo(messageHeader,
                                                                        confirmationsIterator->getPeerRequestNonce(), confirmationsIterator->getAuthenticationPassword(), 
                                                                        existingEncryptedConnection->getOwnSessionKey(), existingEncryptedConnection->getPeerSessionKey()),
                                                                        defaultBSSID, 'C');  // Generates a new message ID to avoid sending encrypted sessionKeys over unencrypted connections.
      }
    
      confirmationsIterator = EspnowDatabase::peerRequestConfirmationsToSend().erase(confirmationsIterator);
    }
    else
    {
      ++confirmationsIterator;
    }

    if(ESP.getFreeHeap() <= bufferedCriticalHeapLevel)
    {
      // Heap is getting very low, which probably means we are receiving a lot of transmissions while trying to transmit responses.
      // Clear all old data to try to avoid running out of memory.
      ConditionalPrinter::warningPrint("WARNING! Free heap below chosen minimum. Performing emergency log clearing.");
      EspnowDatabase::clearOldLogEntries(true);
      return; // confirmationsIterator may be invalid now. Also, we should give the main loop a chance to respond to the situation.
    }

    if(estimatedMaxDurationTracker && estimatedMaxDurationTracker->expired())
      return;
  }
}

EncryptedConnectionStatus EspnowEncryptionBroker::requestEncryptedConnection(const uint8_t *peerMac, EspnowMeshBackend &espnowInstance)
{
  using namespace std::placeholders;
  return requestEncryptedConnectionKernel(peerMac, std::bind(defaultEncryptionRequestBuilder, FPSTR(EspnowProtocolInterpreter::encryptionRequestHeader), 0, _connectionManager.getEspnowHashKey(), _1, _2), espnowInstance);
}

EncryptedConnectionStatus EspnowEncryptionBroker::requestTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t durationMs, EspnowMeshBackend &espnowInstance)
{
  using namespace std::placeholders;
  return requestEncryptedConnectionKernel(peerMac, std::bind(defaultEncryptionRequestBuilder, FPSTR(EspnowProtocolInterpreter::temporaryEncryptionRequestHeader), 
                                                             durationMs, _connectionManager.getEspnowHashKey(), _1, _2), espnowInstance);
}

EncryptedConnectionStatus EspnowEncryptionBroker::requestFlexibleTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t minDurationMs, EspnowMeshBackend &espnowInstance)
{
  using namespace std::placeholders;
  return requestEncryptedConnectionKernel(peerMac, std::bind(flexibleEncryptionRequestBuilder, minDurationMs, _connectionManager.getEspnowHashKey(), _1, _2), espnowInstance);
}

EncryptedConnectionRemovalOutcome EspnowEncryptionBroker::requestEncryptedConnectionRemoval(const uint8_t *peerMac)
{
  using EspnowProtocolInterpreter::encryptedConnectionRemovalRequestHeader;
  
  assert(EspnowConnectionManager::encryptedConnections().size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library
  
  MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex(EspnowConnectionManager::handlePostponedRemovals));
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call requestEncryptedConnectionRemoval from callbacks as this may corrupt program state! Aborting."))); 
    return EncryptedConnectionRemovalOutcome::REMOVAL_REQUEST_FAILED;
  }

  if(EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(peerMac))
  {
    if(EspnowTransmitter::espnowSendToNode(FPSTR(encryptedConnectionRemovalRequestHeader), peerMac, 'P') == TransmissionStatusType::TRANSMISSION_COMPLETE)
    {
      return EspnowConnectionManager::removeEncryptedConnectionUnprotected(peerMac);
    }
    else
    {
      if(encryptedConnection->removalScheduled())
        return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED; // Removal will be completed by mutex destructorHook.
      else
        return EncryptedConnectionRemovalOutcome::REMOVAL_REQUEST_FAILED;
    }
  }
  
  // peerMac is already removed
  return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED;
}

bool EspnowEncryptionBroker::encryptedConnectionEstablished(const EncryptedConnectionStatus connectionStatus)
{
  return static_cast<int>(connectionStatus) > 0;
}

void EspnowEncryptionBroker::setReceivedEncryptedTransmission(const bool receivedEncryptedTransmission) { _receivedEncryptedTransmission = receivedEncryptedTransmission; }
bool EspnowEncryptionBroker::receivedEncryptedTransmission() const {return _receivedEncryptedTransmission;}

bool EspnowEncryptionBroker::verifyEncryptionRequestHmac(const String &encryptionRequestHmacMessage, const uint8_t *requesterStaMac, const uint8_t *requesterApMac, 
                                 const uint8_t *hashKey, const uint8_t hashKeyLength)
{
  using MeshCryptoInterface::verifyMeshHmac;
  using namespace JsonTranslator;
  
  String hmac;
  if(getHmac(encryptionRequestHmacMessage, hmac))
  {
    int32_t hmacStartIndex = encryptionRequestHmacMessage.indexOf(String('"') + FPSTR(jsonHmac) + F("\":"));
    if(hmacStartIndex < 0)
      return false;
   
    if(hmac.length() == 2*CryptoInterface::SHA256_NATURAL_LENGTH // We know that each HMAC byte should become 2 String characters due to uint8ArrayToHexString.
       && verifyMeshHmac(TypeCast::macToString(requesterStaMac) + TypeCast::macToString(requesterApMac) + encryptionRequestHmacMessage.substring(0, hmacStartIndex), hmac, hashKey, hashKeyLength))
    {
      return true;
    }
  }

  return false;
}

bool EspnowEncryptionBroker::verifyPeerSessionKey(const uint64_t sessionKey, const uint8_t *peerMac, const char messageType)
{
  if(EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(peerMac))
  {
    return verifyPeerSessionKey(sessionKey, *encryptedConnection, TypeCast::macToUint64(peerMac), messageType);
  }

  return false;
}

bool EspnowEncryptionBroker::verifyPeerSessionKey(const uint64_t sessionKey, const EncryptedConnectionLog &encryptedConnection, const uint64_t uint64PeerMac, const char messageType)
{
  using namespace EspnowProtocolInterpreter;
  
  if(usesEncryption(sessionKey))
  {
    if(sessionKey == encryptedConnection.getPeerSessionKey() 
       || EspnowDatabase::receivedEspnowTransmissions().find(std::make_pair(createMacAndTypeValue(uint64PeerMac, messageType), sessionKey)) 
          != EspnowDatabase::receivedEspnowTransmissions().end())
    {
      // If sessionKey is correct or sessionKey is one part of a multi-part transmission.
      return true;
    }
  }

  return false;
}

bool EspnowEncryptionBroker::synchronizePeerSessionKey(const uint64_t sessionKey, const uint8_t *peerMac)
{
  if(EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(peerMac))
  {
    return synchronizePeerSessionKey(sessionKey, *encryptedConnection);
  }

  return false;
}

bool EspnowEncryptionBroker::synchronizePeerSessionKey(const uint64_t sessionKey, EncryptedConnectionLog &encryptedConnection)
{
  if(EspnowProtocolInterpreter::usesEncryption(sessionKey))
  {
    if(sessionKey == encryptedConnection.getPeerSessionKey())
    {
      uint8_t hashKey[hashKeyLength] {0};
      encryptedConnection.setPeerSessionKey(EncryptedConnectionLog::incrementSessionKey(sessionKey, encryptedConnection.getHashKey(hashKey), hashKeyLength));
      return true;
    }
  }

  return false;
}

uint8_t EspnowEncryptionBroker::reservedEncryptedConnections()
{
  if(!_ongoingPeerRequestNonce.isEmpty())
    if(!EspnowConnectionManager::getEncryptedConnection(_ongoingPeerRequestMac))
      return EspnowConnectionManager::encryptedConnections().size() + 1; // Reserve one connection spot if we are currently making a peer request to a new node.

  return EspnowConnectionManager::encryptedConnections().size();
}

EncryptedConnectionStatus EspnowEncryptionBroker::requestEncryptedConnectionKernel(const uint8_t *peerMac, const encryptionRequestBuilderType &encryptionRequestBuilder, EspnowMeshBackend &espnowInstance)
{
  using namespace EspnowProtocolInterpreter;
  
  assert(EspnowConnectionManager::encryptedConnections().size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library
  
  MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex(EspnowConnectionManager::handlePostponedRemovals));
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call requestEncryptedConnection from callbacks as this may corrupt program state! Aborting."))); 
    return EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
  }

  EncryptedConnectionLog *existingEncryptedConnection = EspnowConnectionManager::getEncryptedConnection(peerMac);
  ExpiringTimeTracker existingTimeTracker = existingEncryptedConnection && existingEncryptedConnection->temporary() ? 
                                            *existingEncryptedConnection->temporary() : ExpiringTimeTracker(0);
  
  if(!existingEncryptedConnection && EspnowConnectionManager::encryptedConnections().size() >= maxEncryptedConnections)
  {
    assert(EspnowConnectionManager::encryptedConnections().size() == maxEncryptedConnections);
    
    // No capacity for more encrypted connections.
    return EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_SELF;
  }

  String requestNonce = TypeCast::macToString(peerMac) + TypeCast::uint64ToString(MeshUtilityFunctions::randomUint64()) 
                                                       + TypeCast::uint64ToString(MeshUtilityFunctions::randomUint64());
  _ongoingPeerRequestResult = EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
  _ongoingPeerRequestNonce = requestNonce;
  _ongoingPeerRequester = &espnowInstance;
  _reciprocalPeerRequestConfirmation = false;
  std::copy_n(peerMac, 6, _ongoingPeerRequestMac);
  String requestMessage = encryptionRequestBuilder(requestNonce, existingTimeTracker);

  _conditionalPrinter.verboseModePrint(String(F("Sending encrypted connection request to: ")) + TypeCast::macToString(peerMac));

  if(EspnowTransmitter::espnowSendToNode(requestMessage, peerMac, 'P') == TransmissionStatusType::TRANSMISSION_COMPLETE)
  {
    ExpiringTimeTracker requestTimeout([](){ return EspnowDatabase::getEncryptionRequestTimeout(); });
    // _ongoingPeerRequestNonce is set to "" when a peer confirmation response from the mac is received
    while(!requestTimeout && !_ongoingPeerRequestNonce.isEmpty())
    {
      // For obvious reasons dividing by exactly 10 is a good choice.
      ExpiringTimeTracker maxDurationTracker = ExpiringTimeTracker(EspnowDatabase::getEncryptionRequestTimeout()/10);
      sendPeerRequestConfirmations(&maxDurationTracker); // Must be called before delay() to ensure !_ongoingPeerRequestNonce.isEmpty() is still true, so reciprocal peer request order is preserved.
      delay(1);
    }
  }

  if(!_ongoingPeerRequestNonce.isEmpty())
  {
    // If nonce != "" we only received the basic connection info, so the pairing process is incomplete
    _ongoingPeerRequestResult = EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
    _ongoingPeerRequestNonce.clear();
  }
  else if(encryptedConnectionEstablished(_ongoingPeerRequestResult))
  {
    if(_ongoingPeerRequestResult == EncryptedConnectionStatus::CONNECTION_ESTABLISHED)
      // Give the builder a chance to update the message
      requestMessage = encryptionRequestBuilder(requestNonce, existingTimeTracker);
    else if(_ongoingPeerRequestResult == EncryptedConnectionStatus::SOFT_LIMIT_CONNECTION_ESTABLISHED)
      // We will only get a soft limit connection. Adjust future actions based on this.
      requestMessage = Serializer::createEncryptionRequestHmacMessage(FPSTR(temporaryEncryptionRequestHeader), requestNonce, _connectionManager.getEspnowHashKey(), 
                                                                          hashKeyLength, _database.getAutoEncryptionDuration());
    else
      assert(false && String(F("Unknown _ongoingPeerRequestResult during encrypted connection finalization!")));
    
    int32_t messageHeaderEndIndex = requestMessage.indexOf(':');
    String messageHeader = requestMessage.substring(0, messageHeaderEndIndex + 1);
    String messageBody = requestMessage.substring(messageHeaderEndIndex + 1);
    
    // If we do not get an ack within getEncryptionRequestTimeout() the peer has probably had the time to delete the temporary encrypted connection.
    if(EspnowTransmitter::espnowSendToNode(String(FPSTR(encryptedConnectionVerificationHeader)) + requestMessage, peerMac, 'P') == TransmissionStatusType::TRANSMISSION_COMPLETE 
       && !_ongoingPeerRequestEncryptionTimeout)
    {
      EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(peerMac);
      if(!encryptedConnection)
      {
        assert(encryptedConnection && String(F("requestEncryptedConnectionKernel cannot find an encrypted connection!")));
        // requestEncryptedConnectionRemoval received.
        _ongoingPeerRequestResult = EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
      }
      else if(encryptedConnection->removalScheduled() || (encryptedConnection->temporary() && encryptedConnection->temporary()->expired()))
      {
        // Could possibly be caused by a simultaneous temporary peer request from the peer.
        _ongoingPeerRequestResult = EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
      }
      else
      {
        // Finalize connection
        if(messageHeader == FPSTR(encryptionRequestHeader))
        {
          EspnowConnectionManager::temporaryEncryptedConnectionToPermanent(peerMac);
        }
        else if(messageHeader == FPSTR(temporaryEncryptionRequestHeader))
        {
          if(encryptedConnection->temporary())
          {
            // Should not change duration of existing permanent connections.
            uint32_t connectionDuration = 0;
            bool durationFound = JsonTranslator::getDuration(messageBody, connectionDuration);
            assert(durationFound);
            encryptedConnection->setRemainingDuration(connectionDuration);
          }
        }
        else
        {
          assert(false && String(F("Unknown messageHeader during encrypted connection finalization!")));
          _ongoingPeerRequestResult = EncryptedConnectionStatus::API_CALL_FAILED;
        }
      }
    }
    else
    {      
      _ongoingPeerRequestResult = EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
    }
  }

  if(!encryptedConnectionEstablished(_ongoingPeerRequestResult))
  {
    if(!existingEncryptedConnection && !_reciprocalPeerRequestConfirmation)
    {
      // Remove any connection that was added during the request attempt and is no longer in use.
      EspnowConnectionManager::removeEncryptedConnectionUnprotected(peerMac);
    }
  }
  
  _ongoingPeerRequester = nullptr;

  return _ongoingPeerRequestResult;
}

EncryptedConnectionStatus EspnowEncryptionBroker::initiateAutoEncryptingConnection(const EspnowNetworkInfo &recipientInfo, const bool requestPermanentConnection, uint8_t *targetBSSID, EncryptedConnectionLog **existingEncryptedConnection, EspnowMeshBackend &espnowInstance)
{
  assert(recipientInfo.BSSID() != nullptr); // We need at least the BSSID to connect
  recipientInfo.getBSSID(targetBSSID);

  if(_conditionalPrinter.verboseMode()) // Avoid string generation if not required
  {
    espnowInstance.printAPInfo(recipientInfo);
    _conditionalPrinter.verboseModePrint(emptyString);
  }

  *existingEncryptedConnection = EspnowConnectionManager::getEncryptedConnection(targetBSSID);
  EncryptedConnectionStatus connectionStatus = EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_SELF;

  if(requestPermanentConnection)
    connectionStatus = requestEncryptedConnection(targetBSSID, espnowInstance);
  else
    connectionStatus = requestFlexibleTemporaryEncryptedConnection(targetBSSID, _database.getAutoEncryptionDuration(), espnowInstance);

  return connectionStatus;
}

void EspnowEncryptionBroker::finalizeAutoEncryptingConnection(const uint8_t *targetBSSID, const EncryptedConnectionLog *existingEncryptedConnection, const bool requestPermanentConnection)
{
  if(!existingEncryptedConnection && !requestPermanentConnection && !_reciprocalPeerRequestConfirmation)
  {
    // Remove any connection that was added during the transmission attempt and is no longer in use.
    EspnowConnectionManager::removeEncryptedConnectionUnprotected(targetBSSID);
  }
}

String EspnowEncryptionBroker::defaultEncryptionRequestBuilder(const String &requestHeader, const uint32_t durationMs, const uint8_t *hashKey,
                                                         const String &requestNonce, const ExpiringTimeTracker &existingTimeTracker)
{
  (void)existingTimeTracker; // This removes a "unused parameter" compiler warning. Does nothing else.
  
  return Serializer::createEncryptionRequestHmacMessage(requestHeader, requestNonce, hashKey, hashKeyLength, durationMs);
}
    
String EspnowEncryptionBroker::flexibleEncryptionRequestBuilder(const uint32_t minDurationMs, const uint8_t *hashKey, 
                                                           const String &requestNonce, const ExpiringTimeTracker &existingTimeTracker)
{
  using namespace JsonTranslator;
  using EspnowProtocolInterpreter::temporaryEncryptionRequestHeader;

  uint32_t connectionDuration = minDurationMs >= existingTimeTracker.remainingDuration() ? 
                                minDurationMs : existingTimeTracker.remainingDuration();

  return Serializer::createEncryptionRequestHmacMessage(FPSTR(temporaryEncryptionRequestHeader), requestNonce, hashKey, hashKeyLength, connectionDuration);
}
