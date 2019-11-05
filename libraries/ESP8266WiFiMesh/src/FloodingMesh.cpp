/*
 * Copyright (C) 2019 Anders Löfgren
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "FloodingMesh.h"
#include "TypeConversionFunctions.h"
#include "JsonTranslator.h"

std::set<FloodingMesh *> FloodingMesh::availableFloodingMeshes = {};

char FloodingMesh::_broadcastMetadataDelimiter = 23;

void floodingMeshDelay(uint32_t durationMs)
{
  uint32_t startingTime = millis();
  
  while(millis() - startingTime < durationMs)
  {
    delay(1);
    FloodingMesh::performMeshMaintenance();
  }
}

FloodingMesh::FloodingMesh(messageHandlerType messageHandler, const String &meshPassword, const uint8_t espnowEncryptionKey[EspnowProtocolInterpreter::espnowEncryptionKeyLength], 
                           const uint8_t espnowHashKey[EspnowProtocolInterpreter::espnowHashKeyLength], const String &ssidPrefix, 
                           const String &ssidSuffix, bool verboseMode, uint8 meshWiFiChannel) 
                           : _espnowBackend(
                            [this](const String &request, MeshBackendBase &meshInstance){ return _defaultRequestHandler(request, meshInstance); }, 
                            [this](const String &response, MeshBackendBase &meshInstance){ return _defaultResponseHandler(response, meshInstance); }, 
                            [this](int numberOfNetworks, MeshBackendBase &meshInstance){ return _defaultNetworkFilter(numberOfNetworks, meshInstance); }, 
                            [this](String &firstTransmission, EspnowMeshBackend &meshInstance){ return _defaultBroadcastFilter(firstTransmission, meshInstance); },
                            meshPassword, espnowEncryptionKey, espnowHashKey, ssidPrefix, ssidSuffix, verboseMode, meshWiFiChannel)
{
  setMessageHandler(messageHandler);
  restoreDefaultTransmissionOutcomesUpdateHook();
}

FloodingMesh::FloodingMesh(const String &serializedMeshState, messageHandlerType messageHandler, const String &meshPassword, 
                           const uint8_t espnowEncryptionKey[EspnowProtocolInterpreter::espnowEncryptionKeyLength], 
                           const uint8_t espnowHashKey[EspnowProtocolInterpreter::espnowHashKeyLength], const String &ssidPrefix, 
                           const String &ssidSuffix, bool verboseMode, uint8 meshWiFiChannel)
                           : FloodingMesh(messageHandler, meshPassword, espnowEncryptionKey, espnowHashKey, ssidPrefix, ssidSuffix, verboseMode, meshWiFiChannel) 
{
  loadMeshState(serializedMeshState);
}

FloodingMesh::~FloodingMesh()
{
  availableFloodingMeshes.erase(this);
}

void FloodingMesh::begin()
{
  // Initialise the mesh node  
  getEspnowMeshBackend().begin();
  
  availableFloodingMeshes.insert(this); // Returns std::pair<iterator,bool>
}

void FloodingMesh::activateAP()
{
  getEspnowMeshBackend().activateAP();
}

void FloodingMesh::performMeshMaintenance()
{
  for(FloodingMesh *meshInstance : availableFloodingMeshes)
  {
    meshInstance->performMeshInstanceMaintenance();
  }
}

void FloodingMesh::performMeshInstanceMaintenance()
{
  EspnowMeshBackend::performEspnowMaintenance(); 
  
  for(std::list<std::pair<String, bool>>::iterator backlogIterator = _forwardingBacklog.begin();  backlogIterator != _forwardingBacklog.end(); )
  {
    std::pair<String, bool> &messageData = *backlogIterator;
    if(messageData.second) // message encrypted
    {
      _macIgnoreList = messageData.first.substring(0, 12) + ','; // The message should contain the messageID first
      encryptedBroadcastKernel(messageData.first); 
      _macIgnoreList = "";
    }
    else
    {
      broadcastKernel(messageData.first);
    }

    backlogIterator = _forwardingBacklog.erase(backlogIterator);
    
    EspnowMeshBackend::performEspnowMaintenance(); // It is best to performEspnowMaintenance frequently to keep the Espnow backend responsive. Especially if each encryptedBroadcast takes a lot of time.
  }
}

String FloodingMesh::serializeMeshState()
{
  using namespace JsonTranslator;
  
  // Returns: {"meshState":{"connectionState":{"unencMsgID":"123"},"meshMsgCount":"123"}}

  String connectionState = getEspnowMeshBackend().serializeUnencryptedConnection();
  
  return 
  "{\"meshState\":{"
  + connectionState.substring(1, connectionState.length() - 1) + ","
  + createJsonEndPair(jsonMeshMessageCount, String(_messageCount));
}

void FloodingMesh::loadMeshState(const String &serializedMeshState)
{
  using namespace JsonTranslator;
  
  if(!getMeshMessageCount(serializedMeshState, _messageCount))
    getEspnowMeshBackend().warningPrint("WARNING! serializedMeshState did not contain MeshMessageCount. Using default instead.");

  String connectionState = "";
  if(!getConnectionState(serializedMeshState, connectionState) || !getEspnowMeshBackend().addUnencryptedConnection(connectionState))
  {
    getEspnowMeshBackend().warningPrint("WARNING! serializedMeshState did not contain unencryptedMessageID. Using default instead.");
  }
}

String FloodingMesh::generateMessageID()
{
  char messageCountArray[2] = { 0 };
  sprintf(messageCountArray, "%04X", _messageCount++);
  uint8_t apMac[6] {0};
  return macToString(WiFi.softAPmacAddress(apMac)) + String(messageCountArray); // We use the AP MAC address as ID since it is what shows up during WiFi scans
}

void FloodingMesh::broadcast(const String &message)
{
  assert(message.length() <= maxUnencryptedMessageSize());
  
  String messageID = generateMessageID();

  // Remove getEspnowMeshBackend().getMeshName() from the broadcastMetadata below to broadcast to all ESP-NOW nodes regardless of MeshName.
  String targetMeshName = getEspnowMeshBackend().getMeshName();

  broadcastKernel(targetMeshName + String(broadcastMetadataDelimiter()) + messageID + String(broadcastMetadataDelimiter()) + message);
}

void FloodingMesh::broadcastKernel(const String &message)
{
  getEspnowMeshBackend().broadcast(message);
}

void FloodingMesh::setBroadcastReceptionRedundancy(uint8_t redundancy) 
{ 
  assert(redundancy < 255);
  _broadcastReceptionRedundancy = redundancy;
}
uint8_t FloodingMesh::getBroadcastReceptionRedundancy() { return _broadcastReceptionRedundancy; }

void FloodingMesh::encryptedBroadcast(const String &message)
{
  assert(message.length() <= maxEncryptedMessageSize());

  String messageID = generateMessageID();
  
  encryptedBroadcastKernel(messageID + String(broadcastMetadataDelimiter()) + message);  
}

void FloodingMesh::encryptedBroadcastKernel(const String &message)
{
  getEspnowMeshBackend().attemptAutoEncryptingTransmission(message);
}

void FloodingMesh::clearMessageLogs()
{
  _messageIDs.clear();
  std::queue<messageQueueElementType>().swap(_messageIdOrder);
}

void FloodingMesh::clearForwardingBacklog()
{
  _forwardingBacklog.clear();
}
 
void FloodingMesh::setMessageHandler(messageHandlerType messageHandler) { _messageHandler = messageHandler; }
FloodingMesh::messageHandlerType FloodingMesh::getMessageHandler() { return _messageHandler; }

void FloodingMesh::setOriginMac(uint8_t *macArray)
{
  std::copy_n(macArray, 6, _originMac);
}

String FloodingMesh::getOriginMac() { return macToString(_originMac); }
uint8_t *FloodingMesh::getOriginMac(uint8_t *macArray)
{
  std::copy_n(_originMac, 6, macArray);
  return macArray;
}

uint32_t FloodingMesh::maxUnencryptedMessageSize()
{
  return getEspnowMeshBackend().getMaxMessageLength() - MESSAGE_ID_LENGTH - (getEspnowMeshBackend().getMeshName().length() + 1); // Need room for mesh name + delimiter
}

uint32_t FloodingMesh::maxEncryptedMessageSize()
{
  // Need 1 extra delimiter character for maximum metadata efficiency (makes it possible to store exactly 18 MACs in metadata by adding an extra transmission)
  return getEspnowMeshBackend().getMaxMessageLength() - MESSAGE_ID_LENGTH - 1;
}
 
void FloodingMesh::setMessageLogSize(uint16_t messageLogSize) 
{ 
  assert(messageLogSize >= 1);
  _messageLogSize = messageLogSize; 
}
uint16_t FloodingMesh::messageLogSize() { return _messageLogSize; }

void FloodingMesh::setBroadcastMetadataDelimiter(char broadcastMetadataDelimiter) 
{ 
  // Using HEX number characters as a delimiter is a bad idea regardless of broadcast type, since they are always in the broadcast metadata
  assert(broadcastMetadataDelimiter < 48 || 57 < broadcastMetadataDelimiter);
  assert(broadcastMetadataDelimiter < 65 || 70 < broadcastMetadataDelimiter);
  
  _broadcastMetadataDelimiter = broadcastMetadataDelimiter; 
}
char FloodingMesh::broadcastMetadataDelimiter() { return _broadcastMetadataDelimiter; }
  
EspnowMeshBackend &FloodingMesh::getEspnowMeshBackend()
{
  return _espnowBackend;
}

bool FloodingMesh::insertPreliminaryMessageID(uint64_t messageID)
{
  uint8_t apMacArray[6] = { 0 };
  if(messageID >> 16 == macToUint64(WiFi.softAPmacAddress(apMacArray)))
    return false; // The node should not receive its own messages.
  
  auto insertionResult = _messageIDs.emplace(messageID, 0); // Returns std::pair<iterator,bool>

  if(insertionResult.second) // Insertion succeeded.
    updateMessageQueue(insertionResult.first);
  else if(insertionResult.first->second < getBroadcastReceptionRedundancy()) // messageID exists but not with desired redundancy
    insertionResult.first->second++;
  else
    return false; // messageID already existed in _messageIDs with desired redundancy

  return true;
}

bool FloodingMesh::insertCompletedMessageID(uint64_t messageID)
{
  uint8_t apMacArray[6] = { 0 };
  if(messageID >> 16 == macToUint64(WiFi.softAPmacAddress(apMacArray)))
    return false; // The node should not receive its own messages.
  
  auto insertionResult = _messageIDs.emplace(messageID, MESSAGE_COMPLETE); // Returns std::pair<iterator,bool>

  if(insertionResult.second) // Insertion succeeded.
    updateMessageQueue(insertionResult.first);
  else if(insertionResult.first->second < MESSAGE_COMPLETE) // messageID exists but is not complete
    insertionResult.first->second = MESSAGE_COMPLETE;
  else
    return false; // messageID already existed in _messageIDs and is complete

  return true;
}

void FloodingMesh::updateMessageQueue(messageQueueElementType messageIterator)
{
  _messageIdOrder.emplace(messageIterator);
  
  if(_messageIDs.size() > messageLogSize())
  {
    _messageIDs.erase(_messageIdOrder.front());
    _messageIdOrder.pop();
    assert(_messageIDs.size() == messageLogSize()); // If this is false we either have too many elements in messageIDs or we deleted too many elements.
    assert(_messageIDs.size() == _messageIdOrder.size()); // The containers should always be in sync
  }
}

void FloodingMesh::restoreDefaultRequestHandler()
{
  getEspnowMeshBackend().setRequestHandler([this](const String &request, MeshBackendBase &meshInstance){ return _defaultRequestHandler(request, meshInstance); });
}

void FloodingMesh::restoreDefaultResponseHandler()
{
  getEspnowMeshBackend().setResponseHandler([this](const String &response, MeshBackendBase &meshInstance){ return _defaultResponseHandler(response, meshInstance); });
}

void FloodingMesh::restoreDefaultNetworkFilter()
{
  getEspnowMeshBackend().setNetworkFilter([this](int numberOfNetworks, MeshBackendBase &meshInstance){ return _defaultNetworkFilter(numberOfNetworks, meshInstance); });
}

void FloodingMesh::restoreDefaultBroadcastFilter()
{
  getEspnowMeshBackend().setBroadcastFilter([this](String &firstTransmission, EspnowMeshBackend &meshInstance){ return _defaultBroadcastFilter(firstTransmission, meshInstance); });
}

void FloodingMesh::restoreDefaultTransmissionOutcomesUpdateHook()
{
  /* Optional way of doing things. Lambda is supposedly better https://stackoverflow.com/a/36596295 .
     
  using namespace std::placeholders;
  
  getEspnowMeshBackend().setTransmissionOutcomesUpdateHook(std::bind(&FloodingMesh::_defaultTransmissionOutcomesUpdateHook, this, _1));
  */
  
  getEspnowMeshBackend().setTransmissionOutcomesUpdateHook([this](MeshBackendBase &meshInstance){ return _defaultTransmissionOutcomesUpdateHook(meshInstance); });
}

