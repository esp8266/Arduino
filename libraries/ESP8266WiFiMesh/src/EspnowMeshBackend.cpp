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

namespace
{
  using EspnowProtocolInterpreter::encryptedConnectionKeyLength;
  using EspnowProtocolInterpreter::hashKeyLength;

  namespace TypeCast = MeshTypeConversionFunctions;
  
  constexpr uint8_t maxEncryptedConnections = 6; // This is limited by the ESP-NOW API. Max 6 in AP or AP+STA mode. Max 10 in STA mode. See "ESP-NOW User Guide" for more info. 
  
  constexpr uint64_t uint64MSB = 0x8000000000000000;

  double _transmissionsTotal = 0;
  double _transmissionsFailed = 0;

  std::shared_ptr<bool> _espnowSendToNodeMutex = std::make_shared<bool>(false);
  uint8_t _transmissionTargetBSSID[6] = {0};
 
  uint8_t _espnowEncryptionKok[EspnowProtocolInterpreter::encryptedConnectionKeyLength] = { 0 };
  bool _espnowEncryptionKokSet = false;
  uint8_t _espnowMessageEncryptionKey[CryptoInterface::ENCRYPTION_KEY_LENGTH] = { 0 };
  bool _useEncryptedMessages = false;
  uint32_t _unsynchronizedMessageID = 0;
  
  String _ongoingPeerRequestNonce;
  uint8_t _ongoingPeerRequestMac[6] = {0};
  EspnowMeshBackend *_ongoingPeerRequester = nullptr;
  EncryptedConnectionStatus _ongoingPeerRequestResult = EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_SELF;
  ExpiringTimeTracker _ongoingPeerRequestEncryptionTimeout([](){ return EspnowMeshBackend::getEncryptionRequestTimeout(); });
  bool _reciprocalPeerRequestConfirmation = false;

  // _logEntryLifetimeMs is based on someone storing 40 responses of 750 bytes each = 30 000 bytes (roughly full memory), 
  // which takes 2000 ms + some margin to send. Also, we want to avoid old entries taking up memory if they cannot be sent, 
  // so storage duration should not be too long.
  uint32_t _logEntryLifetimeMs = 2500;
  uint32_t _broadcastResponseTimeoutMs = 1000; // This is shorter than _logEntryLifetimeMs to preserve RAM since broadcasts are not deleted from sentRequests until they expire.
  ExpiringTimeTracker _logClearingCooldown(500);

  uint32_t _encryptionRequestTimeoutMs = 300;

  uint32_t _criticalHeapLevel = 6000; // In bytes
  uint32_t _criticalHeapLevelBuffer = 6000; // In bytes

  bool _espnowSendConfirmed = false;

  std::list<ResponseData> responsesToSend = {};
  std::list<PeerRequestLog> peerRequestConfirmationsToSend = {};
  
  std::vector<EncryptedConnectionLog> encryptedConnections = {};

  EspnowMeshBackend *_espnowRequestManager = nullptr;
  
  constexpr uint32_t _maxBytesPerTransmission = 250;
  uint8_t _maxTransmissionsPerMessage = 3;

  uint32_t _espnowTransmissionTimeoutMs = 40;
  uint32_t _espnowRetransmissionIntervalMs = 15; 
}

const uint8_t EspnowMeshBackend::broadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint64_t EspnowMeshBackend::uint64BroadcastMac = 0xFFFFFFFFFFFF;

bool EspnowMeshBackend::_staticVerboseMode = false;

std::shared_ptr<bool> EspnowMeshBackend::_espnowTransmissionMutex = std::make_shared<bool>(false);
std::shared_ptr<bool> EspnowMeshBackend::_espnowConnectionQueueMutex = std::make_shared<bool>(false);
std::shared_ptr<bool> EspnowMeshBackend::_responsesToSendMutex = std::make_shared<bool>(false);

std::map<std::pair<EspnowMeshBackend::macAndType_td, EspnowMeshBackend::messageID_td>, MessageData> EspnowMeshBackend::receivedEspnowTransmissions = {};
std::map<std::pair<EspnowMeshBackend::peerMac_td, EspnowMeshBackend::messageID_td>, RequestData> EspnowMeshBackend::sentRequests = {};
std::map<std::pair<EspnowMeshBackend::peerMac_td, EspnowMeshBackend::messageID_td>, TimeTracker> EspnowMeshBackend::receivedRequests = {}; 
/*
std::list<ResponseData> EspnowMeshBackend::responsesToSend = {};
std::list<PeerRequestLog> EspnowMeshBackend::peerRequestConfirmationsToSend = {};*/

std::vector<EspnowNetworkInfo> EspnowMeshBackend::_connectionQueue = {};
std::vector<TransmissionOutcome> EspnowMeshBackend::_latestTransmissionOutcomes = {};

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
                                     : MeshBackendBase(requestHandler, responseHandler, networkFilter, MeshBackendType::ESP_NOW)
{
  // Reserve the maximum possible usage early on to prevent heap fragmentation later.
  encryptedConnections.reserve(maxEncryptedConnections);

  setBroadcastFilter(broadcastFilter);
  setSSID(ssidPrefix, emptyString, ssidSuffix);
  setMeshPassword(meshPassword);
  setVerboseModeState(verboseMode);
  setWiFiChannel(meshWiFiChannel);
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

  deleteSentRequestsByOwner(this);
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
    if(_espnowEncryptionKokSet && esp_now_set_kok(_espnowEncryptionKok, encryptedConnectionKeyLength)) // esp_now_set_kok returns 0 on success.
      warningPrint(String(F("Failed to set ESP-NOW KoK!")));
      
    if(getEspnowRequestManager() == nullptr)
    {
      setEspnowRequestManager(this);
    }
    
    esp_now_register_recv_cb(espnowReceiveCallbackWrapper);
    esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) {
      if(_espnowSendConfirmed)
        return;
      else if(!sendStatus && MeshUtilityFunctions::macEqual(mac, _transmissionTargetBSSID)) // sendStatus == 0 when send was OK.
        _espnowSendConfirmed = true; // We do not want to reset this to false. That only happens before transmissions. Otherwise subsequent failed send attempts may obscure an initial successful one.
    });

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
    responsesToSend.clear();
    peerRequestConfirmationsToSend.clear();
    receivedEspnowTransmissions.clear();
    sentRequests.clear();
    receivedRequests.clear();
    encryptedConnections.clear();
    EncryptedConnectionLog::setNewRemovalsScheduled(false);
    
    return true;
  }
  else
  {
    return false;
  }
}

std::vector<EspnowNetworkInfo> & EspnowMeshBackend::connectionQueue()
{
  MutexTracker connectionQueueMutexTracker(_espnowConnectionQueueMutex);
  if(!connectionQueueMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! connectionQueue locked. Don't call connectionQueue() from callbacks other than NetworkFilter as this may corrupt program state!"))); 
  }
  
  return _connectionQueue;
}

const std::vector<EspnowNetworkInfo> & EspnowMeshBackend::constConnectionQueue()
{  
  return _connectionQueue;
}

std::vector<TransmissionOutcome> & EspnowMeshBackend::latestTransmissionOutcomes()
{
  return _latestTransmissionOutcomes;
}

bool EspnowMeshBackend::latestTransmissionSuccessful()
{
  return latestTransmissionSuccessfulBase(latestTransmissionOutcomes());
}

