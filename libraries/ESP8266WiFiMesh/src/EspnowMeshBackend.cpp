/*
  EspnowMeshBackend
 
  Copyright (C) 2019 Anders Löfgren
 
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

#include "EspnowMeshBackend.h"
#include "TypeConversionFunctions.h"
#include "UtilityFunctions.h"
#include "MutexTracker.h"
#include "JsonTranslator.h"
#include "MeshCryptoInterface.h"
#include "Serializer.h"

namespace
{
  using EspnowProtocolInterpreter::encryptedConnectionKeyLength;
  using EspnowProtocolInterpreter::hashKeyLength;

  namespace TypeCast = MeshTypeConversionFunctions;

  EspnowMeshBackend *_espnowRequestManager = nullptr;
}

void espnowDelay(uint32_t durationMs)
{
  ExpiringTimeTracker timeout(durationMs);

  do
  {
    // We want to delay before performEspnowMaintenance() so background tasks can be managed first.
    // Initial while combined with YieldAndDelayMs polledTimeout::YieldPolicy is not suitable since the delay then occurs before evaluating the condition (meaning durationMs = 1 never executes the loop interior).
    delay(1);
    EspnowMeshBackend::performEspnowMaintenance();
  }
  while(!timeout);
}

EspnowMeshBackend::EspnowMeshBackend(const requestHandlerType requestHandler, const responseHandlerType responseHandler, const networkFilterType networkFilter, 
                                     const broadcastFilterType broadcastFilter, const String &meshPassword, const String &ssidPrefix, const String &ssidSuffix, const bool verboseMode,
                                     const uint8 meshWiFiChannel) 
                                     : MeshBackendBase(requestHandler, responseHandler, networkFilter, MeshBackendType::ESP_NOW), 
                                       _database(_conditionalPrinter, meshWiFiChannel), _connectionManager(_conditionalPrinter, _database),
                                       _transmitter(_conditionalPrinter, _database, _connectionManager), 
                                       _encryptionBroker(_conditionalPrinter, _database, _connectionManager, _transmitter)
{
  setBroadcastFilter(broadcastFilter);
  setSSID(ssidPrefix, emptyString, ssidSuffix);
  setMeshPassword(meshPassword);
  setVerboseModeState(verboseMode);
  EspnowMeshBackend::setWiFiChannel(meshWiFiChannel);
}

EspnowMeshBackend::EspnowMeshBackend(const requestHandlerType requestHandler, const responseHandlerType responseHandler, const networkFilterType networkFilter, 
                                     const broadcastFilterType broadcastFilter, const String &meshPassword, const uint8_t espnowEncryptedConnectionKey[encryptedConnectionKeyLength],
                                     const uint8_t espnowHashKey[hashKeyLength], const String &ssidPrefix, const String &ssidSuffix, const bool verboseMode,
                                     const uint8 meshWiFiChannel) 
                                     : EspnowMeshBackend(requestHandler, responseHandler, networkFilter, broadcastFilter, meshPassword, ssidPrefix, ssidSuffix, verboseMode, meshWiFiChannel)
{
  setEspnowEncryptedConnectionKey(espnowEncryptedConnectionKey);
  setEspnowHashKey(espnowHashKey);
}

EspnowMeshBackend::EspnowMeshBackend(const requestHandlerType requestHandler, const responseHandlerType responseHandler, const networkFilterType networkFilter, 
                                     const broadcastFilterType broadcastFilter, const String &meshPassword, const String &espnowEncryptedConnectionKeySeed,
                                     const String &espnowHashKeySeed, const String &ssidPrefix, const String &ssidSuffix, const bool verboseMode,
                                     const uint8 meshWiFiChannel) 
                                     : EspnowMeshBackend(requestHandler, responseHandler, networkFilter, broadcastFilter, meshPassword, ssidPrefix, ssidSuffix, verboseMode, meshWiFiChannel)
{
  setEspnowEncryptedConnectionKey(espnowEncryptedConnectionKeySeed);
  setEspnowHashKey(espnowHashKeySeed);
}

EspnowMeshBackend::~EspnowMeshBackend()
{
  if(isEspnowRequestManager())
  {
    setEspnowRequestManager(nullptr);
  }

  _database.deleteSentRequestsByOwner(this);
}

std::vector<EspnowNetworkInfo> & EspnowMeshBackend::connectionQueue()
{  
  return EspnowDatabase::connectionQueue();
}

const std::vector<EspnowNetworkInfo> & EspnowMeshBackend::constConnectionQueue()
{  
  return EspnowDatabase::constConnectionQueue();
}

std::vector<TransmissionOutcome> & EspnowMeshBackend::latestTransmissionOutcomes()
{
  return EspnowDatabase::latestTransmissionOutcomes();
}

bool EspnowMeshBackend::latestTransmissionSuccessful()
{
  return latestTransmissionSuccessfulBase(latestTransmissionOutcomes());
}

void EspnowMeshBackend::begin()
{
  if(!getAPController()) // If there is no active AP controller
    WiFi.mode(WIFI_STA); // WIFI_AP_STA mode automatically sets up an AP, so we can't use that as default.

  activateEspnow();
}

bool EspnowMeshBackend::activateEspnow()
{
  if (esp_now_init()==0) 
  {    
    if(!EspnowConnectionManager::initializeEncryptionKok())
      warningPrint(String(F("Failed to set ESP-NOW KoK!")));
      
    if(getEspnowRequestManager() == nullptr)
    {
      setEspnowRequestManager(this);
    }
    
    esp_now_register_recv_cb(espnowReceiveCallbackWrapper);
    esp_now_register_send_cb(EspnowTransmitter::espnowSendCallback);

    // Role must be set before adding peers. Cannot be changed while having peers.
    // With ESP_NOW_ROLE_CONTROLLER, we always transmit from the station interface, which gives predictability.
    if(esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER)) // esp_now_set_self_role returns 0 on success.
      warningPrint(String(F("Failed to set ESP-NOW role! Maybe ESP-NOW peers are already added?")));

    verboseModePrint(String(F("ESP-NOW activated.")));
    verboseModePrint(String(F("My ESP-NOW STA MAC: ")) + WiFi.macAddress() + '\n'); // Get the station MAC address. The softAP MAC is different.
    
    return true;
  } 
  else 
  {
    warningPrint(String(F("ESP-NOW init failed!")));
    return false;
  }
}

bool EspnowMeshBackend::deactivateEspnow()
{
  // esp_now_deinit() clears all ESP-NOW API settings, including receive callback, send callback, Kok and peers.
  // The node will however continue to give acks to received ESP-NOW transmissions as long as the receiving interface (AP or STA) is active, even though the transmissions will not be processed.
  if(esp_now_deinit() == 0)
  {
    EspnowDatabase::responsesToSend().clear();
    EspnowDatabase::peerRequestConfirmationsToSend().clear();
    EspnowDatabase::receivedEspnowTransmissions().clear();
    EspnowDatabase::sentRequests().clear();
    EspnowDatabase::receivedRequests().clear();
    EspnowConnectionManager::encryptedConnections().clear();
    EncryptedConnectionLog::setNewRemovalsScheduled(false);
    
    return true;
  }
  else
  {
    return false;
  }
}

void EspnowMeshBackend::performEspnowMaintenance(const uint32_t estimatedMaxDuration)
{
  ExpiringTimeTracker estimatedMaxDurationTracker = ExpiringTimeTracker(estimatedMaxDuration);
  
  // Doing this during an ESP-NOW transmission could invalidate iterators
  MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex(EspnowConnectionManager::handlePostponedRemovals));
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call performEspnowMaintenance from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }

  EspnowDatabase::clearOldLogEntries(false);

  if(EncryptedConnectionLog::getSoonestExpiringConnectionTracker() && EncryptedConnectionLog::getSoonestExpiringConnectionTracker()->expired())
  {
    EspnowConnectionManager::updateTemporaryEncryptedConnections();
  }

  if(estimatedMaxDuration > 0)
  {
    if(estimatedMaxDurationTracker.expired())
      return;
    else
      sendStoredEspnowMessages(&estimatedMaxDurationTracker);
  }
  else
  {
    sendStoredEspnowMessages();
  }
}

void EspnowMeshBackend::espnowReceiveCallbackWrapper(uint8_t *macaddr, uint8_t *dataArray, const uint8_t len)
{
  using namespace EspnowProtocolInterpreter;

  // Since this callback can be called during any delay(), we should always consider all mutexes captured.
  // This provides a consistent mutex environment, which facilitates development and debugging. 
  // Otherwise we get issues such as EspnowTransmitter::_espnowTransmissionMutex will usually be free, but occasionally taken (when callback occurs in a delay() during attemptTransmission).
  MutexTracker captureBanTracker(MutexTracker::captureBan());
  
  if(len >= metadataSize()) // If we do not receive at least the metadata bytes, the transmission is invalid.
  {    
    //uint32_t callbackStart = millis();

    // If there is a espnowRequestManager, get it
    EspnowMeshBackend *currentEspnowRequestManager = getEspnowRequestManager();
    
    char messageType = getMessageType(dataArray);
    uint64_t receivedMessageID = getMessageID(dataArray);

    if(currentEspnowRequestManager && !currentEspnowRequestManager->acceptsUnverifiedRequests() 
       && !usesConstantSessionKey(messageType) && !EspnowEncryptionBroker::verifyPeerSessionKey(receivedMessageID, macaddr, messageType))
    {
      return;
    }

    if(EspnowTransmitter::useEncryptedMessages())
    {
      // chacha20Poly1305Decrypt decrypts dataArray in place.
      // We are using the protocol bytes as a key salt.
      if(!CryptoInterface::chacha20Poly1305Decrypt(dataArray + metadataSize(), len - metadataSize(), getEspnowMessageEncryptionKey(), dataArray, 
                                                   protocolBytesSize, dataArray + protocolBytesSize, dataArray + protocolBytesSize + 12))
      {
        return; // Decryption of message failed.
      }
    }
    
    uint64_t uint64StationMac = TypeCast::macToUint64(macaddr);
    bool transmissionEncrypted = usesEncryption(receivedMessageID);

    // Useful when debugging the protocol
    //Serial.print("Received from Mac: " + TypeCast::macToString(macaddr) + " ID: " + TypeCast::uint64ToString(receivedMessageID));
    //Serial.println(transmissionEncrypted ? " Encrypted" : " Unencrypted");

    if(messageType == 'Q' || messageType == 'B') // Question (request) or Broadcast
    {
      if(ESP.getFreeHeap() <= criticalHeapLevel())
      {
        warningPrint("WARNING! Free heap below critical level. Suspending ESP-NOW request processing until the situation improves.");
        return;
      }

      if(currentEspnowRequestManager)
      {        
        if(!EspnowDatabase::requestReceived(uint64StationMac, receivedMessageID)) // If the request has not already been received
        {
          if(transmissionEncrypted)
          {
            EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(macaddr);
            
            if(!encryptedConnection || (!EspnowEncryptionBroker::synchronizePeerSessionKey(receivedMessageID, *encryptedConnection) &&
                                        !EspnowEncryptionBroker::verifyPeerSessionKey(receivedMessageID, *encryptedConnection, uint64StationMac, messageType)))
            {
              // We received an encrypted transmission 
              // and we have no encrypted connection to the transmitting node (in which case we want to avoid sending the secret session key back in an unencrypted response)
              // or the transmission has the wrong session key 
              // and it doesn't have a session key that matches any multi-part transmission we are currently receiving (in which case the transmission is invalid).
              return;
            }
          }
          
          //Serial.println("espnowReceiveCallbackWrapper before internal callback " + String(millis() - callbackStart));
          
          currentEspnowRequestManager->espnowReceiveCallback(macaddr, dataArray, len);
        }
      }
    }
    else if(messageType == 'A') // Answer (response)
    { 
      EspnowMeshBackend *requestSender = nullptr;
      uint64_t requestMac = 0;
      
      if(transmissionEncrypted)
      {        
        // An encrypted transmission can only be sent to the station interface, since it otherwise won't arrive (because of ESP_NOW_ROLE_CONTROLLER).
        requestMac = uint64StationMac;
        requestSender = EspnowDatabase::getOwnerOfSentRequest(requestMac, receivedMessageID);
      }
      else
      {
        // An unencrypted transmission was probably sent to the AP interface as a result of a scan.
        requestMac = getTransmissionMac(dataArray);
        requestSender = EspnowDatabase::getOwnerOfSentRequest(requestMac, receivedMessageID);

        // But if not, also check if it was sent to the station interface.
        if(!requestSender)
        {
          requestMac = uint64StationMac;
          requestSender = EspnowDatabase::getOwnerOfSentRequest(requestMac, receivedMessageID);
        }

        // Or if it was sent as a broadcast. (A broadcast can never be encrypted)
        if(!requestSender)
        {
          requestSender = EspnowDatabase::getOwnerOfSentRequest(uint64BroadcastMac, receivedMessageID);
        }
      }

      // If this node sent the request and it has not already been answered.
      if(requestSender)
      {        
        uint8_t macArray[6] = { 0 };
        
        requestSender->espnowReceiveCallback(TypeCast::uint64ToMac(requestMac, macArray), dataArray, len);
      }
    }
    else if(messageType == 'S') // Synchronization request
    {
      EspnowEncryptionBroker::synchronizePeerSessionKey(receivedMessageID, macaddr);
    }
    else if(messageType == 'P') // Peer request
    {
      EspnowEncryptionBroker::handlePeerRequest(macaddr, dataArray, len, uint64StationMac, receivedMessageID);
    }
    else if(messageType == 'C') // peer request Confirmation
    {
      EspnowEncryptionBroker::handlePeerRequestConfirmation(macaddr, dataArray, len);
    }
    else
    {
      assert(messageType == 'Q' || messageType == 'A' || messageType == 'B' || messageType == 'S' || messageType == 'P' || messageType == 'C');
    }

    //Serial.println("espnowReceiveCallbackWrapper duration " + String(millis() - callbackStart));
  }
}

void EspnowMeshBackend::espnowReceiveCallback(const uint8_t *macaddr, uint8_t *dataArray, const uint8_t len)
{  
  using namespace EspnowProtocolInterpreter;
  
  ////// <Method overview> //////
  /*
  if(messageStart)
  {
    storeTransmission
  }
  else
  {
    if(messageFound)
      storeTransmission or (erase and return)
    else
      return
  }
  
  if(transmissionsRemaining != 0)
    return
    
  processMessage
  */
  ////// </Method overview> //////

  char messageType = getMessageType(dataArray);
  uint8_t transmissionsRemaining = getTransmissionsRemaining(dataArray);
  uint64_t uint64Mac = TypeCast::macToUint64(macaddr);
  
  // The MAC is 6 bytes so two bytes of uint64Mac are free. We must include the messageType there since it is possible that we will
  // receive both a request and a response that shares the same messageID from the same uint64Mac, being distinguished only by the messageType.
  // This would otherwise potentially cause the request and response to be mixed into one message when they are multi-part transmissions sent roughly at the same time.
  macAndType_td macAndType = createMacAndTypeValue(uint64Mac, messageType); 
  uint64_t messageID = getMessageID(dataArray);
  
  //uint32_t methodStart = millis();

  if(isMessageStart(dataArray))
  {
    if(messageType == 'B')
    {
      auto key = std::make_pair(macAndType, messageID);
      if(EspnowDatabase::receivedEspnowTransmissions().find(key) != EspnowDatabase::receivedEspnowTransmissions().end())
        return; // Should not call BroadcastFilter more than once for an accepted message
      
      String message = getHashKeyLength(dataArray, len);
      _database.setSenderMac(macaddr);
      uint8_t senderAPMac[6] {0};
      _database.setSenderAPMac(getTransmissionMac(dataArray, senderAPMac));
      _encryptionBroker.setReceivedEncryptedTransmission(usesEncryption(messageID));
      bool acceptBroadcast = getBroadcastFilter()(message, *this);
      if(acceptBroadcast)
      {
        // Does nothing if key already in receivedEspnowTransmissions
        EspnowDatabase::receivedEspnowTransmissions().insert(std::make_pair(key, MessageData(message, getTransmissionsRemaining(dataArray))));
      }
      else
      {
        return;
      }
    }
    else
    {  
      // Does nothing if key already in receivedEspnowTransmissions
      EspnowDatabase::receivedEspnowTransmissions().insert(std::make_pair(std::make_pair(macAndType, messageID), MessageData(dataArray, len)));
    }
  }
  else
  {
    std::map<std::pair<macAndType_td, messageID_td>, MessageData>::iterator storedMessageIterator = EspnowDatabase::receivedEspnowTransmissions().find(std::make_pair(macAndType, messageID));

    if(storedMessageIterator == EspnowDatabase::receivedEspnowTransmissions().end()) // If we have not stored the key already, we missed the first message part.
    {
      return;
    }
    
    if(!storedMessageIterator->second.addToMessage(dataArray, len))
    {
      // If we received the wrong message part, remove the whole message if we have missed a part.
      // Otherwise just ignore the received part since it has already been stored.
      
      uint8_t transmissionsRemainingExpected = storedMessageIterator->second.getTransmissionsRemaining() - 1;
      
      if(transmissionsRemaining < transmissionsRemainingExpected)
      {
        EspnowDatabase::receivedEspnowTransmissions().erase(storedMessageIterator);
        return;
      }
    }
  }
  
  //Serial.println("methodStart storage done " + String(millis() - methodStart));
  
  if(transmissionsRemaining != 0)
  {
    return;
  }

  std::map<std::pair<macAndType_td, messageID_td>, MessageData>::iterator storedMessageIterator = EspnowDatabase::receivedEspnowTransmissions().find(std::make_pair(macAndType, messageID));
  assert(storedMessageIterator != EspnowDatabase::receivedEspnowTransmissions().end());

  // Copy totalMessage in case user callbacks (request/responseHandler) do something odd with receivedEspnowTransmissions list.
  String totalMessage = storedMessageIterator->second.getTotalMessage(); // https://stackoverflow.com/questions/134731/returning-a-const-reference-to-an-object-instead-of-a-copy It is likely that most compilers will perform Named Value Return Value Optimisation in this case

  EspnowDatabase::receivedEspnowTransmissions().erase(storedMessageIterator); // Erase the extra copy of the totalMessage, to save RAM. 
   
  //Serial.println("methodStart erase done " + String(millis() - methodStart));
  
  if(messageType == 'Q' || messageType == 'B') // Question (request) or Broadcast
  {
    EspnowDatabase::storeReceivedRequest(uint64Mac, messageID, TimeTracker(millis()));
    //Serial.println("methodStart request stored " + String(millis() - methodStart));
      
    _database.setSenderMac(macaddr);
    uint8_t senderAPMac[6] {0};
    _database.setSenderAPMac(getTransmissionMac(dataArray, senderAPMac));
    _encryptionBroker.setReceivedEncryptedTransmission(usesEncryption(messageID));
    String response = getRequestHandler()(totalMessage, *this);
    //Serial.println("methodStart response acquired " + String(millis() - methodStart));
     
    if(response.length() > 0)
    {
      EspnowDatabase::responsesToSend().push_back(ResponseData(response, macaddr, messageID));
      
      //Serial.println("methodStart Q done " + String(millis() - methodStart));
    }
  }
  else if(messageType == 'A') // Answer (response)
  {
    EspnowDatabase::deleteSentRequest(uint64Mac, messageID); // Request has been answered, so stop accepting new answers about it.

    if(EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(macaddr))
    {
      if(encryptedConnection->getOwnSessionKey() == messageID)
      {
        encryptedConnection->setDesync(false); // We just received an answer to the latest request we sent to the node, so the node sending the answer must now be in sync.
        encryptedConnection->incrementOwnSessionKey();
      }
    } 
    
    _database.setSenderMac(macaddr);
    uint8_t senderAPMac[6] {0};
    _database.setSenderAPMac(getTransmissionMac(dataArray, senderAPMac));
    _encryptionBroker.setReceivedEncryptedTransmission(usesEncryption(messageID));
    getResponseHandler()(totalMessage, *this);
  }
  else
  {
    assert(messageType == 'Q' || messageType == 'A' || messageType == 'B');
  }
  
  ESP.wdtFeed(); // Prevents WDT reset in case we receive a lot of transmissions without break.

  //Serial.println("methodStart wdtFeed done " + String(millis() - methodStart));
}

