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

#ifndef __ESPNOWDATABASE_H__
#define __ESPNOWDATABASE_H__

#include <Arduino.h>
#include "EspnowNetworkInfo.h"
#include "TransmissionOutcome.h"
#include "ResponseData.h"
#include "RequestData.h"
#include "EspnowProtocolInterpreter.h"
#include <list>
#include <map>
#include "MessageData.h"
#include "MutexTracker.h"
#include "PeerRequestLog.h"
#include "ConditionalPrinter.h"
#include "TypeConversionFunctions.h"

class EspnowMeshBackend;

class EspnowDatabase
{

public:

  EspnowDatabase(ConditionalPrinter &conditionalPrinterInstance, const uint8 espnowWiFiChannel);

  static std::vector<EspnowNetworkInfo> & connectionQueue();
  static const std::vector<EspnowNetworkInfo> & constConnectionQueue();
  static std::vector<TransmissionOutcome> & latestTransmissionOutcomes();
  static uint32_t criticalHeapLevel();
  static void setCriticalHeapLevelBuffer(const uint32_t bufferInBytes);
  static uint32_t criticalHeapLevelBuffer();
  static void setLogEntryLifetimeMs(const uint32_t logEntryLifetimeMs);
  static uint32_t logEntryLifetimeMs();
  static void setBroadcastResponseTimeoutMs(const uint32_t broadcastResponseTimeoutMs);
  static uint32_t broadcastResponseTimeoutMs();
  static String getScheduledResponseMessage(const uint32_t responseIndex);
  static const uint8_t *getScheduledResponseRecipient(const uint32_t responseIndex);
  static uint32_t numberOfScheduledResponses();
  static void clearAllScheduledResponses();
  static void deleteScheduledResponsesByRecipient(const uint8_t *recipientMac, const bool encryptedOnly);
  static void setEncryptionRequestTimeout(const uint32_t timeoutMs);
  static uint32_t getEncryptionRequestTimeout();
  
  void setAutoEncryptionDuration(const uint32_t duration);
  uint32_t getAutoEncryptionDuration() const;
  String getSenderMac() const;
  uint8_t *getSenderMac(uint8_t *macArray) const;
  String getSenderAPMac() const;
  uint8_t *getSenderAPMac(uint8_t *macArray) const;

  using macAndType_td = EspnowProtocolInterpreter::macAndType_td;
  using messageID_td = EspnowProtocolInterpreter::messageID_td;
  using peerMac_td = EspnowProtocolInterpreter::peerMac_td;

  static size_t deleteSentRequestsByOwner(const EspnowMeshBackend *instancePointer);
  static std::list<ResponseData> & responsesToSend();
  static std::list<PeerRequestLog> & peerRequestConfirmationsToSend();
  static std::map<std::pair<macAndType_td, messageID_td>, MessageData> & receivedEspnowTransmissions();
  static std::map<std::pair<peerMac_td, messageID_td>, RequestData> & sentRequests();
  static std::map<std::pair<peerMac_td, messageID_td>, TimeTracker> & receivedRequests();
  
  static bool requestReceived(const uint64_t requestMac, const uint64_t requestID);

  /** 
   * Will be captured when the connectionQueue should not be modified.
   */
  static MutexTracker captureEspnowConnectionQueueMutex();
  static MutexTracker captureEspnowConnectionQueueMutex(const std::function<void()> destructorHook);

  /** 
   * Will be captured when no responsesToSend element should be removed.
   */
  static MutexTracker captureResponsesToSendMutex();
  static MutexTracker captureResponsesToSendMutex(const std::function<void()> destructorHook);
  
  static void clearOldLogEntries(bool forced);

  static void storeSentRequest(const uint64_t targetBSSID, const uint64_t messageID, const RequestData &requestData);
  static void storeReceivedRequest(const uint64_t senderBSSID, const uint64_t messageID, const TimeTracker &timeTracker);
  