void EspnowMeshBackend::performEspnowMaintenance(const uint32_t estimatedMaxDuration)
{
  ExpiringTimeTracker estimatedMaxDurationTracker = ExpiringTimeTracker(estimatedMaxDuration);
  
  // Doing this during an ESP-NOW transmission could invalidate iterators
  MutexTracker mutexTracker(_espnowTransmissionMutex, handlePostponedRemovals);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call performEspnowMaintenance from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }
  
  if(_logClearingCooldown) // Clearing too frequently will cause a lot of unnecessary container iterations.
  {
    clearOldLogEntries();
  }
  if(EncryptedConnectionLog::getSoonestExpiringConnectionTracker() && EncryptedConnectionLog::getSoonestExpiringConnectionTracker()->expired())
  {
    updateTemporaryEncryptedConnections();
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

void EspnowMeshBackend::updateTemporaryEncryptedConnections(const bool scheduledRemovalOnly)
{  
  EncryptedConnectionLog::clearSoonestExpiringConnectionTracker();
  
  for(auto connectionIterator = encryptedConnections.begin(); connectionIterator != encryptedConnections.end(); ) 
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

template <typename T, typename U>
void EspnowMeshBackend::deleteExpiredLogEntries(std::map<std::pair<U, uint64_t>, T> &logEntries, const uint32_t maxEntryLifetimeMs)
{
  for(typename std::map<std::pair<U, uint64_t>, T>::iterator entryIterator = logEntries.begin(); 
      entryIterator != logEntries.end(); )
  {
    if(entryIterator->second.getTimeTracker().timeSinceCreation() > maxEntryLifetimeMs)
    {
      entryIterator = logEntries.erase(entryIterator);
    }
    else
      ++entryIterator;
  }
}

template <typename U>
void EspnowMeshBackend::deleteExpiredLogEntries(std::map<std::pair<U, uint64_t>, TimeTracker> &logEntries, const uint32_t maxEntryLifetimeMs)
{
  for(typename std::map<std::pair<U, uint64_t>, TimeTracker>::iterator entryIterator = logEntries.begin(); 
      entryIterator != logEntries.end(); )
  {
    if(entryIterator->second.timeSinceCreation() > maxEntryLifetimeMs)
    {
      entryIterator = logEntries.erase(entryIterator);
    }
    else
      ++entryIterator;
  }
}

void EspnowMeshBackend::deleteExpiredLogEntries(std::map<std::pair<peerMac_td, messageID_td>, RequestData> &logEntries, const uint32_t requestLifetimeMs, const uint32_t broadcastLifetimeMs)
{
  for(typename std::map<std::pair<peerMac_td, messageID_td>, RequestData>::iterator entryIterator = logEntries.begin(); 
      entryIterator != logEntries.end(); )
  {
    bool broadcast = entryIterator->first.first == uint64BroadcastMac;
    uint32_t timeSinceCreation = entryIterator->second.getTimeTracker().timeSinceCreation();
    
    if((!broadcast && timeSinceCreation > requestLifetimeMs) 
        || (broadcast && timeSinceCreation > broadcastLifetimeMs))
    {
      entryIterator = logEntries.erase(entryIterator);
    }
    else
      ++entryIterator;
  }
}

template <typename T>
void EspnowMeshBackend::deleteExpiredLogEntries(std::list<T> &logEntries, const uint32_t maxEntryLifetimeMs)
{
  for(typename std::list<T>::iterator entryIterator = logEntries.begin(); 
      entryIterator != logEntries.end(); )
  {
    if(entryIterator->getTimeTracker().timeSinceCreation() > maxEntryLifetimeMs)
    {
      entryIterator = logEntries.erase(entryIterator);
    }
    else
      ++entryIterator;
  }
}

template <> 
void EspnowMeshBackend::deleteExpiredLogEntries(std::list<EncryptedConnectionLog> &logEntries, const uint32_t maxEntryLifetimeMs)
{
  for(typename std::list<EncryptedConnectionLog>::iterator entryIterator = logEntries.begin(); 
    entryIterator != logEntries.end(); )
  {
    auto timeTrackerPointer = entryIterator->temporary();
    if(timeTrackerPointer && timeTrackerPointer->elapsedTime() > maxEntryLifetimeMs)
    {
      entryIterator = logEntries.erase(entryIterator);
    }
    else
      ++entryIterator;
  }
}

template <> 
void EspnowMeshBackend::deleteExpiredLogEntries(std::list<PeerRequestLog> &logEntries, const uint32_t maxEntryLifetimeMs)
{
  for(typename std::list<PeerRequestLog>::iterator entryIterator = logEntries.begin(); 
    entryIterator != logEntries.end(); )
  {
    auto timeTrackerPointer = entryIterator->temporary();
    if(timeTrackerPointer && timeTrackerPointer->elapsedTime() > maxEntryLifetimeMs)
    {
      entryIterator = logEntries.erase(entryIterator);
    }
    else
      ++entryIterator;
  }
}

void EspnowMeshBackend::clearOldLogEntries()
{
  // Clearing all old log entries at the same time should help minimize heap fragmentation.
  
  // uint32_t startTime = millis();
  
  _logClearingCooldown.reset();
  
  deleteExpiredLogEntries(receivedEspnowTransmissions, logEntryLifetimeMs());
  deleteExpiredLogEntries(receivedRequests, logEntryLifetimeMs()); // Just needs to be long enough to not accept repeated transmissions by mistake.
  deleteExpiredLogEntries(sentRequests, logEntryLifetimeMs(), broadcastResponseTimeoutMs());
  deleteExpiredLogEntries(responsesToSend, logEntryLifetimeMs());
  deleteExpiredLogEntries(peerRequestConfirmationsToSend, getEncryptionRequestTimeout());
}

void EspnowMeshBackend::espnowReceiveCallbackWrapper(uint8_t *macaddr, uint8_t *dataArray, const uint8_t len)
{
  using namespace EspnowProtocolInterpreter;

  // Since this callback can be called during any delay(), we should always consider all mutexes captured.
  // This provides a consistent mutex environment, which facilitates development and debugging. 
  // Otherwise we get issues such as _espnowTransmissionMutex will usually be free, but occasionally taken (when callback occurs in a delay() during attemptTransmission).
  MutexTracker captureBanTracker(MutexTracker::captureBan());
  
  if(len >= metadataSize()) // If we do not receive at least the metadata bytes, the transmission is invalid.
  {    
    //uint32_t callbackStart = millis();

    // If there is a espnowRequestManager, get it
    EspnowMeshBackend *currentEspnowRequestManager = getEspnowRequestManager();
    
    char messageType = getMessageType(dataArray);
    uint64_t receivedMessageID = getMessageID(dataArray);

    if(currentEspnowRequestManager && !currentEspnowRequestManager->acceptsUnverifiedRequests() 
       && !usesConstantSessionKey(messageType) && !verifyPeerSessionKey(receivedMessageID, macaddr, messageType))
    {
      return;
    }

    if(useEncryptedMessages())
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
        if(!requestReceived(uint64StationMac, receivedMessageID)) // If the request has not already been received
        {
          if(transmissionEncrypted)
          {
            EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(macaddr);
            
            if(!encryptedConnection || (!synchronizePeerSessionKey(receivedMessageID, *encryptedConnection) &&
                                        !verifyPeerSessionKey(receivedMessageID, *encryptedConnection, uint64StationMac, messageType)))
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
        requestSender = getOwnerOfSentRequest(requestMac, receivedMessageID);
      }
      else
      {
        // An unencrypted transmission was probably sent to the AP interface as a result of a scan.
        requestMac = getTransmissionMac(dataArray);
        requestSender = getOwnerOfSentRequest(requestMac, receivedMessageID);

        // But if not, also check if it was sent to the station interface.
        if(!requestSender)
        {
          requestMac = uint64StationMac;
          requestSender = getOwnerOfSentRequest(requestMac, receivedMessageID);
        }

        // Or if it was sent as a broadcast. (A broadcast can never be encrypted)
        if(!requestSender)
        {
          requestSender = getOwnerOfSentRequest(uint64BroadcastMac, receivedMessageID);
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
      synchronizePeerSessionKey(receivedMessageID, macaddr);
    }
    else if(messageType == 'P') // Peer request
    {
      handlePeerRequest(macaddr, dataArray, len, uint64StationMac, receivedMessageID);
    }
    else if(messageType == 'C') // peer request Confirmation
    {
      handlePeerRequestConfirmation(macaddr, dataArray, len);
    }
    else
    {
      assert(messageType == 'Q' || messageType == 'A' || messageType == 'B' || messageType == 'S' || messageType == 'P' || messageType == 'C');
    }

    //Serial.println("espnowReceiveCallbackWrapper duration " + String(millis() - callbackStart));
  }
}

void EspnowMeshBackend::handlePeerRequest(const uint8_t *macaddr, uint8_t *dataArray, const uint8_t len, const uint64_t uint64StationMac, const uint64_t receivedMessageID)
{  
  // Pairing process ends when encryptedConnectionVerificationHeader is received, maxConnectionsReachedHeader is sent or timeout is reached.
  // Pairing process stages for request receiver:
  // Receive: encryptionRequestHeader or temporaryEncryptionRequestHeader.
  // Send: maxConnectionsReachedHeader / basicConnectionInfoHeader -> encryptedConnectionInfoHeader or softLimitEncryptedConnectionInfoHeader or maxConnectionsReachedHeader.
  // Receive: encryptedConnectionVerificationHeader.
  
  using namespace EspnowProtocolInterpreter;
  
  if(!requestReceived(uint64StationMac, receivedMessageID))
  {
    storeReceivedRequest(uint64StationMac, receivedMessageID, TimeTracker(millis()));
    
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
        connectionLogIterator encryptedConnection = connectionLogEndIterator();
        if(!getEncryptedConnectionIterator(macaddr, encryptedConnection))
          assert(false && String(F("We must have an encrypted connection if we received an encryptedConnectionVerificationHeader which was encryptedCorrectly.")));
        
        if(connectionRequestType == FPSTR(encryptionRequestHeader))
        {
          temporaryEncryptedConnectionToPermanent(macaddr);
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
      if(EspnowMeshBackend *currentEspnowRequestManager = getEspnowRequestManager())
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
          if(correctDestination && JsonTranslator::verifyEncryptionRequestHmac(message, macaddr, getTransmissionMac(dataArray, apMacArray), currentEspnowRequestManager->getEspnowHashKey(), hashKeyLength))
            peerRequestConfirmationsToSend.emplace_back(receivedMessageID, encryptedCorrectly, currentEspnowRequestManager->getMeshPassword(), currentEspnowRequestManager->encryptedConnectionsSoftLimit(), 
                                                        requestNonce, macaddr, apMacArray, currentEspnowRequestManager->getEspnowHashKey());
        }
      }
    }
    else if(messageHeader == FPSTR(encryptedConnectionRemovalRequestHeader))
    {
      if(encryptedCorrectly)
        removeEncryptedConnection(macaddr);
    }
    else
    {
      assert(false && String(F("Unknown P-type message received!")));
    }
  }
}

void EspnowMeshBackend::handlePeerRequestConfirmation(uint8_t *macaddr, uint8_t *dataArray, const uint8_t len)
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
           JsonTranslator::verifyEncryptionRequestHmac(message, macaddr, apMacArray, _ongoingPeerRequester->getEspnowHashKey(), hashKeyLength))
        {
           _ongoingPeerRequestEncryptionTimeout.reset();
  
          connectionLogIterator existingEncryptedConnection = connectionLogEndIterator();
          
          if(!getEncryptedConnectionIterator(macaddr, existingEncryptedConnection))
          {
            // Although the newly created session keys are normally never used (they are replaced with synchronized ones later), the session keys must still be randomized to prevent attacks until replaced.
            _ongoingPeerRequestResult = _ongoingPeerRequester->addTemporaryEncryptedConnection(macaddr, apMacArray, createSessionKey(), createSessionKey(), getEncryptionRequestTimeout());
          }
          else 
          {
            // Encrypted connection already exists
            _ongoingPeerRequestResult = EncryptedConnectionStatus::CONNECTION_ESTABLISHED;
            
            if(auto timeTrackerPointer = existingEncryptedConnection->temporary())
            {
              if(timeTrackerPointer->remainingDuration() < getEncryptionRequestTimeout()) // Should only extend duration for existing connections.
              {
                existingEncryptedConnection->setRemainingDuration(getEncryptionRequestTimeout());
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
          
          EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(macaddr);
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
        if(JsonTranslator::verifyEncryptionRequestHmac(message, macaddr, apMacArray, _ongoingPeerRequester->getEspnowHashKey(), hashKeyLength))
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
      if(receivedEspnowTransmissions.find(key) != receivedEspnowTransmissions.end())
        return; // Should not call BroadcastFilter more than once for an accepted message
      
      String message = getHashKeyLength(dataArray, len);
      setSenderMac(macaddr);
      getTransmissionMac(dataArray, _senderAPMac);
      setReceivedEncryptedTransmission(usesEncryption(messageID));
      bool acceptBroadcast = getBroadcastFilter()(message, *this);
      if(acceptBroadcast)
      {
        // Does nothing if key already in receivedEspnowTransmissions
        receivedEspnowTransmissions.insert(std::make_pair(key, MessageData(message, getTransmissionsRemaining(dataArray))));
      }
      else
      {
        return;
      }
    }
    else
    {  
      // Does nothing if key already in receivedEspnowTransmissions
      receivedEspnowTransmissions.insert(std::make_pair(std::make_pair(macAndType, messageID), MessageData(dataArray, len)));
    }
  }
  else
  {
    std::map<std::pair<macAndType_td, messageID_td>, MessageData>::iterator storedMessageIterator = receivedEspnowTransmissions.find(std::make_pair(macAndType, messageID));

    if(storedMessageIterator == receivedEspnowTransmissions.end()) // If we have not stored the key already, we missed the first message part.
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
        receivedEspnowTransmissions.erase(storedMessageIterator);
        return;
      }
    }
  }
  
  //Serial.println("methodStart storage done " + String(millis() - methodStart));
  
  if(transmissionsRemaining != 0)
  {
    return;
  }

  std::map<std::pair<macAndType_td, messageID_td>, MessageData>::iterator storedMessageIterator = receivedEspnowTransmissions.find(std::make_pair(macAndType, messageID));
  assert(storedMessageIterator != receivedEspnowTransmissions.end());

  // Copy totalMessage in case user callbacks (request/responseHandler) do something odd with receivedEspnowTransmissions list.
  String totalMessage = storedMessageIterator->second.getTotalMessage(); // https://stackoverflow.com/questions/134731/returning-a-const-reference-to-an-object-instead-of-a-copy It is likely that most compilers will perform Named Value Return Value Optimisation in this case

  receivedEspnowTransmissions.erase(storedMessageIterator); // Erase the extra copy of the totalMessage, to save RAM. 
   
  //Serial.println("methodStart erase done " + String(millis() - methodStart));
  
  if(messageType == 'Q' || messageType == 'B') // Question (request) or Broadcast
  {
    storeReceivedRequest(uint64Mac, messageID, TimeTracker(millis()));
    //Serial.println("methodStart request stored " + String(millis() - methodStart));
      
    setSenderMac(macaddr);
    getTransmissionMac(dataArray, _senderAPMac);
    setReceivedEncryptedTransmission(usesEncryption(messageID));
    String response = getRequestHandler()(totalMessage, *this);
    //Serial.println("methodStart response acquired " + String(millis() - methodStart));
     
    if(response.length() > 0)
    {
      responsesToSend.push_back(ResponseData(response, macaddr, messageID));
      
      //Serial.println("methodStart Q done " + String(millis() - methodStart));
    }
  }
  else if(messageType == 'A') // Answer (response)
  {
    deleteSentRequest(uint64Mac, messageID); // Request has been answered, so stop accepting new answers about it.

    if(EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(macaddr))
    {
      if(encryptedConnection->getOwnSessionKey() == messageID)
      {
        encryptedConnection->setDesync(false); // We just received an answer to the latest request we sent to the node, so the node sending the answer must now be in sync.
        encryptedConnection->incrementOwnSessionKey();
      }
    } 
    
    setSenderMac(macaddr);
    getTransmissionMac(dataArray, _senderAPMac);
    setReceivedEncryptedTransmission(usesEncryption(messageID));
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

bool EspnowMeshBackend::encryptedConnectionEstablished(const EncryptedConnectionStatus connectionStatus)
{
  return static_cast<int>(connectionStatus) > 0;
}

void EspnowMeshBackend::setLogEntryLifetimeMs(const uint32_t logEntryLifetimeMs)
{
  _logEntryLifetimeMs = logEntryLifetimeMs;
}
uint32_t EspnowMeshBackend::logEntryLifetimeMs() { return _logEntryLifetimeMs; }

void EspnowMeshBackend::setBroadcastResponseTimeoutMs(const uint32_t broadcastResponseTimeoutMs)
{
  _broadcastResponseTimeoutMs = broadcastResponseTimeoutMs;
}
uint32_t EspnowMeshBackend::broadcastResponseTimeoutMs() { return _broadcastResponseTimeoutMs; }

void EspnowMeshBackend::setCriticalHeapLevelBuffer(const uint32_t bufferInBytes)
{
  _criticalHeapLevelBuffer = bufferInBytes;
}

uint32_t EspnowMeshBackend::criticalHeapLevelBuffer()
{
  return _criticalHeapLevelBuffer;
}

template <typename T>
T *EspnowMeshBackend::getMapValue(std::map<uint64_t, T> &mapIn, const uint64_t keyIn)
{
  typename std::map<uint64_t, T>::iterator mapIterator = mapIn.find(keyIn);

  if(mapIterator != mapIn.end())
  {
    return &mapIterator->second;
  }

  return nullptr;
}

void EspnowMeshBackend::storeSentRequest(const uint64_t targetBSSID, const uint64_t messageID, const RequestData &requestData)
{
  sentRequests.insert(std::make_pair(std::make_pair(targetBSSID, messageID), requestData));
}

void EspnowMeshBackend::storeReceivedRequest(const uint64_t senderBSSID, const uint64_t messageID, const TimeTracker &timeTracker)
{
  receivedRequests.insert(std::make_pair(std::make_pair(senderBSSID, messageID), timeTracker));
}

EspnowMeshBackend *EspnowMeshBackend::getOwnerOfSentRequest(const uint64_t requestMac, const uint64_t requestID)
{
  std::map<std::pair<peerMac_td, messageID_td>, RequestData>::iterator sentRequest = sentRequests.find(std::make_pair(requestMac, requestID));
  
  if(sentRequest != sentRequests.end())
  {
    return &sentRequest->second.getMeshInstance();
  }
  
  return nullptr;
}

size_t EspnowMeshBackend::deleteSentRequest(const uint64_t requestMac, const uint64_t requestID)
{
  return sentRequests.erase(std::make_pair(requestMac, requestID));
}

size_t EspnowMeshBackend::deleteSentRequestsByOwner(const EspnowMeshBackend *instancePointer)
{
  size_t numberDeleted = 0;
  
  for(std::map<std::pair<peerMac_td, messageID_td>, RequestData>::iterator requestIterator = sentRequests.begin(); 
      requestIterator != sentRequests.end(); )
  {
    if(&requestIterator->second.getMeshInstance() == instancePointer) // If instance at instancePointer made the request
    {
      requestIterator = sentRequests.erase(requestIterator);
      numberDeleted++;
    }
    else
      ++requestIterator;
  }

  return numberDeleted;
}

bool EspnowMeshBackend::requestReceived(const uint64_t requestMac, const uint64_t requestID)
{
  return receivedRequests.count(std::make_pair(requestMac, requestID));
}

uint32_t EspnowMeshBackend::criticalHeapLevel()
{
  return _criticalHeapLevel;
}

uint64_t EspnowMeshBackend::generateMessageID(const EncryptedConnectionLog *encryptedConnection)
{
  if(encryptedConnection)
  {
    return encryptedConnection->getOwnSessionKey();
  }

  return _unsynchronizedMessageID++;
}

uint64_t EspnowMeshBackend::createSessionKey()
{
  uint64_t newSessionKey = MeshUtilityFunctions::randomUint64();
  return EspnowProtocolInterpreter::usesEncryption(newSessionKey) ? newSessionKey : (newSessionKey | ((uint64_t)RANDOM_REG32) << 32 | uint64MSB);
}

void EspnowMeshBackend::setEspnowTransmissionTimeout(const uint32_t timeoutMs)
{
  _espnowTransmissionTimeoutMs = timeoutMs;
}
uint32_t EspnowMeshBackend::getEspnowTransmissionTimeout() {return _espnowTransmissionTimeoutMs;}

void EspnowMeshBackend::setEspnowRetransmissionInterval(const uint32_t intervalMs)
{
  _espnowRetransmissionIntervalMs = intervalMs;
}
uint32_t EspnowMeshBackend::getEspnowRetransmissionInterval() {return _espnowRetransmissionIntervalMs;}

void EspnowMeshBackend::setEncryptionRequestTimeout(const uint32_t timeoutMs)
{
  _encryptionRequestTimeoutMs = timeoutMs;
}
uint32_t EspnowMeshBackend::getEncryptionRequestTimeout() {return _encryptionRequestTimeoutMs;}

void EspnowMeshBackend::setAutoEncryptionDuration(const uint32_t duration)
{
  _autoEncryptionDuration = duration;
}
uint32_t EspnowMeshBackend::getAutoEncryptionDuration() const {return _autoEncryptionDuration;}

void EspnowMeshBackend::setBroadcastFilter(const broadcastFilterType broadcastFilter) {_broadcastFilter = broadcastFilter;}
EspnowMeshBackend::broadcastFilterType EspnowMeshBackend::getBroadcastFilter() const {return _broadcastFilter;}

bool EspnowMeshBackend::usesConstantSessionKey(const char messageType)
{
  return messageType == 'A' || messageType == 'C';
}

TransmissionStatusType EspnowMeshBackend::espnowSendToNode(const String &message, const uint8_t *targetBSSID, const char messageType, EspnowMeshBackend *espnowInstance)
{
  using EspnowProtocolInterpreter::synchronizationRequestHeader;
  
  EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(targetBSSID);
  
  if(encryptedConnection)
  {
    uint8_t encryptedMac[6] {0};
    encryptedConnection->getEncryptedPeerMac(encryptedMac);
    
    assert(esp_now_is_peer_exist(encryptedMac) > 0 && String(F("ERROR! Attempting to send content marked as encrypted via unencrypted connection!")));
    
    if(encryptedConnection->desync())
    {
      espnowSendToNodeUnsynchronized(FPSTR(synchronizationRequestHeader), encryptedMac, 'S', generateMessageID(encryptedConnection), espnowInstance);
  
      if(encryptedConnection->desync())
      {
        return TransmissionStatusType::TRANSMISSION_FAILED;
      }
    }

    return espnowSendToNodeUnsynchronized(message, encryptedMac, messageType, generateMessageID(encryptedConnection), espnowInstance);
  }
  
  return espnowSendToNodeUnsynchronized(message, targetBSSID, messageType, generateMessageID(encryptedConnection), espnowInstance);
}

TransmissionStatusType EspnowMeshBackend::espnowSendToNodeUnsynchronized(const String message, const uint8_t *targetBSSID, const char messageType, const uint64_t messageID, EspnowMeshBackend *espnowInstance)
{
  using namespace EspnowProtocolInterpreter;
  
  MutexTracker mutexTracker(_espnowSendToNodeMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! espnowSendToNode already in progress. Don't call espnowSendToNode from callbacks as this will make it impossible to know which transmissions succeed! Aborting."))); 
    return TransmissionStatusType::TRANSMISSION_FAILED;
  }

  // We copy the message String and bssid array from the arguments in this method to make sure they are
  // not modified by a callback during the delay(1) calls further down. 
  // This also makes it possible to get the current _transmissionTargetBSSID outside of the method.
  std::copy_n(targetBSSID, 6, _transmissionTargetBSSID);
  
  EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(_transmissionTargetBSSID);
  
  int32_t transmissionsRequired = ceil((double)message.length() / getMaxMessageBytesPerTransmission());
  int32_t transmissionsRemaining = transmissionsRequired > 1 ? transmissionsRequired - 1 : 0;

  _transmissionsTotal++;

  // Though it is possible to handle messages requiring more than 3 transmissions with the current design, transmission fail rates would increase dramatically. 
  // Messages composed of up to 128 transmissions can be handled without modification, but RAM limitations on the ESP8266 would make this hard in practice. 
  // We thus prefer to keep the code simple and performant instead.
  // Very large messages can always be split by the user as required. 
  assert(transmissionsRequired <= getMaxTransmissionsPerMessage());
  assert(messageType == 'Q' || messageType == 'A' || messageType == 'B' || messageType == 'S' || messageType == 'P' || messageType == 'C');
  if(messageType == 'P' || messageType == 'C')
  {
    assert(transmissionsRequired == 1); // These messages are assumed to be contained in one message by the receive callbacks.
  }
  
  uint8_t transmissionSize = 0;
  bool messageStart = true;
  uint8_t espnowMetadataSize = metadataSize();

  do
  {
    ////// Manage logs //////
    
    if(transmissionsRemaining == 0 && (messageType == 'Q' || messageType == 'B'))
    {
      assert(espnowInstance); // espnowInstance required when transmitting 'Q' and 'B' type messages.
      // If we are sending the last transmission of a request we should store the sent request in the log no matter if we receive an ack for the final transmission or not.
      // That way we will always be ready to receive the response to the request when there is a chance the request message was transmitted successfully, 
      // even if the final ack for the request message was lost.
      storeSentRequest(TypeCast::macToUint64(_transmissionTargetBSSID), messageID, RequestData(*espnowInstance));
    }
    
    ////// Create transmission array //////
    
    if(transmissionsRemaining > 0)
    {
      transmissionSize = getMaxBytesPerTransmission();
    }
    else
    {
      transmissionSize = espnowMetadataSize;
      
      if(message.length() > 0)
      {
        uint32_t remainingLength = message.length() % getMaxMessageBytesPerTransmission();
        transmissionSize += (remainingLength == 0 ? getMaxMessageBytesPerTransmission() : remainingLength);
      }
    }
    
    uint8_t transmission[transmissionSize];

    ////// Fill protocol bytes //////
    
    transmission[messageTypeIndex] = messageType;
    
    if(messageStart)
    {
      transmission[transmissionsRemainingIndex] = (char)(transmissionsRemaining | 0x80);
    }
    else
    {
      transmission[transmissionsRemainingIndex] = (char)transmissionsRemaining;
    }

    // Fills indicies in range [transmissionMacIndex, transmissionMacIndex + 5] (6 bytes) with the MAC address of the WiFi AP interface.
    // We always transmit from the station interface (due to using ESP_NOW_ROLE_CONTROLLER), so this makes it possible to always know both interface MAC addresses of a node that sends a transmission.
    WiFi.softAPmacAddress(transmission + transmissionMacIndex);

    setMessageID(transmission, messageID);

    ////// Fill message bytes //////
    
    int32_t transmissionStartIndex = (transmissionsRequired - transmissionsRemaining - 1) * getMaxMessageBytesPerTransmission();
    
    std::copy_n(message.begin() + transmissionStartIndex, transmissionSize - espnowMetadataSize, transmission + espnowMetadataSize);

    if(useEncryptedMessages())
    {      
      // chacha20Poly1305Encrypt encrypts transmission in place.
      // We are using the protocol bytes as a key salt.
      CryptoInterface::chacha20Poly1305Encrypt(transmission + espnowMetadataSize, transmissionSize - espnowMetadataSize, getEspnowMessageEncryptionKey(), transmission, 
                                               protocolBytesSize, transmission + protocolBytesSize, transmission + protocolBytesSize + 12);
    }
    
    ////// Transmit //////

    uint32_t retransmissions = 0;
    if(messageType == 'B')
      retransmissions = espnowInstance->getBroadcastTransmissionRedundancy();
      
    for(uint32_t i = 0; i <= retransmissions; ++i)
    {
      _espnowSendConfirmed = false;
      ExpiringTimeTracker transmissionTimeout([](){ return getEspnowTransmissionTimeout(); });
      
      while(!_espnowSendConfirmed && !transmissionTimeout)
      {
        if(esp_now_send(_transmissionTargetBSSID, transmission, transmissionSize) == 0) // == 0 => Success
        {
          ExpiringTimeTracker retransmissionTime([](){ return getEspnowRetransmissionInterval(); });
          while(!_espnowSendConfirmed && !retransmissionTime && !transmissionTimeout)
          {        
            delay(1); // Note that callbacks can be called during delay time, so it is possible to receive a transmission during this delay.
          }
        }
  
        if(_espnowSendConfirmed)
        {
          if(messageStart)
          {        
            if(encryptedConnection && !usesConstantSessionKey(messageType) && encryptedConnection->getOwnSessionKey() == messageID)
            {
              encryptedConnection->setDesync(false);
              encryptedConnection->incrementOwnSessionKey();
            }
            
            messageStart = false;
          }  
          
          break;
        }
      }
    }
    
    if(!_espnowSendConfirmed)
    {
      ++_transmissionsFailed;

      staticVerboseModePrint(String(F("espnowSendToNode failed!")));
      staticVerboseModePrint(String(F("Transmission #: ")) + String(transmissionsRequired - transmissionsRemaining) + String('/') + String(transmissionsRequired));
      staticVerboseModePrint(String(F("Transmission fail rate (up) ")) + String(getTransmissionFailRate()));

      if(messageStart && encryptedConnection && !usesConstantSessionKey(messageType) && encryptedConnection->getOwnSessionKey() == messageID)
        encryptedConnection->setDesync(true);
      
      return TransmissionStatusType::TRANSMISSION_FAILED;
    }

    --transmissionsRemaining; // This is used when transfering multi-transmission messages.
    
  } while(transmissionsRemaining >= 0);

  // Useful when debugging the protocol
  //staticVerboseModePrint("Sent to Mac: " + TypeCast::macToString(_transmissionTargetBSSID) + " ID: " + TypeCast::uint64ToString(messageID)); 
  
  return TransmissionStatusType::TRANSMISSION_COMPLETE;
}

TransmissionStatusType EspnowMeshBackend::sendRequest(const String &message, const uint8_t *targetBSSID)
{
  TransmissionStatusType transmissionStatus = espnowSendToNode(message, targetBSSID, 'Q', this);

  return transmissionStatus;
}

TransmissionStatusType EspnowMeshBackend::sendResponse(const String &message, const uint64_t requestID, const uint8_t *targetBSSID)
{
  EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(targetBSSID);
  uint8_t encryptedMac[6] {0};
    
  if(encryptedConnection)
  {
    encryptedConnection->getEncryptedPeerMac(encryptedMac);
    assert(esp_now_is_peer_exist(encryptedMac) > 0 && String(F("ERROR! Attempting to send content marked as encrypted via unencrypted connection!")));
  }
  
  return espnowSendToNodeUnsynchronized(message, encryptedConnection ? encryptedMac : targetBSSID, 'A', requestID, this);
}

bool EspnowMeshBackend::transmissionInProgress(){return *_espnowTransmissionMutex;}

EspnowMeshBackend::macAndType_td EspnowMeshBackend::createMacAndTypeValue(const uint64_t uint64Mac, const char messageType)
{
  return static_cast<macAndType_td>(uint64Mac << 8 | (uint64_t)messageType);
}

uint64_t EspnowMeshBackend::macAndTypeToUint64Mac(const macAndType_td &macAndTypeValue)
{
  return static_cast<uint64_t>(macAndTypeValue) >> 8;
}

void EspnowMeshBackend::setEspnowEncryptedConnectionKey(const uint8_t espnowEncryptedConnectionKey[encryptedConnectionKeyLength])
{
  assert(espnowEncryptedConnectionKey != nullptr);
   
  for(int i = 0; i < encryptedConnectionKeyLength; ++i)
  {
    _espnowEncryptedConnectionKey[i] = espnowEncryptedConnectionKey[i];
  }
}

void EspnowMeshBackend::setEspnowEncryptedConnectionKey(const String &espnowEncryptedConnectionKeySeed)
{
  MeshCryptoInterface::initializeKey(_espnowEncryptedConnectionKey, encryptedConnectionKeyLength, espnowEncryptedConnectionKeySeed);
}

const uint8_t *EspnowMeshBackend::getEspnowEncryptedConnectionKey() const
{
  return _espnowEncryptedConnectionKey;
}

uint8_t *EspnowMeshBackend::getEspnowEncryptedConnectionKey(uint8_t resultArray[encryptedConnectionKeyLength]) const
{
  std::copy_n(_espnowEncryptedConnectionKey, encryptedConnectionKeyLength, resultArray);
  return resultArray;
}

bool EspnowMeshBackend::setEspnowEncryptionKok(uint8_t espnowEncryptionKok[encryptedConnectionKeyLength])
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

bool EspnowMeshBackend::setEspnowEncryptionKok(const String &espnowEncryptionKokSeed)
{
  uint8_t espnowEncryptionKok[encryptedConnectionKeyLength] {};
  MeshCryptoInterface::initializeKey(espnowEncryptionKok, encryptedConnectionKeyLength, espnowEncryptionKokSeed);

  return setEspnowEncryptionKok(espnowEncryptionKok);
}

const uint8_t *EspnowMeshBackend::getEspnowEncryptionKok()
{
  if(_espnowEncryptionKokSet)
    return _espnowEncryptionKok;
  else
    return nullptr;
}

void EspnowMeshBackend::setEspnowHashKey(const uint8_t espnowHashKey[hashKeyLength])
{
  assert(espnowHashKey != nullptr);

  for(int i = 0; i < hashKeyLength; ++i)
  {
    _espnowHashKey[i] = espnowHashKey[i];
  }
}

void EspnowMeshBackend::setEspnowHashKey(const String &espnowHashKeySeed)
{
  MeshCryptoInterface::initializeKey(_espnowHashKey, hashKeyLength, espnowHashKeySeed);
}

const uint8_t *EspnowMeshBackend::getEspnowHashKey() const
{
  return _espnowHashKey;
}

void EspnowMeshBackend::setEspnowMessageEncryptionKey(const uint8_t espnowMessageEncryptionKey[CryptoInterface::ENCRYPTION_KEY_LENGTH])
{
  assert(espnowMessageEncryptionKey != nullptr);
   
  for(int i = 0; i < CryptoInterface::ENCRYPTION_KEY_LENGTH; ++i)
  {
    _espnowMessageEncryptionKey[i] = espnowMessageEncryptionKey[i];
  }
}

void EspnowMeshBackend::setEspnowMessageEncryptionKey(const String &espnowMessageEncryptionKeySeed)
{
  MeshCryptoInterface::initializeKey(_espnowMessageEncryptionKey, CryptoInterface::ENCRYPTION_KEY_LENGTH, espnowMessageEncryptionKeySeed);
}

const uint8_t *EspnowMeshBackend::getEspnowMessageEncryptionKey()
{
  return _espnowMessageEncryptionKey;
}

void EspnowMeshBackend::setUseEncryptedMessages(const bool useEncryptedMessages) 
{
  MutexTracker mutexTracker(_espnowSendToNodeMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! espnowSendToNode in progress. Don't call setUseEncryptedMessages from non-hook callbacks since this may modify the ESP-NOW transmission parameters during ongoing transmissions! Aborting.")));
  }
  
  _useEncryptedMessages = useEncryptedMessages; 
}
bool EspnowMeshBackend::useEncryptedMessages() { return _useEncryptedMessages; }

bool EspnowMeshBackend::verifyPeerSessionKey(const uint64_t sessionKey, const uint8_t *peerMac, const char messageType)
{
  if(EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerMac))
  {
    return verifyPeerSessionKey(sessionKey, *encryptedConnection, TypeCast::macToUint64(peerMac), messageType);
  }

  return false;
}

bool EspnowMeshBackend::verifyPeerSessionKey(const uint64_t sessionKey, const EncryptedConnectionLog &encryptedConnection, const uint64_t uint64PeerMac, const char messageType)
{
  if(EspnowProtocolInterpreter::usesEncryption(sessionKey))
  {
    if(sessionKey == encryptedConnection.getPeerSessionKey() 
       || receivedEspnowTransmissions.find(std::make_pair(createMacAndTypeValue(uint64PeerMac, messageType), sessionKey)) 
          != receivedEspnowTransmissions.end())
    {
      // If sessionKey is correct or sessionKey is one part of a multi-part transmission.
      return true;
    }
  }

  return false;
}

bool EspnowMeshBackend::synchronizePeerSessionKey(const uint64_t sessionKey, const uint8_t *peerMac)
{
  if(EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerMac))
  {
    return synchronizePeerSessionKey(sessionKey, *encryptedConnection);
  }

  return false;
}