void EspnowMeshBackend::setEspnowRequestManager(EspnowMeshBackend *espnowMeshInstance)
{
  _espnowRequestManager = espnowMeshInstance;
}

EspnowMeshBackend *EspnowMeshBackend::getEspnowRequestManager() {return _espnowRequestManager;}

bool EspnowMeshBackend::isEspnowRequestManager() const
{
  return (this == getEspnowRequestManager());
}

void EspnowMeshBackend::setLogEntryLifetimeMs(const uint32_t logEntryLifetimeMs)
{
  EspnowDatabase::setLogEntryLifetimeMs(logEntryLifetimeMs);
}
uint32_t EspnowMeshBackend::logEntryLifetimeMs() { return EspnowDatabase::logEntryLifetimeMs(); }

void EspnowMeshBackend::setBroadcastResponseTimeoutMs(const uint32_t broadcastResponseTimeoutMs)
{
  EspnowDatabase::setBroadcastResponseTimeoutMs(broadcastResponseTimeoutMs);
}
uint32_t EspnowMeshBackend::broadcastResponseTimeoutMs() { return EspnowDatabase::broadcastResponseTimeoutMs(); }

void EspnowMeshBackend::setCriticalHeapLevelBuffer(const uint32_t bufferInBytes)
{
  EspnowDatabase::setCriticalHeapLevelBuffer(bufferInBytes);
}