  /**
  * Get a pointer to the EspnowMeshBackend instance that sent a request with the given requestID to the specified mac address.
  * 
  * @return A valid EspnowMeshBackend pointer if a matching entry is found in the EspnowMeshBackend sentRequests container. nullptr otherwise.
  */
  static EspnowMeshBackend *getOwnerOfSentRequest(const uint64_t requestMac, const uint64_t requestID);
  
  /**
  * Delete all entries in the sentRequests container where requestMac is noted as having received requestID.
  * 
  * @return The number of entries deleted.
  */
  static size_t deleteSentRequest(const uint64_t requestMac, const uint64_t requestID);
  
  /**
   * Set the MAC address considered to be the sender of the most recently received ESP-NOW request, response or broadcast.
   * 
   * @param macArray An uint8_t array which contains the MAC address to store. The method will store the first 6 bytes of the array.
   */
  void setSenderMac(const uint8_t *macArray);

  /**
   * Set the MAC address considered to be the AP MAC of the sender of the most recently received ESP-NOW request, response or broadcast.
   * 
   * @param macArray An uint8_t array which contains the MAC address to store. The method will store the first 6 bytes of the array.
   */
  void setSenderAPMac(const uint8_t *macArray);

  void setWiFiChannel(const uint8 newWiFiChannel);
  uint8 getWiFiChannel() const;
  
  /**
   * Remove all entries which target peerMac in the logEntries map.
   * Optionally deletes only entries sent/received by encrypted transmissions.
   * 
   * @param logEntries The map to process.
   * @param peerMac The MAC address of the peer node.
   * @param encryptedOnly If true, only entries sent/received by encrypted transmissions will be deleted.
   */
  template <typename T>
  static void deleteEntriesByMac(std::map<std::pair<macAndType_td, uint64_t>, T> &logEntries, const uint8_t *peerMac, const bool encryptedOnly)
  {    
    bool macFound = false;
    
    for(typename std::map<std::pair<macAndType_td, uint64_t>, T>::iterator entryIterator = logEntries.begin(); 
        entryIterator != logEntries.end(); )
    {
      if(macAndTypeToUint64Mac(entryIterator->first.first) == MeshTypeConversionFunctions::macToUint64(peerMac))
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
  static void deleteEntriesByMac(std::map<std::pair<uint64_t, uint64_t>, T> &logEntries, const uint8_t *peerMac, const bool encryptedOnly)
  {    
    bool macFound = false;
    
    for(typename std::map<std::pair<uint64_t, uint64_t>, T>::iterator entryIterator = logEntries.begin(); 
        entryIterator != logEntries.end(); )
    {
      if(entryIterator->first.first == MeshTypeConversionFunctions::macToUint64(peerMac))
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

protected:

  static std::vector<EspnowNetworkInfo> _connectionQueue;
  static std::vector<TransmissionOutcome> _latestTransmissionOutcomes;

  static std::list<ResponseData>::const_iterator getScheduledResponse(const uint32_t responseIndex);

private:

  ConditionalPrinter & _conditionalPrinter;

  uint32_t _autoEncryptionDuration = 50;
  
  template <typename T, typename U>
  static void deleteExpiredLogEntries(std::map<std::pair<U, uint64_t>, T> &logEntries, const uint32_t maxEntryLifetimeMs);

  template <typename U>
  static void deleteExpiredLogEntries(std::map<std::pair<U, uint64_t>, TimeTracker> &logEntries, const uint32_t maxEntryLifetimeMs);

  static void deleteExpiredLogEntries(std::map<std::pair<peerMac_td, messageID_td>, RequestData> &logEntries, const uint32_t requestLifetimeMs, const uint32_t broadcastLifetimeMs);

  template <typename T>
  static void deleteExpiredLogEntries(std::list<T> &logEntries, const uint32_t maxEntryLifetimeMs);

  uint8_t _senderMac[6] = {0};
  uint8_t _senderAPMac[6] = {0};
  
  uint8 _espnowWiFiChannel;
};

#endif