bool EspnowMeshBackend::synchronizePeerSessionKey(const uint64_t sessionKey, EncryptedConnectionLog &encryptedConnection)
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

std::list<ResponseData>::const_iterator EspnowMeshBackend::getScheduledResponse(const uint32_t responseIndex)
{
  assert(responseIndex < numberOfScheduledResponses());

  bool startFromBeginning = responseIndex < numberOfScheduledResponses()/2;
  auto responseIterator = startFromBeginning ? responsesToSend.cbegin() : responsesToSend.cend();
  uint32_t stepsToTarget = startFromBeginning ? responseIndex : numberOfScheduledResponses() - responseIndex; // cend is one element beyond the last

  while(stepsToTarget > 0)
  {
    startFromBeginning ? ++responseIterator : --responseIterator;
    --stepsToTarget;
  }

  return responseIterator;
}

String EspnowMeshBackend::getScheduledResponseMessage(const uint32_t responseIndex)
{
  return getScheduledResponse(responseIndex)->getMessage();
}

const uint8_t *EspnowMeshBackend::getScheduledResponseRecipient(const uint32_t responseIndex)
{
  return getScheduledResponse(responseIndex)->getRecipientMac();
}

uint32_t EspnowMeshBackend::numberOfScheduledResponses() {return responsesToSend.size();}