uint32_t EspnowMeshBackend::criticalHeapLevelBuffer()
{
  return EspnowDatabase::criticalHeapLevelBuffer();
}

uint32_t EspnowMeshBackend::criticalHeapLevel()
{
  return EspnowDatabase::criticalHeapLevel();
}

void EspnowMeshBackend::setEspnowTransmissionTimeout(const uint32_t timeoutMs)
{
  EspnowTransmitter::setEspnowTransmissionTimeout(timeoutMs);
}
uint32_t EspnowMeshBackend::getEspnowTransmissionTimeout() {return EspnowTransmitter::getEspnowTransmissionTimeout();}

void EspnowMeshBackend::setEspnowRetransmissionInterval(const uint32_t intervalMs)
{
  EspnowTransmitter::setEspnowRetransmissionInterval(intervalMs);
}
uint32_t EspnowMeshBackend::getEspnowRetransmissionInterval() {return EspnowTransmitter::getEspnowRetransmissionInterval();}

void EspnowMeshBackend::setEncryptionRequestTimeout(const uint32_t timeoutMs)
{
  EspnowDatabase::setEncryptionRequestTimeout(timeoutMs);
}
uint32_t EspnowMeshBackend::getEncryptionRequestTimeout() {return EspnowDatabase::getEncryptionRequestTimeout();}