/**
 * Callback for when other nodes send you a request
 *
 * @param request The request string received from another node in the mesh
 * @param meshInstance The MeshBackendBase instance that called the function.
 * @return The string to send back to the other node. For ESP-NOW, return an empy string ("") if no response should be sent.
 */
String FloodingMesh::_defaultRequestHandler(const String &request, MeshBackendBase &meshInstance)
{
  (void)meshInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
  
  String broadcastTarget = "";
  String remainingRequest = "";
  
  if(request.charAt(0) == broadcastMetadataDelimiter())
  {
    int32_t broadcastTargetEndIndex = request.indexOf(broadcastMetadataDelimiter(), 1);

    if(broadcastTargetEndIndex == -1)
      return ""; // broadcastMetadataDelimiter not found
    
    broadcastTarget = request.substring(1, broadcastTargetEndIndex + 1); // Include delimiter
    remainingRequest = request.substring(broadcastTargetEndIndex + 1);
  }
  else
  {
    remainingRequest = request;
  }
  
  int32_t messageIDEndIndex = remainingRequest.indexOf(broadcastMetadataDelimiter());

  if(messageIDEndIndex == -1)
    return ""; // broadcastMetadataDelimiter not found

  uint64_t messageID = stringToUint64(remainingRequest.substring(0, messageIDEndIndex));

  if(insertCompletedMessageID(messageID))
  {
    uint8_t originMacArray[6] = { 0 };
    setOriginMac(uint64ToMac(messageID >> 16, originMacArray));
  
    String message = remainingRequest.substring(messageIDEndIndex + 1);
    
    if(getMessageHandler()(message, *this))
    {
      message = broadcastTarget + remainingRequest.substring(0, messageIDEndIndex + 1) + message;
      assert(message.length() <= _espnowBackend.getMaxMessageLength());
      _forwardingBacklog.emplace_back(message, getEspnowMeshBackend().receivedEncryptedMessage());
    }
  }
  
  return "";
}

