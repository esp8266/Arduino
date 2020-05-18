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

#include "EspnowTransmitter.h"
#include "EspnowMeshBackend.h"
#include "TypeConversionFunctions.h"
#include "UtilityFunctions.h"
#include "MeshCryptoInterface.h"
#include "JsonTranslator.h"

namespace
{
  namespace TypeCast = MeshTypeConversionFunctions;
  
  double _transmissionsTotal = 0;
  double _transmissionsFailed = 0;
    
  std::shared_ptr<bool> _espnowTransmissionMutex = std::make_shared<bool>(false);
  std::shared_ptr<bool> _espnowSendToNodeMutex = std::make_shared<bool>(false);
  
  uint32_t _espnowTransmissionTimeoutMs = 40;
  uint32_t _espnowRetransmissionIntervalMs = 15;
  
  uint8_t _espnowMessageEncryptionKey[experimental::crypto::ENCRYPTION_KEY_LENGTH] = { 0 };
  bool _useEncryptedMessages = false;
  
  uint8_t _transmissionTargetBSSID[6] = {0};
  
  bool _espnowSendConfirmed = false;

  uint8_t _maxTransmissionsPerMessage = 3;
}

EspnowTransmitter::EspnowTransmitter(ConditionalPrinter &conditionalPrinterInstance, EspnowDatabase &databaseInstance, EspnowConnectionManager &connectionManagerInstance) 
                                      : _conditionalPrinter(conditionalPrinterInstance), _database(databaseInstance), _connectionManager(connectionManagerInstance)
{
}

void EspnowTransmitter::espnowSendCallback(uint8_t* mac, uint8_t sendStatus)
{
  if(_espnowSendConfirmed)
    return;
  else if(!sendStatus && MeshUtilityFunctions::macEqual(mac, _transmissionTargetBSSID)) // sendStatus == 0 when send was OK.
    _espnowSendConfirmed = true; // We do not want to reset this to false. That only happens before transmissions. Otherwise subsequent failed send attempts may obscure an initial successful one.
}

void EspnowTransmitter::setUseEncryptedMessages(const bool useEncryptedMessages) 
{
  MutexTracker mutexTracker(_espnowSendToNodeMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! espnowSendToNode in progress. Don't call setUseEncryptedMessages from non-hook callbacks since this may modify the ESP-NOW transmission parameters during ongoing transmissions! Aborting.")));
  }
  
  _useEncryptedMessages = useEncryptedMessages; 
}
bool EspnowTransmitter::useEncryptedMessages() { return _useEncryptedMessages; }

void EspnowTransmitter::setEspnowMessageEncryptionKey(const uint8_t espnowMessageEncryptionKey[experimental::crypto::ENCRYPTION_KEY_LENGTH])
{
  assert(espnowMessageEncryptionKey != nullptr);
   
  for(int i = 0; i < experimental::crypto::ENCRYPTION_KEY_LENGTH; ++i)
  {
    _espnowMessageEncryptionKey[i] = espnowMessageEncryptionKey[i];
  }
}

void EspnowTransmitter::setEspnowMessageEncryptionKey(const String &espnowMessageEncryptionKeySeed)
{
  MeshCryptoInterface::initializeKey(_espnowMessageEncryptionKey, experimental::crypto::ENCRYPTION_KEY_LENGTH, espnowMessageEncryptionKeySeed);
}

const uint8_t *EspnowTransmitter::getEspnowMessageEncryptionKey()
{
  return _espnowMessageEncryptionKey;
}

void EspnowTransmitter::setBroadcastTransmissionRedundancy(const uint8_t redundancy) { _broadcastTransmissionRedundancy = redundancy; }
uint8_t EspnowTransmitter::getBroadcastTransmissionRedundancy() const { return _broadcastTransmissionRedundancy; }

void EspnowTransmitter::setResponseTransmittedHook(const responseTransmittedHookType responseTransmittedHook) { _responseTransmittedHook = responseTransmittedHook; }
EspnowTransmitter::responseTransmittedHookType EspnowTransmitter::getResponseTransmittedHook() const { return _responseTransmittedHook; }