void EspnowMeshBackend::setAutoEncryptionDuration(const uint32_t duration)
{
  _database.setAutoEncryptionDuration(duration);
}
uint32_t EspnowMeshBackend::getAutoEncryptionDuration() const {return _database.getAutoEncryptionDuration();}

void EspnowMeshBackend::setBroadcastFilter(const broadcastFilterType broadcastFilter) {_broadcastFilter = broadcastFilter;}
EspnowMeshBackend::broadcastFilterType EspnowMeshBackend::getBroadcastFilter() const {return _broadcastFilter;}

void EspnowMeshBackend::setEspnowEncryptedConnectionKey(const uint8_t espnowEncryptedConnectionKey[encryptedConnectionKeyLength])
{
  _connectionManager.setEspnowEncryptedConnectionKey(espnowEncryptedConnectionKey);
}

void EspnowMeshBackend::setEspnowEncryptedConnectionKey(const String &espnowEncryptedConnectionKeySeed)
{
  _connectionManager.setEspnowEncryptedConnectionKey(espnowEncryptedConnectionKeySeed);
}

const uint8_t *EspnowMeshBackend::getEspnowEncryptedConnectionKey() const
{
  return _connectionManager.getEspnowEncryptedConnectionKey();
}

uint8_t *EspnowMeshBackend::getEspnowEncryptedConnectionKey(uint8_t resultArray[encryptedConnectionKeyLength]) const
{
  return _connectionManager.getEspnowEncryptedConnectionKey(resultArray);
}