void EspnowMeshBackend::clearAllScheduledResponses()
{
  MutexTracker responsesToSendMutexTracker(_responsesToSendMutex);
  if(!responsesToSendMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! responsesToSend locked. Don't call clearAllScheduledResponses from callbacks as this may corrupt program state! Aborting."))); 
  }
  
  responsesToSend.clear();
}

void EspnowMeshBackend::deleteScheduledResponsesByRecipient(const uint8_t *recipientMac, const bool encryptedOnly)
{
  MutexTracker responsesToSendMutexTracker(_responsesToSendMutex);
  if(!responsesToSendMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! responsesToSend locked. Don't call deleteScheduledResponsesByRecipient from callbacks as this may corrupt program state! Aborting."))); 
  }
  
  for(auto responseIterator = responsesToSend.begin(); responseIterator != responsesToSend.end(); )
  {
    if(MeshUtilityFunctions::macEqual(responseIterator->getRecipientMac(), recipientMac) && 
       (!encryptedOnly || EspnowProtocolInterpreter::usesEncryption(responseIterator->getRequestID())))
    {
      responseIterator = responsesToSend.erase(responseIterator);
    }
    else
      ++responseIterator;
  }
}

void EspnowMeshBackend::setSenderMac(const uint8_t *macArray)
{
  std::copy_n(macArray, 6, _senderMac);
}

