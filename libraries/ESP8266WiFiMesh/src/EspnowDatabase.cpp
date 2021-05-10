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

#include "EspnowDatabase.h"
#include "EspnowMeshBackend.h"
#include "UtilityFunctions.h"

namespace
{
  namespace TypeCast = MeshTypeConversionFunctions;

  // _logEntryLifetimeMs is based on someone storing 40 responses of 750 bytes each = 30 000 bytes (roughly full memory), 
  // which takes 2000 ms + some margin to send. Also, we want to avoid old entries taking up memory if they cannot be sent, 
  // so storage duration should not be too long.
  uint32_t _logEntryLifetimeMs = 2500;
  uint32_t _broadcastResponseTimeoutMs = 1000; // This is shorter than _logEntryLifetimeMs to preserve RAM since broadcasts are not deleted from sentRequests until they expire.
  ExpiringTimeTracker _logClearingCooldown(500);

  uint32_t _encryptionRequestTimeoutMs = 300;

  uint32_t _criticalHeapLevel = 6000; // In bytes
  uint32_t _criticalHeapLevelBuffer = 6000; // In bytes

  using EspnowProtocolInterpreter::macAndType_td;
  using EspnowProtocolInterpreter::messageID_td;
  using EspnowProtocolInterpreter::peerMac_td;

  std::list<ResponseData> _responsesToSend = {};
  std::list<PeerRequestLog> _peerRequestConfirmationsToSend = {};

  std::map<std::pair<macAndType_td, messageID_td>, MessageData> _receivedEspnowTransmissions = {};
  std::map<std::pair<peerMac_td, messageID_td>, RequestData> _sentRequests = {};
  std::map<std::pair<peerMac_td, messageID_td>, TimeTracker> _receivedRequests = {};

  std::shared_ptr<bool> _espnowConnectionQueueMutex = std::make_shared<bool>(false);
  std::shared_ptr<bool> _responsesToSendMutex = std::make_shared<bool>(false);
}

std::vector<EspnowNetworkInfo> EspnowDatabase::_connectionQueue = {};
std::vector<TransmissionOutcome> EspnowDatabase::_latestTransmissionOutcomes = {};

EspnowDatabase::EspnowDatabase(ConditionalPrinter &conditionalPrinterInstance, const uint8 espnowWiFiChannel) : _conditionalPrinter(conditionalPrinterInstance), _espnowWiFiChannel(espnowWiFiChannel)
{
}

std::vector<EspnowNetworkInfo> & EspnowDatabase::connectionQueue()
{
  MutexTracker connectionQueueMutexTracker(_espnowConnectionQueueMutex);
  if(!connectionQueueMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! connectionQueue locked. Don't call connectionQueue() from callbacks other than NetworkFilter as this may corrupt program state!"))); 
  }
  
  return _connectionQueue;
}

const std::vector<EspnowNetworkInfo> & EspnowDatabase::constConnectionQueue()
{  
  return _connectionQueue;
}

std::vector<TransmissionOutcome> & EspnowDatabase::latestTransmissionOutcomes()
{
  return _latestTransmissionOutcomes;
}

void EspnowDatabase::setCriticalHeapLevelBuffer(const uint32_t bufferInBytes)
{
  _criticalHeapLevelBuffer = bufferInBytes;
}

uint32_t EspnowDatabase::criticalHeapLevelBuffer()
{
  return _criticalHeapLevelBuffer;
}

uint32_t EspnowDatabase::criticalHeapLevel()
{
  return _criticalHeapLevel;
}

template <typename T, typename U>
void EspnowDatabase::deleteExpiredLogEntries(std::map<std::pair<U, uint64_t>, T> &logEntries, const uint32_t maxEntryLifetimeMs)
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
void EspnowDatabase::deleteExpiredLogEntries(std::map<std::pair<U, uint64_t>, TimeTracker> &logEntries, const uint32_t maxEntryLifetimeMs)
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