bool EspnowMeshBackend::setEspnowEncryptionKok(uint8_t espnowEncryptionKok[encryptedConnectionKeyLength])
{
  return EspnowConnectionManager::setEspnowEncryptionKok(espnowEncryptionKok);
}

bool EspnowMeshBackend::setEspnowEncryptionKok(const String &espnowEncryptionKokSeed)
{
  return EspnowConnectionManager::setEspnowEncryptionKok(espnowEncryptionKokSeed);
}

const uint8_t *EspnowMeshBackend::getEspnowEncryptionKok()
{
  return EspnowConnectionManager::getEspnowEncryptionKok();
}

void EspnowMeshBackend::setEspnowHashKey(const uint8_t espnowHashKey[hashKeyLength])
{
  _connectionManager.setEspnowHashKey(espnowHashKey);
}

void EspnowMeshBackend::setEspnowHashKey(const String &espnowHashKeySeed)
{
  _connectionManager.setEspnowHashKey(espnowHashKeySeed);
}

const uint8_t *EspnowMeshBackend::getEspnowHashKey() const
{
  return _connectionManager.getEspnowHashKey();
}

void EspnowMeshBackend::setUseEncryptedMessages(const bool useEncryptedMessages) 
{
  EspnowTransmitter::setUseEncryptedMessages(useEncryptedMessages);
}
bool EspnowMeshBackend::useEncryptedMessages() { return EspnowTransmitter::useEncryptedMessages(); }

void EspnowMeshBackend::setEspnowMessageEncryptionKey(const uint8_t espnowMessageEncryptionKey[CryptoInterface::ENCRYPTION_KEY_LENGTH])
{
  EspnowTransmitter::setEspnowMessageEncryptionKey(espnowMessageEncryptionKey);
}

void EspnowMeshBackend::setEspnowMessageEncryptionKey(const String &espnowMessageEncryptionKeySeed)
{
  EspnowTransmitter::setEspnowMessageEncryptionKey(espnowMessageEncryptionKeySeed);
}

const uint8_t *EspnowMeshBackend::getEspnowMessageEncryptionKey()
{
  return EspnowTransmitter::getEspnowMessageEncryptionKey();
}

String EspnowMeshBackend::getScheduledResponseMessage(const uint32_t responseIndex)
{
  return EspnowDatabase::getScheduledResponseMessage(responseIndex);
}