void EspnowTransmitter::setMaxTransmissionsPerMessage(const uint8_t maxTransmissionsPerMessage)
{
  assert(1 <= maxTransmissionsPerMessage && maxTransmissionsPerMessage <= 128);
  
  _maxTransmissionsPerMessage = maxTransmissionsPerMessage;
}

uint8_t EspnowTransmitter::getMaxTransmissionsPerMessage() {return _maxTransmissionsPerMessage;}

uint32_t EspnowTransmitter::getMaxMessageLength()
{
  return getMaxTransmissionsPerMessage() * EspnowProtocolInterpreter::getMaxMessageBytesPerTransmission();
}

void EspnowTransmitter::setEspnowTransmissionTimeout(const uint32_t timeoutMs)
{
  _espnowTransmissionTimeoutMs = timeoutMs;
}
uint32_t EspnowTransmitter::getEspnowTransmissionTimeout() {return _espnowTransmissionTimeoutMs;}

void EspnowTransmitter::setEspnowRetransmissionInterval(const uint32_t intervalMs)
{
  _espnowRetransmissionIntervalMs = intervalMs;
}
uint32_t EspnowTransmitter::getEspnowRetransmissionInterval() {return _espnowRetransmissionIntervalMs;}

double EspnowTransmitter::getTransmissionFailRate()
{
  if(_transmissionsTotal == 0)
    return 0;
    
  return _transmissionsFailed/_transmissionsTotal;
}

void EspnowTransmitter::resetTransmissionFailRate()
{
  _transmissionsFailed = 0;
  _transmissionsTotal = 0;
}

void EspnowTransmitter::sendEspnowResponses(const ExpiringTimeTracker *estimatedMaxDurationTracker)
{
  uint32_t bufferedCriticalHeapLevel = EspnowDatabase::criticalHeapLevel() + EspnowDatabase::criticalHeapLevelBuffer(); // We preferably want to start clearing the logs a bit before things get critical.

  MutexTracker responsesToSendMutexTracker(EspnowDatabase::captureResponsesToSendMutex());
  if(!responsesToSendMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! responsesToSend locked. Don't call sendEspnowResponses from callbacks as this may corrupt program state! Aborting."))); 
  }

  uint32_t responseIndex = 0;
  for(std::list<ResponseData>::iterator responseIterator = EspnowDatabase::responsesToSend().begin(); responseIterator != EspnowDatabase::responsesToSend().end(); ++responseIndex)
  {
    if(responseIterator->getTimeTracker().timeSinceCreation() > EspnowDatabase::logEntryLifetimeMs())
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
      if(EspnowMeshBackend *currentEspnowRequestManager = EspnowMeshBackend::getEspnowRequestManager())
        hookOutcome = currentEspnowRequestManager->getResponseTransmittedHook()(responseIterator->getMessage(), responseIterator->getRecipientMac(), responseIndex, *currentEspnowRequestManager);
      
      responseIterator = EspnowDatabase::responsesToSend().erase(responseIterator);
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
      ConditionalPrinter::warningPrint("WARNING! Free heap below chosen minimum. Performing emergency log clearing.");
      EspnowDatabase::clearOldLogEntries(true);
      return; // responseIterator may be invalid now. Also, we should give the main loop a chance to respond to the situation.
    }

    if(!hookOutcome || (estimatedMaxDurationTracker && estimatedMaxDurationTracker->expired()))
      return;
  }
}

MutexTracker EspnowTransmitter::captureEspnowTransmissionMutex()
{
  // Syntax like this will move the resulting value into its new position (similar to NRVO): https://stackoverflow.com/a/11540204
  return MutexTracker(_espnowTransmissionMutex);
}

MutexTracker EspnowTransmitter::captureEspnowTransmissionMutex(const std::function<void()> destructorHook) { return MutexTracker(_espnowTransmissionMutex, destructorHook); }

bool EspnowTransmitter::transmissionInProgress(){return *_espnowTransmissionMutex;}