void EspnowDatabase::deleteExpiredLogEntries(std::map<std::pair<peerMac_td, messageID_td>, RequestData> &logEntries, const uint32_t requestLifetimeMs, const uint32_t broadcastLifetimeMs)
{
  for(typename std::map<std::pair<peerMac_td, messageID_td>, RequestData>::iterator entryIterator = logEntries.begin(); 
      entryIterator != logEntries.end(); )
  {
    bool broadcast = entryIterator->first.first == EspnowProtocolInterpreter::uint64BroadcastMac;
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
void EspnowDatabase::deleteExpiredLogEntries(std::list<T> &logEntries, const uint32_t maxEntryLifetimeMs)
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
void EspnowDatabase::deleteExpiredLogEntries(std::list<PeerRequestLog> &logEntries, const uint32_t maxEntryLifetimeMs)
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

void EspnowDatabase::setLogEntryLifetimeMs(const uint32_t logEntryLifetimeMs)
{
  _logEntryLifetimeMs = logEntryLifetimeMs;
}
uint32_t EspnowDatabase::logEntryLifetimeMs() { return _logEntryLifetimeMs; }

void EspnowDatabase::setBroadcastResponseTimeoutMs(const uint32_t broadcastResponseTimeoutMs)
{
  _broadcastResponseTimeoutMs = broadcastResponseTimeoutMs;
}
uint32_t EspnowDatabase::broadcastResponseTimeoutMs() { return _broadcastResponseTimeoutMs; }

String EspnowDatabase::getScheduledResponseMessage(const uint32_t responseIndex)
{
  return getScheduledResponse(responseIndex)->getMessage();
}

const uint8_t *EspnowDatabase::getScheduledResponseRecipient(const uint32_t responseIndex)
{
  return getScheduledResponse(responseIndex)->getRecipientMac();
}

uint32_t EspnowDatabase::numberOfScheduledResponses() {return responsesToSend().size();}

void EspnowDatabase::clearAllScheduledResponses()
{
  MutexTracker responsesToSendMutexTracker(_responsesToSendMutex);
  if(!responsesToSendMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! responsesToSend locked. Don't call clearAllScheduledResponses from callbacks as this may corrupt program state! Aborting."))); 
  }
  
  responsesToSend().clear();
}

void EspnowDatabase::deleteScheduledResponsesByRecipient(const uint8_t *recipientMac, const bool encryptedOnly)
{
  MutexTracker responsesToSendMutexTracker(_responsesToSendMutex);
  if(!responsesToSendMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! responsesToSend locked. Don't call deleteScheduledResponsesByRecipient from callbacks as this may corrupt program state! Aborting."))); 
  }
  
  for(auto responseIterator = responsesToSend().begin(); responseIterator != responsesToSend().end(); )
  {
    if(MeshUtilityFunctions::macEqual(responseIterator->getRecipientMac(), recipientMac) && 
       (!encryptedOnly || EspnowProtocolInterpreter::usesEncryption(responseIterator->getRequestID())))
    {
      responseIterator = responsesToSend().erase(responseIterator);
    }
    else
      ++responseIterator;
  }
}

void EspnowDatabase::setEncryptionRequestTimeout(const uint32_t timeoutMs)
{
  _encryptionRequestTimeoutMs = timeoutMs;
}
uint32_t EspnowDatabase::getEncryptionRequestTimeout() {return _encryptionRequestTimeoutMs;}

void EspnowDatabase::setAutoEncryptionDuration(const uint32_t duration)
{
  _autoEncryptionDuration = duration;
}
uint32_t EspnowDatabase::getAutoEncryptionDuration() const {return _autoEncryptionDuration;}

String EspnowDatabase::getSenderMac() const {return TypeCast::macToString(_senderMac);}
uint8_t *EspnowDatabase::getSenderMac(uint8_t *macArray) const
{
  std::copy_n(_senderMac, 6, macArray);
  return macArray;
}

String EspnowDatabase::getSenderAPMac() const {return TypeCast::macToString(_senderAPMac);}
uint8_t *EspnowDatabase::getSenderAPMac(uint8_t *macArray) const
{
  std::copy_n(_senderAPMac, 6, macArray);
  return macArray;
}

void EspnowDatabase::clearOldLogEntries(bool forced)
{
  // Clearing all old log entries at the same time should help minimize heap fragmentation.
  
  // uint32_t startTime = millis();

  if(!forced && !_logClearingCooldown) // Clearing too frequently will cause a lot of unnecessary container iterations.
  {
    return;
  }
  
  _logClearingCooldown.reset();
  
  deleteExpiredLogEntries(receivedEspnowTransmissions(), logEntryLifetimeMs());
  deleteExpiredLogEntries(receivedRequests(), logEntryLifetimeMs()); // Just needs to be long enough to not accept repeated transmissions by mistake.
  deleteExpiredLogEntries(sentRequests(), logEntryLifetimeMs(), broadcastResponseTimeoutMs());
  deleteExpiredLogEntries(responsesToSend(), logEntryLifetimeMs());
  deleteExpiredLogEntries(peerRequestConfirmationsToSend(), getEncryptionRequestTimeout());
}

std::list<ResponseData>::const_iterator EspnowDatabase::getScheduledResponse(const uint32_t responseIndex)
{
  assert(responseIndex < numberOfScheduledResponses());

  bool startFromBeginning = responseIndex < numberOfScheduledResponses()/2;
  auto responseIterator = startFromBeginning ? responsesToSend().cbegin() : responsesToSend().cend();
  uint32_t stepsToTarget = startFromBeginning ? responseIndex : numberOfScheduledResponses() - responseIndex; // cend is one element beyond the last

  while(stepsToTarget > 0)
  {
    startFromBeginning ? ++responseIterator : --responseIterator;
    --stepsToTarget;
  }

  return responseIterator;
}

void EspnowDatabase::setSenderMac(const uint8_t *macArray)
{
  std::copy_n(macArray, 6, _senderMac);
}

void EspnowDatabase::setSenderAPMac(const uint8_t *macArray)
{
  std::copy_n(macArray, 6, _senderAPMac);
}

void EspnowDatabase::setWiFiChannel(const uint8 newWiFiChannel)
{
  wifi_country_t wifiCountry;
  wifi_get_country(&wifiCountry); // Note: Should return 0 on success and -1 on failure, but always seems to return 1. Possibly broken API. Channels 1 to 13 are the default limits.
  assert(wifiCountry.schan <= newWiFiChannel && newWiFiChannel <= wifiCountry.schan + wifiCountry.nchan - 1);
  
  _espnowWiFiChannel = newWiFiChannel;
}

uint8 EspnowDatabase::getWiFiChannel() const
{
  return _espnowWiFiChannel;
}

bool EspnowDatabase::requestReceived(const uint64_t requestMac, const uint64_t requestID)
{
  return receivedRequests().count(std::make_pair(requestMac, requestID));
}

MutexTracker EspnowDatabase::captureEspnowConnectionQueueMutex() 
{   
  // Syntax like this will move the resulting value into its new position (similar to NRVO): https://stackoverflow.com/a/11540204
  return MutexTracker(_espnowConnectionQueueMutex); 
}

MutexTracker EspnowDatabase::captureEspnowConnectionQueueMutex(const std::function<void()> destructorHook) { return MutexTracker(_espnowConnectionQueueMutex, destructorHook); }

MutexTracker EspnowDatabase::captureResponsesToSendMutex(){ return MutexTracker(_responsesToSendMutex); }

MutexTracker EspnowDatabase::captureResponsesToSendMutex(const std::function<void()> destructorHook) { return MutexTracker(_responsesToSendMutex, destructorHook); }

void EspnowDatabase::storeSentRequest(const uint64_t targetBSSID, const uint64_t messageID, const RequestData &requestData)
{
  sentRequests().insert(std::make_pair(std::make_pair(targetBSSID, messageID), requestData));
}

void EspnowDatabase::storeReceivedRequest(const uint64_t senderBSSID, const uint64_t messageID, const TimeTracker &timeTracker)
{
  receivedRequests().insert(std::make_pair(std::make_pair(senderBSSID, messageID), timeTracker));
}

EspnowMeshBackend *EspnowDatabase::getOwnerOfSentRequest(const uint64_t requestMac, const uint64_t requestID)
{
  std::map<std::pair<peerMac_td, messageID_td>, RequestData>::iterator sentRequest = sentRequests().find(std::make_pair(requestMac, requestID));
  
  if(sentRequest != sentRequests().end())
  {
    return &sentRequest->second.getMeshInstance();
  }
  
  return nullptr;
}

size_t EspnowDatabase::deleteSentRequest(const uint64_t requestMac, const uint64_t requestID)
{
  return sentRequests().erase(std::make_pair(requestMac, requestID));
}

size_t EspnowDatabase::deleteSentRequestsByOwner(const EspnowMeshBackend *instancePointer)
{
  size_t numberDeleted = 0;
  
  for(std::map<std::pair<peerMac_td, messageID_td>, RequestData>::iterator requestIterator = sentRequests().begin(); 
      requestIterator != sentRequests().end(); )
  {
    if(&requestIterator->second.getMeshInstance() == instancePointer) // If instance at instancePointer made the request
    {
      requestIterator = sentRequests().erase(requestIterator);
      numberDeleted++;
    }
    else
      ++requestIterator;
  }

  return numberDeleted;
}

std::list<ResponseData> & EspnowDatabase::responsesToSend() { return _responsesToSend; }
std::list<PeerRequestLog> & EspnowDatabase::peerRequestConfirmationsToSend() { return _peerRequestConfirmationsToSend; }
std::map<std::pair<macAndType_td, messageID_td>, MessageData> & EspnowDatabase::receivedEspnowTransmissions() { return _receivedEspnowTransmissions; }
std::map<std::pair<peerMac_td, messageID_td>, RequestData> & EspnowDatabase::sentRequests() { return _sentRequests; }
std::map<std::pair<peerMac_td, messageID_td>, TimeTracker> & EspnowDatabase::receivedRequests() { return _receivedRequests; }