const uint8_t *EspnowMeshBackend::getScheduledResponseRecipient(const uint32_t responseIndex)
{
  return EspnowDatabase::getScheduledResponseRecipient(responseIndex);
}

uint32_t EspnowMeshBackend::numberOfScheduledResponses() {return EspnowDatabase::numberOfScheduledResponses();}

void EspnowMeshBackend::clearAllScheduledResponses()
{
  EspnowDatabase::clearAllScheduledResponses();
}

void EspnowMeshBackend::deleteScheduledResponsesByRecipient(const uint8_t *recipientMac, const bool encryptedOnly)
{
  EspnowDatabase::deleteScheduledResponsesByRecipient(recipientMac, encryptedOnly);
}

String EspnowMeshBackend::getSenderMac() const {return _database.getSenderMac();}
uint8_t *EspnowMeshBackend::getSenderMac(uint8_t *macArray) const
{
  return _database.getSenderMac(macArray);
}

String EspnowMeshBackend::getSenderAPMac() const {return _database.getSenderAPMac();}
uint8_t *EspnowMeshBackend::getSenderAPMac(uint8_t *macArray) const
{
  return _database.getSenderAPMac(macArray);
}

bool EspnowMeshBackend::receivedEncryptedTransmission() const {return _encryptionBroker.receivedEncryptedTransmission();}

bool EspnowMeshBackend::addUnencryptedConnection(const String &serializedConnectionState)
{
  return EspnowConnectionManager::addUnencryptedConnection(serializedConnectionState);
}

EncryptedConnectionStatus EspnowMeshBackend::addEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey)
{
  return _connectionManager.addEncryptedConnection(peerStaMac, peerApMac, peerSessionKey, ownSessionKey);
}

EncryptedConnectionStatus EspnowMeshBackend::addEncryptedConnection(const String &serializedConnectionState, const bool ignoreDuration)
{
  return _connectionManager.addEncryptedConnection(serializedConnectionState, ignoreDuration);
}

EncryptedConnectionStatus EspnowMeshBackend::addTemporaryEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint32_t duration)
{  
  return _connectionManager.addTemporaryEncryptedConnection(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, duration);
}

EncryptedConnectionStatus EspnowMeshBackend::addTemporaryEncryptedConnection(const String &serializedConnectionState, const uint32_t duration)
{
  return _connectionManager.addTemporaryEncryptedConnection(serializedConnectionState, duration);
}

EncryptedConnectionStatus EspnowMeshBackend::requestEncryptedConnection(const uint8_t *peerMac)
{
  return _encryptionBroker.requestEncryptedConnection(peerMac, *this);
}

EncryptedConnectionStatus EspnowMeshBackend::requestTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t durationMs)
{
  return _encryptionBroker.requestTemporaryEncryptedConnection(peerMac, durationMs, *this);
}

EncryptedConnectionStatus EspnowMeshBackend::requestFlexibleTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t minDurationMs)
{
  return _encryptionBroker.requestFlexibleTemporaryEncryptedConnection(peerMac, minDurationMs, *this);
}

EncryptedConnectionRemovalOutcome EspnowMeshBackend::removeEncryptedConnection(const uint8_t *peerMac)
{
  return EspnowConnectionManager::removeEncryptedConnection(peerMac);
}

EncryptedConnectionRemovalOutcome EspnowMeshBackend::requestEncryptedConnectionRemoval(const uint8_t *peerMac)
{
  return _encryptionBroker.requestEncryptedConnectionRemoval(peerMac);
}

void EspnowMeshBackend::setAcceptsUnverifiedRequests(const bool acceptsUnverifiedRequests)  { _acceptsUnverifiedRequests = acceptsUnverifiedRequests; }
bool EspnowMeshBackend::acceptsUnverifiedRequests() const { return _acceptsUnverifiedRequests; }

void EspnowMeshBackend::setEncryptedConnectionsSoftLimit(const uint8_t softLimit) 
{ 
  _connectionManager.setEncryptedConnectionsSoftLimit(softLimit);
}

uint8_t EspnowMeshBackend::encryptedConnectionsSoftLimit() const { return _connectionManager.encryptedConnectionsSoftLimit(); }

uint8_t *EspnowMeshBackend::getEncryptedMac(const uint8_t *peerMac, uint8_t *resultArray)
{
  return EspnowConnectionManager::getEncryptedMac(peerMac, resultArray);
}

void EspnowMeshBackend::prepareForTransmission(const String &message, const bool scan, const bool scanAllWiFiChannels)
{
  setMessage(message);
  
  latestTransmissionOutcomes().clear();

  if(scan)
  {
    connectionQueue().clear();
    scanForNetworks(scanAllWiFiChannels);
  }
}

TransmissionStatusType EspnowMeshBackend::initiateTransmission(const String &message, const EspnowNetworkInfo &recipientInfo)
{
  uint8_t targetBSSID[6] {0};

  assert(recipientInfo.BSSID() != nullptr); // We need at least the BSSID to connect
  recipientInfo.getBSSID(targetBSSID);

  if(_conditionalPrinter.verboseMode()) // Avoid string generation if not required
  {
    printAPInfo(recipientInfo);
    _conditionalPrinter.verboseModePrint(emptyString);
  }

  return initiateTransmissionKernel(message, targetBSSID);
}