String EspnowMeshBackend::getSenderMac() const {return TypeCast::macToString(_senderMac);}
uint8_t *EspnowMeshBackend::getSenderMac(uint8_t *macArray) const
{
  std::copy_n(_senderMac, 6, macArray);
  return macArray;
}

void EspnowMeshBackend::setSenderAPMac(const uint8_t *macArray)
{
  std::copy_n(macArray, 6, _senderAPMac);
}

String EspnowMeshBackend::getSenderAPMac() const {return TypeCast::macToString(_senderAPMac);}
uint8_t *EspnowMeshBackend::getSenderAPMac(uint8_t *macArray) const
{
  std::copy_n(_senderAPMac, 6, macArray);
  return macArray;
}

void EspnowMeshBackend::setReceivedEncryptedTransmission(const bool receivedEncryptedTransmission) { _receivedEncryptedTransmission = receivedEncryptedTransmission; }
bool EspnowMeshBackend::receivedEncryptedTransmission() const {return _receivedEncryptedTransmission;}

bool EspnowMeshBackend::addUnencryptedConnection(const String &serializedConnectionState)
{
  return JsonTranslator::getUnsynchronizedMessageID(serializedConnectionState, _unsynchronizedMessageID);
}

EncryptedConnectionStatus EspnowMeshBackend::addEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey)
{
  assert(encryptedConnections.size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library

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

  if(encryptedConnections.size() == maxEncryptedConnections)
  {
    // No capacity for more encrypted connections.
    return EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_SELF;
  }
  // returns 0 on success: int esp_now_add_peer(u8 *mac_addr, u8 role, u8 channel, u8 *key, u8 key_len)
  // Only MAC, encryption key and key length (16) actually matter. The rest is not used by ESP-NOW.
  else if(0 == esp_now_add_peer(peerStaMac, ESP_NOW_ROLE_CONTROLLER, getWiFiChannel(), getEspnowEncryptedConnectionKey(encryptionKeyArray), encryptedConnectionKeyLength))
  {
    encryptedConnections.emplace_back(peerStaMac, peerApMac, peerSessionKey, ownSessionKey, getEspnowHashKey());
    return EncryptedConnectionStatus::CONNECTION_ESTABLISHED;
  }
  else
  {
    return EncryptedConnectionStatus::API_CALL_FAILED;
  }
}

EncryptedConnectionStatus EspnowMeshBackend::addEncryptedConnection(const String &serializedConnectionState, const bool ignoreDuration)
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

EncryptedConnectionStatus EspnowMeshBackend::addTemporaryEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint32_t duration)
{  
  assert(encryptedConnections.size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library

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

EncryptedConnectionStatus EspnowMeshBackend::addTemporaryEncryptedConnection(const String &serializedConnectionState, const uint32_t duration)
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

void EspnowMeshBackend::handlePostponedRemovals()
{
  MutexTracker mutexTracker(_espnowTransmissionMutex);
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

EncryptedConnectionStatus EspnowMeshBackend::requestEncryptedConnectionKernel(const uint8_t *peerMac, const encryptionRequestBuilderType &encryptionRequestBuilder)
{
  using namespace EspnowProtocolInterpreter;
  
  assert(encryptedConnections.size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library
  
  MutexTracker mutexTracker(_espnowTransmissionMutex, handlePostponedRemovals);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call requestEncryptedConnection from callbacks as this may corrupt program state! Aborting."))); 
    return EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
  }

  EncryptedConnectionLog *existingEncryptedConnection = getEncryptedConnection(peerMac);
  ExpiringTimeTracker existingTimeTracker = existingEncryptedConnection && existingEncryptedConnection->temporary() ? 
                                            *existingEncryptedConnection->temporary() : ExpiringTimeTracker(0);
  
  if(!existingEncryptedConnection && encryptedConnections.size() >= maxEncryptedConnections)
  {
    assert(encryptedConnections.size() == maxEncryptedConnections);
    
    // No capacity for more encrypted connections.
    return EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_SELF;
  }

  String requestNonce = TypeCast::macToString(peerMac) + TypeCast::uint64ToString(MeshUtilityFunctions::randomUint64()) 
                                                       + TypeCast::uint64ToString(MeshUtilityFunctions::randomUint64());
  _ongoingPeerRequestResult = EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED;
  _ongoingPeerRequestNonce = requestNonce;
  _ongoingPeerRequester = this;
  _reciprocalPeerRequestConfirmation = false;
  std::copy_n(peerMac, 6, _ongoingPeerRequestMac);
  String requestMessage = encryptionRequestBuilder(requestNonce, existingTimeTracker);

  verboseModePrint(String(F("Sending encrypted connection request to: ")) + TypeCast::macToString(peerMac));

  if(espnowSendToNode(requestMessage, peerMac, 'P') == TransmissionStatusType::TRANSMISSION_COMPLETE)
  {
    ExpiringTimeTracker requestTimeout([](){ return getEncryptionRequestTimeout(); });
    // _ongoingPeerRequestNonce is set to "" when a peer confirmation response from the mac is received
    while(!requestTimeout && !_ongoingPeerRequestNonce.isEmpty())
    {
      // For obvious reasons dividing by exactly 10 is a good choice.
      ExpiringTimeTracker maxDurationTracker = ExpiringTimeTracker(getEncryptionRequestTimeout()/10);
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
      requestMessage = JsonTranslator::createEncryptionRequestHmacMessage(FPSTR(temporaryEncryptionRequestHeader), requestNonce, getEspnowHashKey(), 
                                                                          hashKeyLength, getAutoEncryptionDuration());
    else
      assert(false && String(F("Unknown _ongoingPeerRequestResult during encrypted connection finalization!")));
    
    int32_t messageHeaderEndIndex = requestMessage.indexOf(':');
    String messageHeader = requestMessage.substring(0, messageHeaderEndIndex + 1);
    String messageBody = requestMessage.substring(messageHeaderEndIndex + 1);
    
    // If we do not get an ack within getEncryptionRequestTimeout() the peer has probably had the time to delete the temporary encrypted connection.
    if(espnowSendToNode(String(FPSTR(encryptedConnectionVerificationHeader)) + requestMessage, peerMac, 'P') == TransmissionStatusType::TRANSMISSION_COMPLETE 
       && !_ongoingPeerRequestEncryptionTimeout)
    {
      EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerMac);
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
          temporaryEncryptedConnectionToPermanent(peerMac);
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
      removeEncryptedConnectionUnprotected(peerMac);
    }
  }
  
  _ongoingPeerRequester = nullptr;

  return _ongoingPeerRequestResult;
}

String EspnowMeshBackend::defaultEncryptionRequestBuilder(const String &requestHeader, const uint32_t durationMs, const uint8_t *hashKey,
                                                         const String &requestNonce, const ExpiringTimeTracker &existingTimeTracker)
{
  (void)existingTimeTracker; // This removes a "unused parameter" compiler warning. Does nothing else.
  
  return JsonTranslator::createEncryptionRequestHmacMessage(requestHeader, requestNonce, hashKey, hashKeyLength, durationMs);
}
    
String EspnowMeshBackend::flexibleEncryptionRequestBuilder(const uint32_t minDurationMs, const uint8_t *hashKey, 
                                                           const String &requestNonce, const ExpiringTimeTracker &existingTimeTracker)
{
  using namespace JsonTranslator;
  using EspnowProtocolInterpreter::temporaryEncryptionRequestHeader;

  uint32_t connectionDuration = minDurationMs >= existingTimeTracker.remainingDuration() ? 
                                minDurationMs : existingTimeTracker.remainingDuration();

  return createEncryptionRequestHmacMessage(FPSTR(temporaryEncryptionRequestHeader), requestNonce, hashKey, hashKeyLength, connectionDuration);
}

EncryptedConnectionStatus EspnowMeshBackend::requestEncryptedConnection(const uint8_t *peerMac)
{
  using namespace std::placeholders;
  return requestEncryptedConnectionKernel(peerMac, std::bind(defaultEncryptionRequestBuilder, FPSTR(EspnowProtocolInterpreter::encryptionRequestHeader), 0, getEspnowHashKey(), _1, _2));
}

EncryptedConnectionStatus EspnowMeshBackend::requestTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t durationMs)
{
  using namespace std::placeholders;
  return requestEncryptedConnectionKernel(peerMac, std::bind(defaultEncryptionRequestBuilder, FPSTR(EspnowProtocolInterpreter::temporaryEncryptionRequestHeader), 
                                                             durationMs, getEspnowHashKey(), _1, _2));
}

EncryptedConnectionStatus EspnowMeshBackend::requestFlexibleTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t minDurationMs)
{
  using namespace std::placeholders;
  return requestEncryptedConnectionKernel(peerMac, std::bind(flexibleEncryptionRequestBuilder, minDurationMs, getEspnowHashKey(), _1, _2));
}

bool EspnowMeshBackend::temporaryEncryptedConnectionToPermanent(const uint8_t *peerMac)
{
  if(EncryptedConnectionLog *temporaryConnection = getTemporaryEncryptedConnection(peerMac))
  {
    temporaryConnection->removeDuration();
    return true;
  }
  
  return false;
}

EncryptedConnectionRemovalOutcome EspnowMeshBackend::removeEncryptedConnection(const uint8_t *peerMac)
{
  auto connectionIterator = getEncryptedConnectionIterator(peerMac, encryptedConnections);
  if(connectionIterator != encryptedConnections.end())
  {
    MutexTracker mutexTracker(_espnowTransmissionMutex);
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

EncryptedConnectionRemovalOutcome EspnowMeshBackend::removeEncryptedConnectionUnprotected(const uint8_t *peerMac, std::vector<EncryptedConnectionLog>::iterator *resultingIterator)
{
  connectionLogIterator connectionIterator = getEncryptedConnectionIterator(peerMac, encryptedConnections);
  return removeEncryptedConnectionUnprotected(connectionIterator, resultingIterator);
}

EncryptedConnectionRemovalOutcome EspnowMeshBackend::removeEncryptedConnectionUnprotected(connectionLogIterator &connectionIterator, std::vector<EncryptedConnectionLog>::iterator *resultingIterator)
{
  assert(encryptedConnections.size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library

  if(connectionIterator != connectionLogEndIterator())
  {
    uint8_t encryptedMac[6] {0};
    connectionIterator->getEncryptedPeerMac(encryptedMac);
    staticVerboseModePrint(String(F("Removing connection ")) + TypeCast::macToString(encryptedMac) + String(F("... ")), false);
    bool removalSucceeded = esp_now_del_peer(encryptedMac) == 0;
    
    if(removalSucceeded)
    {
      if(resultingIterator != nullptr)
      {
        *resultingIterator = encryptedConnections.erase(connectionIterator);
      }
      else
      {
        encryptedConnections.erase(connectionIterator);
      }
      staticVerboseModePrint(String(F("Removal succeeded")));
      
      // Not deleting encrypted responses here would cause them to be sent unencrypted, 
      // exposing the peer session key which can be misused later if the encrypted connection is re-established.
      deleteScheduledResponsesByRecipient(encryptedMac, true);
      
      // Not deleting these entries here may cause issues if the encrypted connection is quickly re-added
      // and happens to get the same session keys as before (e.g. requestReceived() could then give false positives).
      deleteEntriesByMac(receivedEspnowTransmissions, encryptedMac, true);
      deleteEntriesByMac(sentRequests, encryptedMac, true);
      deleteEntriesByMac(receivedRequests, encryptedMac, true);
      
      return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED;
    }
    else
    {
      staticVerboseModePrint(String(F("Removal failed")));
      return EncryptedConnectionRemovalOutcome::REMOVAL_FAILED;
    }
  }

  // connection is already removed
  return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED;
}

template <typename T>
void EspnowMeshBackend::deleteEntriesByMac(std::map<std::pair<macAndType_td, uint64_t>, T> &logEntries, const uint8_t *peerMac, const bool encryptedOnly)
{
  bool macFound = false;
  
  for(typename std::map<std::pair<macAndType_td, uint64_t>, T>::iterator entryIterator = logEntries.begin(); 
      entryIterator != logEntries.end(); )
  {
    if(macAndTypeToUint64Mac(entryIterator->first.first) == TypeCast::macToUint64(peerMac))
    {
      macFound = true;
         
      if(!encryptedOnly || EspnowProtocolInterpreter::usesEncryption(entryIterator->first.second))
      {
        entryIterator = logEntries.erase(entryIterator);
        continue;
      }
    }
    else if(macFound)
    {
      // Since the map is sorted by MAC, we know here that no more matching MAC will be found.
      return;
    }

    ++entryIterator;
  }
}

template <typename T>
void EspnowMeshBackend::deleteEntriesByMac(std::map<std::pair<uint64_t, uint64_t>, T> &logEntries, const uint8_t *peerMac, const bool encryptedOnly)
{
  bool macFound = false;
  
  for(typename std::map<std::pair<uint64_t, uint64_t>, T>::iterator entryIterator = logEntries.begin(); 
      entryIterator != logEntries.end(); )
  {
    if(entryIterator->first.first == TypeCast::macToUint64(peerMac))
    {
      macFound = true;
         
      if(!encryptedOnly || EspnowProtocolInterpreter::usesEncryption(entryIterator->first.second))
      {
        entryIterator = logEntries.erase(entryIterator);
        continue;
      }
    }
    else if(macFound)
    {
      // Since the map is sorted by MAC, we know here that no more matching MAC will be found.
      return;
    }

    ++entryIterator;
  }
}

EncryptedConnectionRemovalOutcome EspnowMeshBackend::requestEncryptedConnectionRemoval(const uint8_t *peerMac)
{
  using EspnowProtocolInterpreter::encryptedConnectionRemovalRequestHeader;
  
  assert(encryptedConnections.size() <= maxEncryptedConnections); // If this is not the case, ESP-NOW is no longer in sync with the library
  
  MutexTracker mutexTracker(_espnowTransmissionMutex, handlePostponedRemovals);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call requestEncryptedConnectionRemoval from callbacks as this may corrupt program state! Aborting."))); 
    return EncryptedConnectionRemovalOutcome::REMOVAL_REQUEST_FAILED;
  }

  if(EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerMac))
  {
    if(espnowSendToNode(FPSTR(encryptedConnectionRemovalRequestHeader), peerMac, 'P') == TransmissionStatusType::TRANSMISSION_COMPLETE)
    {
      return removeEncryptedConnectionUnprotected(peerMac);
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

void EspnowMeshBackend::setAcceptsUnverifiedRequests(const bool acceptsUnverifiedRequests)  { _acceptsUnverifiedRequests = acceptsUnverifiedRequests; }
bool EspnowMeshBackend::acceptsUnverifiedRequests() const { return _acceptsUnverifiedRequests; }

void EspnowMeshBackend::setEncryptedConnectionsSoftLimit(const uint8_t softLimit) 
{ 
  assert(softLimit <= 6); // Valid values are 0 to 6, but uint8_t is always at least 0.
  _encryptedConnectionsSoftLimit = softLimit; 
}

uint8_t EspnowMeshBackend::encryptedConnectionsSoftLimit() const { return _encryptedConnectionsSoftLimit; }

template <typename T>
typename T::iterator EspnowMeshBackend::getEncryptedConnectionIterator(const uint8_t *peerMac, T &connectionContainer)
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

EspnowMeshBackend::connectionLogIterator EspnowMeshBackend::connectionLogEndIterator()
{
  return encryptedConnections.end();
}

bool EspnowMeshBackend::getEncryptedConnectionIterator(const uint8_t *peerMac, connectionLogIterator &iterator)
{
  connectionLogIterator result = getEncryptedConnectionIterator(peerMac, encryptedConnections);

  if(result != connectionLogEndIterator())
  {
    iterator = result;
    return true;
  }
  
  return false;
}

bool EspnowMeshBackend::getTemporaryEncryptedConnectionIterator(const uint8_t *peerMac, connectionLogIterator &iterator)
{
  connectionLogIterator result = connectionLogEndIterator();
  
  if(getEncryptedConnectionIterator(peerMac, result) && result->temporary())
  {
    iterator = result;
    return true;
  }
  
  return false;
}

EncryptedConnectionLog *EspnowMeshBackend::getEncryptedConnection(const uint8_t *peerMac)
{
  auto connectionIterator = getEncryptedConnectionIterator(peerMac, encryptedConnections);
  if(connectionIterator != encryptedConnections.end())
  {
    return &(*connectionIterator);
  }
  
  return nullptr;
}

EncryptedConnectionLog *EspnowMeshBackend::getTemporaryEncryptedConnection(const uint8_t *peerMac)
{
  connectionLogIterator connectionIterator = connectionLogEndIterator();
  if(getTemporaryEncryptedConnectionIterator(peerMac, connectionIterator))
  {
    return &(*connectionIterator);
  }
  
  return nullptr;
}


uint8_t *EspnowMeshBackend::getEncryptedMac(const uint8_t *peerMac, uint8_t *resultArray)
{
  if(EncryptedConnectionLog *encryptedConnection = getEncryptedConnection(peerMac))
  {
    return encryptedConnection->getEncryptedPeerMac(resultArray);
  }
  
  return nullptr;
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

  if(verboseMode()) // Avoid string generation if not required
  {
    printAPInfo(recipientInfo);
    verboseModePrint(emptyString);
  }

  return initiateTransmissionKernel(message, targetBSSID);
}

TransmissionStatusType EspnowMeshBackend::initiateTransmissionKernel(const String &message, const uint8_t *targetBSSID)
{
  uint32_t transmissionStartTime = millis();
  TransmissionStatusType transmissionResult = sendRequest(message, targetBSSID);

  uint32_t transmissionDuration = millis() - transmissionStartTime;
  
  if(verboseMode() && transmissionResult == TransmissionStatusType::TRANSMISSION_COMPLETE) // Avoid calculations if not required
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
  if(verboseMode() && successfulTransmissions_AT > 0) // Avoid calculations if not required
  {
    verboseModePrint(String(F("Average duration of successful transmissions: ")) + String(totalDurationWhenSuccessful_AT/successfulTransmissions_AT) + String(F(" ms.")));
    verboseModePrint(String(F("Maximum duration of successful transmissions: ")) + String(maxTransmissionDuration_AT) + String(F(" ms.")));
  }
  else
  {
    verboseModePrint(String(F("No successful transmission.")));
  }
}

void EspnowMeshBackend::attemptTransmission(const String &message, const bool scan, const bool scanAllWiFiChannels)
{
  MutexTracker mutexTracker(_espnowTransmissionMutex, handlePostponedRemovals);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }

  prepareForTransmission(message, scan, scanAllWiFiChannels);

  MutexTracker connectionQueueMutexTracker(_espnowConnectionQueueMutex);
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
  MutexTracker mutexTracker(_espnowTransmissionMutex, handlePostponedRemovals);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return TransmissionStatusType::CONNECTION_FAILED;
  }

  return initiateTransmission(message, recipientInfo);
}

EncryptedConnectionStatus EspnowMeshBackend::initiateAutoEncryptingConnection(const EspnowNetworkInfo &recipientInfo, const bool requestPermanentConnection, uint8_t *targetBSSID, EncryptedConnectionLog **existingEncryptedConnection)
{
  assert(recipientInfo.BSSID() != nullptr); // We need at least the BSSID to connect
  recipientInfo.getBSSID(targetBSSID);

  if(verboseMode()) // Avoid string generation if not required
  {
    printAPInfo(recipientInfo);
    verboseModePrint(emptyString);
  }

  *existingEncryptedConnection = getEncryptedConnection(targetBSSID);
  EncryptedConnectionStatus connectionStatus = EncryptedConnectionStatus::MAX_CONNECTIONS_REACHED_SELF;

  if(requestPermanentConnection)
    connectionStatus = requestEncryptedConnection(targetBSSID);
  else
    connectionStatus = requestFlexibleTemporaryEncryptedConnection(targetBSSID, getAutoEncryptionDuration());

  return connectionStatus;
}

TransmissionStatusType EspnowMeshBackend::initiateAutoEncryptingTransmission(const String &message, uint8_t *targetBSSID, EncryptedConnectionStatus connectionStatus)
{
  TransmissionStatusType transmissionResult = TransmissionStatusType::CONNECTION_FAILED;
  
  if(encryptedConnectionEstablished(connectionStatus))
  {
    uint8_t encryptedMac[6] {0};
    assert(getEncryptedMac(targetBSSID, encryptedMac) && esp_now_is_peer_exist(encryptedMac) > 0 && String(F("ERROR! Attempting to send content marked as encrypted via unencrypted connection!")));
    transmissionResult = initiateTransmissionKernel(message, targetBSSID);
  }
  
  return transmissionResult;
}

void EspnowMeshBackend::finalizeAutoEncryptingConnection(const uint8_t *targetBSSID, const EncryptedConnectionLog *existingEncryptedConnection, const bool requestPermanentConnection)
{
  if(!existingEncryptedConnection && !requestPermanentConnection && !_reciprocalPeerRequestConfirmation)
  {
    // Remove any connection that was added during the transmission attempt and is no longer in use.
    removeEncryptedConnectionUnprotected(targetBSSID);
  }
}

void EspnowMeshBackend::attemptAutoEncryptingTransmission(const String &message, const bool requestPermanentConnections, const bool scan, const bool scanAllWiFiChannels)
{
  MutexTracker outerMutexTracker(_espnowTransmissionMutex, handlePostponedRemovals);
  if(!outerMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call attemptAutoEncryptingTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }

  prepareForTransmission(message, scan, scanAllWiFiChannels);

  outerMutexTracker.releaseMutex();

  MutexTracker connectionQueueMutexTracker(_espnowConnectionQueueMutex);
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
      EncryptedConnectionStatus connectionStatus = initiateAutoEncryptingConnection(currentNetwork, requestPermanentConnections, currentBSSID, &existingEncryptedConnection);
  
      MutexTracker innerMutexTracker = MutexTracker(_espnowTransmissionMutex);
      if(!innerMutexTracker.mutexCaptured())
      {
        assert(false && String(F("ERROR! Unable to recapture Mutex in attemptAutoEncryptingTransmission. Aborting."))); 
        return;
      }
  
      TransmissionStatusType transmissionResult = initiateAutoEncryptingTransmission(getMessage(), currentBSSID, connectionStatus);
  
      latestTransmissionOutcomes().push_back(TransmissionOutcome{.origin = currentNetwork, .transmissionStatus = transmissionResult});
  
      finalizeAutoEncryptingConnection(currentBSSID, existingEncryptedConnection, requestPermanentConnections);
  
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
  EncryptedConnectionStatus connectionStatus = initiateAutoEncryptingConnection(recipientInfo, requestPermanentConnection, targetBSSID, &existingEncryptedConnection);

  MutexTracker mutexTracker(_espnowTransmissionMutex, handlePostponedRemovals);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return TransmissionStatusType::CONNECTION_FAILED;
  }

  TransmissionStatusType transmissionResult = initiateAutoEncryptingTransmission(message, targetBSSID, connectionStatus);

  finalizeAutoEncryptingConnection(targetBSSID, existingEncryptedConnection, requestPermanentConnection);

  return transmissionResult;
}

void EspnowMeshBackend::broadcast(const String &message)
{  
  MutexTracker mutexTracker(_espnowTransmissionMutex, handlePostponedRemovals);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Transmission in progress. Don't call broadcast from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }

  espnowSendToNode(message, broadcastMac, 'B', this);
}

void EspnowMeshBackend::setBroadcastTransmissionRedundancy(const uint8_t redundancy) { _broadcastTransmissionRedundancy = redundancy; }
uint8_t EspnowMeshBackend::getBroadcastTransmissionRedundancy() const { return _broadcastTransmissionRedundancy; }

void EspnowMeshBackend::setResponseTransmittedHook(const responseTransmittedHookType responseTransmittedHook) { _responseTransmittedHook = responseTransmittedHook; }
EspnowMeshBackend::responseTransmittedHookType EspnowMeshBackend::getResponseTransmittedHook() const { return _responseTransmittedHook; }

void EspnowMeshBackend::sendStoredEspnowMessages(const ExpiringTimeTracker *estimatedMaxDurationTracker)
{
  sendPeerRequestConfirmations(estimatedMaxDurationTracker);

  if(estimatedMaxDurationTracker && estimatedMaxDurationTracker->expired())
    return;

  sendEspnowResponses(estimatedMaxDurationTracker);
}

void EspnowMeshBackend::sendPeerRequestConfirmations(const ExpiringTimeTracker *estimatedMaxDurationTracker)
{
  uint32_t bufferedCriticalHeapLevel = criticalHeapLevel() + criticalHeapLevelBuffer(); // We preferably want to start clearing the logs a bit before things get critical.
  // _ongoingPeerRequestNonce can change during every delay(), but we need to remember the initial value to know from where sendPeerRequestConfirmations was called.
  String initialOngoingPeerRequestNonce = _ongoingPeerRequestNonce;
  
  for(std::list<PeerRequestLog>::iterator confirmationsIterator = peerRequestConfirmationsToSend.begin(); confirmationsIterator != peerRequestConfirmationsToSend.end(); )
  {     
    using namespace EspnowProtocolInterpreter;

    // True if confirmationsIterator contains a peer request received from the same node we are currently sending a peer request to.
    bool reciprocalPeerRequest = !initialOngoingPeerRequestNonce.isEmpty() && confirmationsIterator->connectedTo(_ongoingPeerRequestMac);
         
    auto timeTrackerPointer = confirmationsIterator->temporary();
    assert(timeTrackerPointer); // peerRequestConfirmations should always expire and so should always have a timeTracker
    if(timeTrackerPointer->elapsedTime() > getEncryptionRequestTimeout() 
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

    EncryptedConnectionLog *existingEncryptedConnection = getEncryptedConnection(defaultBSSID);
    
    // If we receive a non-encrypted request for encrypted connection from a node that already exists as an encrypted peer for us we cannot send a response to the encrypted MAC 
    // since that transmission will then be encrypted and impossible for the request sender to read. Of course, removing the existing encrypted connection would also work,
    // but make it very simple for a third party to disrupt an encrypted connection by just sending random requests for encrypted connection.
    bool sendToDefaultBSSID = confirmationsIterator->requestEncrypted() || !existingEncryptedConnection;
    
    // Note that callbacks can be called during delay time, so it is possible to receive a transmission during espnowSendToNode
    // (which may add an element to the peerRequestConfirmationsToSend list).

    if(!existingEncryptedConnection && 
       ((reciprocalPeerRequest && encryptedConnections.size() >= maxEncryptedConnections) || (!reciprocalPeerRequest && reservedEncryptedConnections() >= maxEncryptedConnections)))
    {
      espnowSendToNodeUnsynchronized(JsonTranslator::createEncryptionRequestHmacMessage(FPSTR(maxConnectionsReachedHeader), 
                                                        confirmationsIterator->getPeerRequestNonce(), hashKey, hashKeyLength),
                                                        defaultBSSID, 'C', generateMessageID(nullptr)); // Generates a new message ID to avoid sending encrypted sessionKeys over unencrypted connections.
                                                        
      confirmationsIterator = peerRequestConfirmationsToSend.erase(confirmationsIterator);
    }
    else if(espnowSendToNodeUnsynchronized(JsonTranslator::createEncryptionRequestHmacMessage(FPSTR(basicConnectionInfoHeader),
                                                              confirmationsIterator->getPeerRequestNonce(), hashKey, hashKeyLength),
                                                              sendToDefaultBSSID ? defaultBSSID : unencryptedBSSID, 'C', generateMessageID(nullptr)) // Generates a new message ID to avoid sending encrypted sessionKeys over unencrypted connections.
                                                              == TransmissionStatusType::TRANSMISSION_COMPLETE)
    {            
      // Try to add encrypted connection. If connection added send confirmation with encryptedConnection->getOwnSessionKey() as session key and C type message (won't increment key). Then proceed with next request (no need to wait for answer).
      if(existingEncryptedConnection)
      {
        if(auto timeTrackerPointer = existingEncryptedConnection->temporary())
        {
          if(getEncryptionRequestTimeout() > timeTrackerPointer->remainingDuration())
          {
            existingEncryptedConnection->setRemainingDuration(getEncryptionRequestTimeout());
          }
        }
      }
      else if(EspnowMeshBackend *currentEspnowRequestManager = getEspnowRequestManager())
      {
        uint8_t staMacArray[6] = { 0 };
        uint8_t apMacArray[6] = { 0 };
        currentEspnowRequestManager->addTemporaryEncryptedConnection(confirmationsIterator->getPeerStaMac(staMacArray), confirmationsIterator->getPeerApMac(apMacArray),
                                                                     createSessionKey(), createSessionKey(), getEncryptionRequestTimeout());
        existingEncryptedConnection = getEncryptedConnection(defaultBSSID);
      }
      else
      {
        warningPrint(String(F("WARNING! Ignoring received encrypted connection request since no EspnowRequestManager is assigned.")));
      }
      
      if(!existingEncryptedConnection)
      {
        // Send "node full" message
        espnowSendToNodeUnsynchronized(JsonTranslator::createEncryptionRequestHmacMessage(FPSTR(maxConnectionsReachedHeader), 
                                                          confirmationsIterator->getPeerRequestNonce(), hashKey, hashKeyLength), 
                                                          defaultBSSID, 'C', generateMessageID(nullptr)); // Generates a new message ID to avoid sending encrypted sessionKeys over unencrypted connections.
      }
      else
      {
        if(reciprocalPeerRequest)
          _reciprocalPeerRequestConfirmation = true;
        
        delay(5); // Give some time for the peer to add an encrypted connection
        
        assert(esp_now_is_peer_exist(defaultBSSID) > 0 && String(F("ERROR! Attempting to send content marked as encrypted via unencrypted connection!")));

        String messageHeader;

        if(existingEncryptedConnection->temporary() && // Should never change permanent connections
          ((reciprocalPeerRequest && encryptedConnections.size() > confirmationsIterator->getEncryptedConnectionsSoftLimit()) 
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
        espnowSendToNodeUnsynchronized(JsonTranslator::createEncryptedConnectionInfo(messageHeader,
                                                          confirmationsIterator->getPeerRequestNonce(), confirmationsIterator->getAuthenticationPassword(), 
                                                          existingEncryptedConnection->getOwnSessionKey(), existingEncryptedConnection->getPeerSessionKey()),
                                                          defaultBSSID, 'C', generateMessageID(nullptr));  // Generates a new message ID to avoid sending encrypted sessionKeys over unencrypted connections.
      }
    
      confirmationsIterator = peerRequestConfirmationsToSend.erase(confirmationsIterator);
    }
    else
    {
      ++confirmationsIterator;
    }

    if(ESP.getFreeHeap() <= bufferedCriticalHeapLevel)
    {
      // Heap is getting very low, which probably means we are receiving a lot of transmissions while trying to transmit responses.
      // Clear all old data to try to avoid running out of memory.
      warningPrint("WARNING! Free heap below chosen minimum. Performing emergency log clearing.");
      clearOldLogEntries();
      return; // confirmationsIterator may be invalid now. Also, we should give the main loop a chance to respond to the situation.
    }

    if(estimatedMaxDurationTracker && estimatedMaxDurationTracker->expired())
      return;
  }
}

void EspnowMeshBackend::sendEspnowResponses(const ExpiringTimeTracker *estimatedMaxDurationTracker)
{
  uint32_t bufferedCriticalHeapLevel = criticalHeapLevel() + criticalHeapLevelBuffer(); // We preferably want to start clearing the logs a bit before things get critical.

  MutexTracker responsesToSendMutexTracker(_responsesToSendMutex);
  if(!responsesToSendMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! responsesToSend locked. Don't call sendEspnowResponses from callbacks as this may corrupt program state! Aborting."))); 
  }

  uint32_t responseIndex = 0;
  for(std::list<ResponseData>::iterator responseIterator = responsesToSend.begin(); responseIterator != responsesToSend.end(); ++responseIndex)
  {
    if(responseIterator->getTimeTracker().timeSinceCreation() > logEntryLifetimeMs())
    {
      // If the response is older than logEntryLifetimeMs(), the corresponding request log entry has been deleted at the request sender, 
      // so the request sender will not accept our response any more.
      // This probably happens because we have a high transmission activity and more requests coming in than we can handle.
      ++responseIterator;
      continue;
    }

    bool hookOutcome = true;
    // Note that callbacks can be called during delay time, so it is possible to receive a transmission during espnowSendToNode
    // (which may add an element to the responsesToSend list).
    if(espnowSendToNodeUnsynchronized(responseIterator->getMessage(), responseIterator->getRecipientMac(), 'A', responseIterator->getRequestID())
       == TransmissionStatusType::TRANSMISSION_COMPLETE)
    {
      if(EspnowMeshBackend *currentEspnowRequestManager = getEspnowRequestManager())
        hookOutcome = currentEspnowRequestManager->getResponseTransmittedHook()(responseIterator->getMessage(), responseIterator->getRecipientMac(), responseIndex, *currentEspnowRequestManager);
      
      responseIterator = responsesToSend.erase(responseIterator);
      --responseIndex;
    }
    else
    {
      ++responseIterator;
    }

    if(ESP.getFreeHeap() <= bufferedCriticalHeapLevel)
    {
      // Heap is getting very low, which probably means we are receiving a lot of transmissions while trying to transmit responses.
      // Clear all old data to try to avoid running out of memory.
      warningPrint("WARNING! Free heap below chosen minimum. Performing emergency log clearing.");
      clearOldLogEntries();
      return; // responseIterator may be invalid now. Also, we should give the main loop a chance to respond to the situation.
    }

    if(!hookOutcome || (estimatedMaxDurationTracker && estimatedMaxDurationTracker->expired()))
      return;
  }
}

uint32_t EspnowMeshBackend::getMaxBytesPerTransmission()
{
  return _maxBytesPerTransmission;
}

uint32_t EspnowMeshBackend::getMaxMessageBytesPerTransmission()
{
  using namespace EspnowProtocolInterpreter;
  return getMaxBytesPerTransmission() - metadataSize();
}

void EspnowMeshBackend::setMaxTransmissionsPerMessage(const uint8_t maxTransmissionsPerMessage)
{
  assert(1 <= maxTransmissionsPerMessage && maxTransmissionsPerMessage <= 128);
  
  _maxTransmissionsPerMessage = maxTransmissionsPerMessage;
}

uint8_t EspnowMeshBackend::getMaxTransmissionsPerMessage() {return _maxTransmissionsPerMessage;}

uint32_t EspnowMeshBackend::getMaxMessageLength()
{
  return getMaxTransmissionsPerMessage() * getMaxMessageBytesPerTransmission();
}

uint8_t EspnowMeshBackend::numberOfEncryptedConnections()
{
  return encryptedConnections.size();
}

uint8_t EspnowMeshBackend::reservedEncryptedConnections()
{
  if(!_ongoingPeerRequestNonce.isEmpty())
    if(!getEncryptedConnection(_ongoingPeerRequestMac))
      return encryptedConnections.size() + 1; // Reserve one connection spot if we are currently making a peer request to a new node.

  return encryptedConnections.size();
}

ConnectionType EspnowMeshBackend::getConnectionInfoHelper(const EncryptedConnectionLog *encryptedConnection, uint32_t *remainingDuration, uint8_t *peerMac)
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

ConnectionType EspnowMeshBackend::getConnectionInfo(uint8_t *peerMac, uint32_t *remainingDuration)
{
  EncryptedConnectionLog *encryptedConnection = nullptr;
  
  if(peerMac)
    encryptedConnection = getEncryptedConnection(peerMac);
  
  return getConnectionInfoHelper(encryptedConnection, remainingDuration);
}

ConnectionType EspnowMeshBackend::getConnectionInfo(const uint32_t connectionIndex, uint32_t *remainingDuration, uint8_t *peerMac)
{
  EncryptedConnectionLog *encryptedConnection = nullptr;

  if(connectionIndex < numberOfEncryptedConnections())
    encryptedConnection = &encryptedConnections[connectionIndex];

  return getConnectionInfoHelper(encryptedConnection, remainingDuration, peerMac);
}

double EspnowMeshBackend::getTransmissionFailRate()
{
  if(_transmissionsTotal == 0)
    return 0;
    
  return _transmissionsFailed/_transmissionsTotal;
}

void EspnowMeshBackend::resetTransmissionFailRate()
{
  _transmissionsFailed = 0;
  _transmissionsTotal = 0;
}

String EspnowMeshBackend::serializeUnencryptedConnection()
{
  using namespace JsonTranslator;
  
  // Returns: {"connectionState":{"unsyncMsgID":"123"}}
  
  return String(FPSTR(jsonConnectionState)) + createJsonEndPair(FPSTR(jsonUnsynchronizedMessageID), String(_unsynchronizedMessageID));
}

String EspnowMeshBackend::serializeEncryptedConnection(const uint8_t *peerMac)
{
  String serializedConnection(emptyString);
  
  EncryptedConnectionLog *encryptedConnection = nullptr;
  
  if(peerMac)
    encryptedConnection = getEncryptedConnection(peerMac);
  
  if(encryptedConnection)
    serializedConnection = encryptedConnection->serialize();

  return serializedConnection;
}

String EspnowMeshBackend::serializeEncryptedConnection(const uint32_t connectionIndex)
{
  String serializedConnection(emptyString);
  
  if(connectionIndex < numberOfEncryptedConnections())
    serializedConnection = encryptedConnections[connectionIndex].serialize();
  
  return serializedConnection;
}