/**
 * Callback for when you get a response from other nodes
 *
 * @param response The response string received from another node in the mesh
 * @param meshInstance The MeshBackendBase instance that called the function.
 * @return The status code resulting from the response, as an int
 */
transmission_status_t FloodingMesh::_defaultResponseHandler(const String &response, MeshBackendBase &meshInstance)
{
  transmission_status_t statusCode = TS_TRANSMISSION_COMPLETE;

  getEspnowMeshBackend().warningPrint("WARNING! Response to FloodingMesh broadcast received, but none is expected!");

  (void)response; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
  (void)meshInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.

  return statusCode;
}

/**
 * Callback used to decide which networks to connect to once a WiFi scan has been completed. 
 *
 * @param numberOfNetworks The number of networks found in the WiFi scan.
 * @param meshInstance The MeshBackendBase instance that called the function.
 */
void FloodingMesh::_defaultNetworkFilter(int numberOfNetworks, MeshBackendBase &meshInstance)
{
  // Note that the network index of a given node may change whenever a new scan is done.
  for (int networkIndex = 0; networkIndex < numberOfNetworks; ++networkIndex) 
  {
    String currentSSID = WiFi.SSID(networkIndex);
    int meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

    // Connect to any APs which contain meshInstance.getMeshName()
    if(meshNameIndex >= 0)
    {      
      if(_macIgnoreList.indexOf(macToString(WiFi.BSSID(networkIndex))) == -1) // If the BSSID is not in the ignore list
      {
        if(EspnowMeshBackend *espnowInstance = meshBackendCast<EspnowMeshBackend *>(&meshInstance))
        {
          espnowInstance->connectionQueue().push_back(networkIndex);
        }
        else
        {
          Serial.println(String(F("Invalid mesh backend!")));
        }
      }
    }
  }
}