TransmissionStatusType EspnowMeshBackend::initiateTransmissionKernel(const String &message, const uint8_t *targetBSSID)
{
  uint32_t transmissionStartTime = millis();
  TransmissionStatusType transmissionResult = _transmitter.sendRequest(message, targetBSSID, this);

  uint32_t transmissionDuration = millis() - transmissionStartTime;
  
  if(_conditionalPrinter.verboseMode() && transmissionResult == TransmissionStatusType::TRANSMISSION_COMPLETE) // Avoid calculations if not required
  {
    totalDurationWhenSuccessful_AT += transmissionDuration;
    ++successfulTransmissions_AT;
    if(transmissionDuration > maxTransmissionDuration_AT)
    {
      maxTransmissionDuration_AT = transmissionDuration;
    }
  }

  return transmissionResult;
}

void EspnowMeshBackend::printTransmissionStatistics() const
{
  if(_conditionalPrinter.verboseMode() && successfulTransmissions_AT > 0) // Avoid calculations if not required
  {
    _conditionalPrinter.verboseModePrint(String(F("Average duration of successful transmissions: ")) + String(totalDurationWhenSuccessful_AT/successfulTransmissions_AT) + String(F(" ms.")));
    _conditionalPrinter.verboseModePrint(String(F("Maximum duration of successful transmissions: ")) + String(maxTransmissionDuration_AT) + String(F(" ms.")));
  }
  else
  {
    _conditionalPrinter.verboseModePrint(String(F("No successful transmission.")));
  }
}

void EspnowMeshBackend::attemptTransmission(const String &message, const bool scan, const bool scanAllWiFiChannels)
{
  MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex(EspnowConnectionManager::handlePostponedRemovals));
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }

  prepareForTransmission(message, scan, scanAllWiFiChannels);

  MutexTracker connectionQueueMutexTracker(EspnowDatabase::captureEspnowConnectionQueueMutex());
  if(!connectionQueueMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! connectionQueue locked. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
  }
  else
  { 
    for(const EspnowNetworkInfo &currentNetwork : constConnectionQueue())
    {
      TransmissionStatusType transmissionResult = initiateTransmission(getMessage(), currentNetwork);
  
      latestTransmissionOutcomes().push_back(TransmissionOutcome{.origin = currentNetwork, .transmissionStatus = transmissionResult});
  
      if(!getTransmissionOutcomesUpdateHook()(*this))
        break;
    }
  }

  printTransmissionStatistics();
}

TransmissionStatusType EspnowMeshBackend::attemptTransmission(const String &message, const EspnowNetworkInfo &recipientInfo)
{
  MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex(EspnowConnectionManager::handlePostponedRemovals));
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return TransmissionStatusType::CONNECTION_FAILED;
  }

  return initiateTransmission(message, recipientInfo);
}

TransmissionStatusType EspnowMeshBackend::initiateAutoEncryptingTransmission(const String &message, uint8_t *targetBSSID, EncryptedConnectionStatus connectionStatus)
{
  TransmissionStatusType transmissionResult = TransmissionStatusType::CONNECTION_FAILED;
  
  if(EspnowEncryptionBroker::encryptedConnectionEstablished(connectionStatus))
  {
    uint8_t encryptedMac[6] {0};
    assert(getEncryptedMac(targetBSSID, encryptedMac) && esp_now_is_peer_exist(encryptedMac) > 0 && String(F("ERROR! Attempting to send content marked as encrypted via unencrypted connection!")));
    transmissionResult = initiateTransmissionKernel(message, targetBSSID);
  }
  
  return transmissionResult;
}

void EspnowMeshBackend::attemptAutoEncryptingTransmission(const String &message, const bool requestPermanentConnections, const bool scan, const bool scanAllWiFiChannels)
{
  MutexTracker outerMutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex(EspnowConnectionManager::handlePostponedRemovals));
  if(!outerMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call attemptAutoEncryptingTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }

  prepareForTransmission(message, scan, scanAllWiFiChannels);

  outerMutexTracker.releaseMutex();

  MutexTracker connectionQueueMutexTracker(EspnowDatabase::captureEspnowConnectionQueueMutex());
  if(!connectionQueueMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! connectionQueue locked. Don't call attemptAutoEncryptingTransmission from callbacks as this may corrupt program state! Aborting."))); 
  }
  else
  {
    for(const EspnowNetworkInfo &currentNetwork : constConnectionQueue())
    {    
      uint8_t currentBSSID[6] {0};
      EncryptedConnectionLog *existingEncryptedConnection = nullptr;
      EncryptedConnectionStatus connectionStatus = _encryptionBroker.initiateAutoEncryptingConnection(currentNetwork, requestPermanentConnections, currentBSSID, &existingEncryptedConnection, *this);
  
      MutexTracker innerMutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex());
      if(!innerMutexTracker.mutexCaptured())
      {
        assert(false && String(F("ERROR! Unable to recapture Mutex in attemptAutoEncryptingTransmission. Aborting."))); 
        return;
      }
  
      TransmissionStatusType transmissionResult = initiateAutoEncryptingTransmission(getMessage(), currentBSSID, connectionStatus);
  
      latestTransmissionOutcomes().push_back(TransmissionOutcome{.origin = currentNetwork, .transmissionStatus = transmissionResult});
  
      _encryptionBroker.finalizeAutoEncryptingConnection(currentBSSID, existingEncryptedConnection, requestPermanentConnections);
  
      if(!getTransmissionOutcomesUpdateHook()(*this))
        break;
    }
  }

  printTransmissionStatistics();
}