TransmissionStatusType EspnowTransmitter::espnowSendToNode(const String &message, const uint8_t *targetBSSID, const char messageType, EspnowMeshBackend *espnowInstance)
{
  using EspnowProtocolInterpreter::synchronizationRequestHeader;
  
  EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(targetBSSID);
  
  if(encryptedConnection)
  {
    uint8_t encryptedMac[6] {0};
    encryptedConnection->getEncryptedPeerMac(encryptedMac);
    
    assert(esp_now_is_peer_exist(encryptedMac) > 0 && String(F("ERROR! Attempting to send content marked as encrypted via unencrypted connection!")));
    
    if(encryptedConnection->desync())
    {
      espnowSendToNodeUnsynchronized(FPSTR(synchronizationRequestHeader), encryptedMac, 'S', EspnowConnectionManager::generateMessageID(encryptedConnection), espnowInstance);
  
      if(encryptedConnection->desync())
      {
        return TransmissionStatusType::TRANSMISSION_FAILED;
      }
    }

    return espnowSendToNodeUnsynchronized(message, encryptedMac, messageType, EspnowConnectionManager::generateMessageID(encryptedConnection), espnowInstance);
  }
  
  return espnowSendToNodeUnsynchronized(message, targetBSSID, messageType, EspnowConnectionManager::generateMessageID(encryptedConnection), espnowInstance);
}

TransmissionStatusType EspnowTransmitter::espnowSendToNodeUnsynchronized(const String message, const uint8_t *targetBSSID, const char messageType, const uint64_t messageID, EspnowMeshBackend *espnowInstance)
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
  
  EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(_transmissionTargetBSSID);
  
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
      EspnowDatabase::storeSentRequest(TypeCast::macToUint64(_transmissionTargetBSSID), messageID, RequestData(*espnowInstance));
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
      experimental::crypto::ChaCha20Poly1305::encrypt(transmission + espnowMetadataSize, transmissionSize - espnowMetadataSize, getEspnowMessageEncryptionKey(), transmission, 
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

      ConditionalPrinter::staticVerboseModePrint(String(F("espnowSendToNode failed!")));
      ConditionalPrinter::staticVerboseModePrint(String(F("Transmission #: ")) + String(transmissionsRequired - transmissionsRemaining) + String('/') + String(transmissionsRequired));
      ConditionalPrinter::staticVerboseModePrint(String(F("Transmission fail rate (up) ")) + String(getTransmissionFailRate()));

      if(messageStart && encryptedConnection && !usesConstantSessionKey(messageType) && encryptedConnection->getOwnSessionKey() == messageID)
        encryptedConnection->setDesync(true);
      
      return TransmissionStatusType::TRANSMISSION_FAILED;
    }

    --transmissionsRemaining; // This is used when transfering multi-transmission messages.
    
  } while(transmissionsRemaining >= 0);

  // Useful when debugging the protocol
  //_conditionalPrinter.staticVerboseModePrint("Sent to Mac: " + TypeCast::macToString(_transmissionTargetBSSID) + " ID: " + TypeCast::uint64ToString(messageID)); 
  
  return TransmissionStatusType::TRANSMISSION_COMPLETE;
}

TransmissionStatusType EspnowTransmitter::espnowSendPeerRequestConfirmationsUnsynchronized(const String message, const uint8_t *targetBSSID, const char messageType, EspnowMeshBackend *espnowInstance)
{
  return espnowSendToNodeUnsynchronized(message, targetBSSID, messageType, EspnowConnectionManager::generateMessageID(nullptr), espnowInstance);
}

TransmissionStatusType EspnowTransmitter::sendRequest(const String &message, const uint8_t *targetBSSID, EspnowMeshBackend *espnowInstance)
{
  TransmissionStatusType transmissionStatus = espnowSendToNode(message, targetBSSID, 'Q', espnowInstance);

  return transmissionStatus;
}

TransmissionStatusType EspnowTransmitter::sendResponse(const String &message, const uint64_t requestID, const uint8_t *targetBSSID, EspnowMeshBackend *espnowInstance)
{
  EncryptedConnectionLog *encryptedConnection = EspnowConnectionManager::getEncryptedConnection(targetBSSID);
  uint8_t encryptedMac[6] {0};
    
  if(encryptedConnection)
  {
    encryptedConnection->getEncryptedPeerMac(encryptedMac);
    assert(esp_now_is_peer_exist(encryptedMac) > 0 && String(F("ERROR! Attempting to send content marked as encrypted via unencrypted connection!")));
  }
  
  return espnowSendToNodeUnsynchronized(message, encryptedConnection ? encryptedMac : targetBSSID, 'A', requestID, espnowInstance);
}