/**
 * Callback used to decide which broadcast messages to accept. Only called for the first transmission in each broadcast.
 * If true is returned from this callback, the first broadcast transmission is saved until the entire broadcast message has been received.
 * The complete broadcast message will then be sent to the requestHandler.
 * If false is returned from this callback, the broadcast message is discarded.
 * Note that the BroadcastFilter may be called multiple times for messages that are discarded in this way, but is only called once for accepted messages.
 *
 * @param firstTransmission The first transmission of the broadcast.
 * @param meshInstance The EspnowMeshBackend instance that called the function.
 * 
 * @return True if the broadcast should be accepted. False otherwise.
 */
bool FloodingMesh::_defaultBroadcastFilter(String &firstTransmission, EspnowMeshBackend &meshInstance)
{
  // This broadcastFilter will accept a transmission if it contains the broadcastMetadataDelimiter
  // and as metaData either no targetMeshName or a targetMeshName that matches the MeshName of meshInstance
  // and insertPreliminaryMessageID(messageID) returns true.

  // Broadcast firstTransmission String structure: targetMeshName+messageID+message.
   
  int32_t metadataEndIndex = firstTransmission.indexOf(broadcastMetadataDelimiter());

  if(metadataEndIndex == -1)
    return false; // broadcastMetadataDelimiter not found

  String targetMeshName = firstTransmission.substring(0, metadataEndIndex);
  
  if(targetMeshName != "" && meshInstance.getMeshName() != targetMeshName)
  {
    return false; // Broadcast is for another mesh network
  }
  else 
  {
    int32_t messageIDEndIndex = firstTransmission.indexOf(broadcastMetadataDelimiter(), metadataEndIndex + 1);

    if(messageIDEndIndex == -1)
      return false; // broadcastMetadataDelimiter not found
  
    uint64_t messageID = stringToUint64(firstTransmission.substring(metadataEndIndex + 1, messageIDEndIndex));

    if(insertPreliminaryMessageID(messageID))
    {
      // Add broadcast identifier to stored message and mark as accepted broadcast.
      firstTransmission = String(broadcastMetadataDelimiter()) + firstTransmission;
      return true;
    }
    else
    {
      return false; // Broadcast has already been received the maximum number of times
    }
  }
}

/**
 * Once passed to the setTransmissionOutcomesUpdateHook method of the ESP-NOW backend, 
 * this function will be called after each update of the latestTransmissionOutcomes vector during attemptTransmission. 
 * (which happens after each individual transmission has finished)
 * 
 * @param meshInstance The MeshBackendBase instance that called the function.
 * 
 * @return True if attemptTransmission should continue with the next entry in the connectionQueue. False if attemptTransmission should stop.
 */
bool FloodingMesh::_defaultTransmissionOutcomesUpdateHook(MeshBackendBase &meshInstance)
{
  (void)meshInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.

  return true;
}