TransmissionStatusType EspnowMeshBackend::attemptAutoEncryptingTransmission(const String &message, const EspnowNetworkInfo &recipientInfo, const bool requestPermanentConnection)
{
  uint8_t targetBSSID[6] {0};
  EncryptedConnectionLog *existingEncryptedConnection = nullptr;
  EncryptedConnectionStatus connectionStatus = _encryptionBroker.initiateAutoEncryptingConnection(recipientInfo, requestPermanentConnection, targetBSSID, &existingEncryptedConnection, *this);

  MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex(EspnowConnectionManager::handlePostponedRemovals));
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return TransmissionStatusType::CONNECTION_FAILED;
  }

  TransmissionStatusType transmissionResult = initiateAutoEncryptingTransmission(message, targetBSSID, connectionStatus);

  _encryptionBroker.finalizeAutoEncryptingConnection(targetBSSID, existingEncryptedConnection, requestPermanentConnection);

  return transmissionResult;
}

void EspnowMeshBackend::broadcast(const String &message)
{  
  MutexTracker mutexTracker(EspnowTransmitter::captureEspnowTransmissionMutex(EspnowConnectionManager::handlePostponedRemovals));
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call broadcast from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }

  EspnowTransmitter::espnowSendToNode(message, EspnowProtocolInterpreter::broadcastMac, 'B', this);
}

void EspnowMeshBackend::setBroadcastTransmissionRedundancy(const uint8_t redundancy) { _transmitter.setBroadcastTransmissionRedundancy(redundancy); }
uint8_t EspnowMeshBackend::getBroadcastTransmissionRedundancy() const { return _transmitter.getBroadcastTransmissionRedundancy(); }

void EspnowMeshBackend::setResponseTransmittedHook(const EspnowTransmitter::responseTransmittedHookType responseTransmittedHook) { _transmitter.setResponseTransmittedHook(responseTransmittedHook); }
EspnowTransmitter::responseTransmittedHookType EspnowMeshBackend::getResponseTransmittedHook() const { return _transmitter.getResponseTransmittedHook(); }

void EspnowMeshBackend::sendStoredEspnowMessages(const ExpiringTimeTracker *estimatedMaxDurationTracker)
{
  EspnowEncryptionBroker::sendPeerRequestConfirmations(estimatedMaxDurationTracker);

  if(estimatedMaxDurationTracker && estimatedMaxDurationTracker->expired())
    return;

  EspnowTransmitter::sendEspnowResponses(estimatedMaxDurationTracker);
}

uint32_t EspnowMeshBackend::getMaxMessageBytesPerTransmission()
{
  return EspnowProtocolInterpreter::getMaxMessageBytesPerTransmission();
}

void EspnowMeshBackend::setMaxTransmissionsPerMessage(const uint8_t maxTransmissionsPerMessage)
{
  EspnowTransmitter::setMaxTransmissionsPerMessage(maxTransmissionsPerMessage);
}

uint8_t EspnowMeshBackend::getMaxTransmissionsPerMessage() {return EspnowTransmitter::getMaxTransmissionsPerMessage();}

uint32_t EspnowMeshBackend::getMaxMessageLength()
{
  return EspnowTransmitter::getMaxMessageLength();
}

void EspnowMeshBackend::setVerboseModeState(const bool enabled) {_conditionalPrinter.setVerboseModeState(enabled); ConditionalPrinter::setStaticVerboseModeState(enabled);}
bool EspnowMeshBackend::verboseMode() const {return ConditionalPrinter::staticVerboseMode();}

void EspnowMeshBackend::verboseModePrint(const String &stringToPrint, const bool newline) const
{
  _conditionalPrinter.verboseModePrint(stringToPrint, newline);
}

bool EspnowMeshBackend::staticVerboseMode() {return ConditionalPrinter::staticVerboseMode();}
void EspnowMeshBackend::staticVerboseModePrint(const String &stringToPrint, const bool newline)
{
  ConditionalPrinter::staticVerboseModePrint(stringToPrint, newline);
}

uint8_t EspnowMeshBackend::numberOfEncryptedConnections()
{
  return EspnowConnectionManager::numberOfEncryptedConnections();
}

ConnectionType EspnowMeshBackend::getConnectionInfo(uint8_t *peerMac, uint32_t *remainingDuration)
{
  return EspnowConnectionManager::getConnectionInfo(peerMac, remainingDuration);
}

ConnectionType EspnowMeshBackend::getConnectionInfo(const uint32_t connectionIndex, uint32_t *remainingDuration, uint8_t *peerMac)
{
  return EspnowConnectionManager::getConnectionInfo(connectionIndex, remainingDuration, peerMac);
}

double EspnowMeshBackend::getTransmissionFailRate()
{
  return EspnowTransmitter::getTransmissionFailRate();
}

void EspnowMeshBackend::resetTransmissionFailRate()
{
  EspnowTransmitter::resetTransmissionFailRate();
}

String EspnowMeshBackend::serializeUnencryptedConnection()
{
  return EspnowConnectionManager::serializeUnencryptedConnection();
}

String EspnowMeshBackend::serializeEncryptedConnection(const uint8_t *peerMac)
{
  return EspnowConnectionManager::serializeEncryptedConnection(peerMac);
}

String EspnowMeshBackend::serializeEncryptedConnection(const uint32_t connectionIndex)
{  
  return EspnowConnectionManager::serializeEncryptedConnection(connectionIndex);
}

void EspnowMeshBackend::setWiFiChannel(const uint8 newWiFiChannel)
{
  MeshBackendBase::setWiFiChannel(newWiFiChannel);
  _database.setWiFiChannel(newWiFiChannel);
}
