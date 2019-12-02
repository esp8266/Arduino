/*
  ESP8266WiFiMesh.cpp - Mesh network node
  Sets up a Mesh Node which acts as a router, creating a Mesh Network with other nodes.
 
  Copyright (c) 2015 Julian Fell. All rights reserved.
  Updated 2018 by Anders Löfgren.
 
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







/********************************************************************************************
* NOTE!
*
* This class is deprecated and will be removed in core version 3.0.0.
* If you are still using this class, please consider migrating to the new API shown in 
* the EspnowMeshBackend.h or TcpIpMeshBackend.h source files.
*
* TODO: delete this file.
********************************************************************************************/







#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <WiFiServer.h>
#include <assert.h>

#include "ESP8266WiFiMesh.h"
#include "TypeConversionFunctions.h"

namespace TypeCast = MeshTypeConversionFunctions;

#define SERVER_IP_ADDR      "192.168.4.1"

const IPAddress ESP8266WiFiMesh::emptyIP = IPAddress();

String ESP8266WiFiMesh::lastSSID;
bool ESP8266WiFiMesh::staticIPActivated = false;

// IP needs to be at the same subnet as server gateway (192.168.4 in this case). Station gateway ip must match ip for server.
IPAddress ESP8266WiFiMesh::staticIP = emptyIP;
IPAddress ESP8266WiFiMesh::gateway = IPAddress(192,168,4,1);
IPAddress ESP8266WiFiMesh::subnetMask = IPAddress(255,255,255,0);
ESP8266WiFiMesh *ESP8266WiFiMesh::apController = nullptr;

std::vector<NetworkInfo> ESP8266WiFiMesh::connectionQueue = {};
std::vector<TransmissionResult> ESP8266WiFiMesh::latestTransmissionOutcomes = {};

ESP8266WiFiMesh::~ESP8266WiFiMesh()
{
  deactivateAP();
}

ESP8266WiFiMesh::ESP8266WiFiMesh(ESP8266WiFiMesh::requestHandlerType requestHandler, ESP8266WiFiMesh::responseHandlerType responseHandler, 
                                 ESP8266WiFiMesh::networkFilterType networkFilter, const String &meshPassword, const String &meshName, 
                                 const String &nodeID, bool verboseMode, uint8 meshWiFiChannel, uint16_t serverPort) 
                                 : _server(serverPort)
{  
  updateNetworkNames(meshName, (!nodeID.isEmpty() ? nodeID : TypeCast::uint64ToString(ESP.getChipId())));
  _requestHandler = requestHandler;
  _responseHandler = responseHandler;
  setWiFiChannel(meshWiFiChannel);
  _serverPort = serverPort;
  _meshPassword = meshPassword;
  _verboseMode = verboseMode;
  _networkFilter = networkFilter;
}

void ESP8266WiFiMesh::updateNetworkNames(const String &newMeshName, const String &newNodeID)
{
  if(!newMeshName.isEmpty())
    _meshName = newMeshName;
  if(!newNodeID.isEmpty())
    _nodeID = newNodeID;

  String newSSID = _meshName + _nodeID;

  if(_SSID != newSSID)
  {
    _SSID = newSSID;
  
    // Apply SSID changes to active AP.
    if(isAPController())
      restartAP();
  }
}

void ESP8266WiFiMesh::begin()
{
  ////////////////////////////<DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////
  if(_handler != NULL)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( _SSID.c_str() );
    _server.begin();
  }
  else
  {
  ////////////////////////////</DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////
    if(!ESP8266WiFiMesh::getAPController()) // If there is no active AP controller
      WiFi.mode(WIFI_STA); // WIFI_AP_STA mode automatically sets up an AP, so we can't use that as default.
    
    #if LWIP_VERSION_MAJOR >= 2
    verboseModePrint(F("lwIP version is at least 2. Static ip optimizations enabled.\n"));
    #else
    verboseModePrint(F("lwIP version is less than 2. Static ip optimizations DISABLED.\n"));
    #endif
  }
}

void ESP8266WiFiMesh::setStaticIP(const IPAddress &newIP)
{
  // Comment out the line below to remove static IP and use DHCP instead. 
  // DHCP makes WiFi connection happen slower, but there is no need to care about manually giving different IPs to the nodes and less need to worry about used IPs giving "Server unavailable" issues. 
  // Static IP has faster connection times (50 % of DHCP) and will make sending of data to a node that is already transmitting data happen more reliably. 
  // Note that after WiFi.config(staticIP, gateway, subnetMask) is used, static IP will always be active, even for new connections, unless WiFi.config(0u,0u,0u); is called.
  WiFi.config(newIP, gateway, subnetMask); 
  staticIPActivated = true;
  staticIP = newIP;
}

IPAddress ESP8266WiFiMesh::getStaticIP()
{
  if(staticIPActivated)
    return staticIP;

  return emptyIP;
}

void ESP8266WiFiMesh::disableStaticIP()
{
  WiFi.config(0u,0u,0u);
  optimistic_yield(10000);
  staticIPActivated = false;
}

void ESP8266WiFiMesh::activateAP()
{
  // Deactivate active AP to avoid two servers using the same port, which can lead to crashes.
  if(ESP8266WiFiMesh *currentAPController = ESP8266WiFiMesh::getAPController())
    currentAPController->deactivateAP();

  WiFi.softAP( _SSID.c_str(), _meshPassword.c_str(), _meshWiFiChannel, _apHidden, _maxAPStations ); // Note that a maximum of 8 stations can be connected at a time to each AP
  WiFi.mode(WIFI_AP_STA);

  _server = WiFiServer(_serverPort); // Fixes an occasional crash bug that occurs when using the copy constructor to duplicate the AP controller.
  _server.begin(); // Actually calls _server.stop()/_server.close() first.

  apController = this;
}

void ESP8266WiFiMesh::deactivateAP()
{
  if(isAPController())
  {
    _server.stop();
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);

    // Since there is no active AP controller now, make the apController variable point to nothing. 
    apController = nullptr;
  }
}

void ESP8266WiFiMesh::restartAP()
{
  deactivateAP();
  optimistic_yield(10000);
  activateAP();
  optimistic_yield(10000);
}

ESP8266WiFiMesh * ESP8266WiFiMesh::getAPController()
{
  return apController; 
}

bool ESP8266WiFiMesh::isAPController()
{
  return (this == apController);
}

void ESP8266WiFiMesh::setWiFiChannel(uint8 newWiFiChannel)
{
  assert(1 <= newWiFiChannel && newWiFiChannel <= 13);
  
  _meshWiFiChannel = newWiFiChannel;

  // WiFi.channel() will change if this node connects to an AP with another channel, 
  // so there is no guarantee we are using _meshWiFiChannel.
  // Also, we cannot change the WiFi channel while we are still connected to the other AP.
  if(WiFi.channel() != _meshWiFiChannel && WiFi.status() != WL_CONNECTED)
  {  
    // Apply changes to active AP.
    if(isAPController())
      restartAP();
  }
}

uint8 ESP8266WiFiMesh::getWiFiChannel()
{
  return _meshWiFiChannel;
}

void ESP8266WiFiMesh::setMeshName(const String &newMeshName)
{
  updateNetworkNames(newMeshName);
}

String ESP8266WiFiMesh::getMeshName() {return _meshName;}

void ESP8266WiFiMesh::setNodeID(const String &newNodeID)
{
  updateNetworkNames("", newNodeID);
}

String ESP8266WiFiMesh::getNodeID() {return _nodeID;}

void ESP8266WiFiMesh::setSSID(const String &newMeshName, const String &newNodeID)
{
  updateNetworkNames(newMeshName, newNodeID);
}

String ESP8266WiFiMesh::getSSID() {return _SSID;}

void ESP8266WiFiMesh::setMessage(const String &newMessage) {_message = newMessage;}
String ESP8266WiFiMesh::getMessage() {return _message;}

void ESP8266WiFiMesh::setRequestHandler(ESP8266WiFiMesh::requestHandlerType requestHandler) {_requestHandler = requestHandler;}
ESP8266WiFiMesh::requestHandlerType ESP8266WiFiMesh::getRequestHandler() {return _requestHandler;}

void ESP8266WiFiMesh::setResponseHandler(ESP8266WiFiMesh::responseHandlerType responseHandler) {_responseHandler = responseHandler;}
ESP8266WiFiMesh::responseHandlerType ESP8266WiFiMesh::getResponseHandler() {return _responseHandler;}

void ESP8266WiFiMesh::setNetworkFilter(ESP8266WiFiMesh::networkFilterType networkFilter) {_networkFilter = networkFilter;}
ESP8266WiFiMesh::networkFilterType ESP8266WiFiMesh::getNetworkFilter() {return _networkFilter;}

void ESP8266WiFiMesh::setScanHidden(bool scanHidden)
{
  _scanHidden = scanHidden;
}

bool ESP8266WiFiMesh::getScanHidden() {return _scanHidden;}

void ESP8266WiFiMesh::setAPHidden(bool apHidden)
{
  if(_apHidden != apHidden)
  {
    _apHidden = apHidden;
    
    // Apply changes to active AP.
    if(isAPController())
      restartAP();
  }
}

bool ESP8266WiFiMesh::getAPHidden() {return _apHidden;}

void ESP8266WiFiMesh::setMaxAPStations(uint8_t maxAPStations)
{
  assert(maxAPStations <= 8); // Valid values are 0 to 8, but uint8_t is always at least 0.
  
  if(_maxAPStations != maxAPStations)
  {
    _maxAPStations = maxAPStations;
    
    // Apply changes to active AP.
    if(isAPController())
      restartAP();
  }
}

bool ESP8266WiFiMesh::getMaxAPStations() {return _maxAPStations;}

void ESP8266WiFiMesh::setConnectionAttemptTimeout(int32_t connectionAttemptTimeoutMs)
{
  _connectionAttemptTimeoutMs = connectionAttemptTimeoutMs;
}

int32_t ESP8266WiFiMesh::getConnectionAttemptTimeout() {return _connectionAttemptTimeoutMs;}

void ESP8266WiFiMesh::setStationModeTimeout(int stationModeTimeoutMs)
{
  _stationModeTimeoutMs = stationModeTimeoutMs;
}

int ESP8266WiFiMesh::getStationModeTimeout() {return _stationModeTimeoutMs;}

void ESP8266WiFiMesh::setAPModeTimeout(uint32_t apModeTimeoutMs)
{
  _apModeTimeoutMs = apModeTimeoutMs;
}

uint32_t ESP8266WiFiMesh::getAPModeTimeout() {return _apModeTimeoutMs;}

bool ESP8266WiFiMesh::latestTransmissionSuccessful()
{
  if(ESP8266WiFiMesh::latestTransmissionOutcomes.empty())
    return false;
  else
    for(TransmissionResult &transmissionResult : ESP8266WiFiMesh::latestTransmissionOutcomes) 
      if(transmissionResult.transmissionStatus != TS_TRANSMISSION_COMPLETE) 
        return false;

  return true;
}

/**
 * Disconnect completely from a network.
 */
void ESP8266WiFiMesh::fullStop(WiFiClient &currClient)
{
  currClient.stop();
  optimistic_yield(10000);
  WiFi.disconnect();
  optimistic_yield(10000);
}

/**
 * Wait for a WiFiClient to transmit
 *
 * @return: True if the client is ready, false otherwise.
 * 
 */
bool ESP8266WiFiMesh::waitForClientTransmission(WiFiClient &currClient, uint32_t maxWait)
{
  uint32_t connectionStartTime = millis();
  uint32_t waitingTime = millis() - connectionStartTime;
  while(currClient.connected() && !currClient.available() && waitingTime < maxWait)
  {
    delay(1);
    waitingTime = millis() - connectionStartTime;
  }

  /* Return false if the client isn't ready to communicate */
  if (WiFi.status() == WL_DISCONNECTED && !currClient.available())
  {
    verboseModePrint(F("Disconnected!")); 
    return false;
  }
  
  return true;
}

/**
 * Send the mesh instance's current message then read back the other node's response
 * and pass that to the user-supplied responseHandler.
 *
 * @param currClient The client to which the message should be transmitted.
 * @return: A status code based on the outcome of the exchange.
 * 
 */
transmission_status_t ESP8266WiFiMesh::exchangeInfo(WiFiClient &currClient)
{
  verboseModePrint("Transmitting");  // Not storing strings in flash (via F()) to avoid performance impacts when using the string.
    
  currClient.print(getMessage() + '\r');
  optimistic_yield(10000);

  if (!waitForClientTransmission(currClient, _stationModeTimeoutMs))
  {
    fullStop(currClient);
    return TS_CONNECTION_FAILED;
  }

  if (!currClient.available()) 
  {
    verboseModePrint(F("No response!"));
    return TS_TRANSMISSION_FAILED; // WiFi.status() != WL_DISCONNECTED so we do not want to use fullStop(currClient) here since that would force the node to scan for WiFi networks.
  }

  String response = currClient.readStringUntil('\r');
  optimistic_yield(10000);
  currClient.flush();

  /* Pass data to user callback */
  return _responseHandler(response, *this);
}

/**
 * Handle data transfer process with a connected AP.
 *
 * @return: A status code based on the outcome of the data transfer attempt.
 */
transmission_status_t ESP8266WiFiMesh::attemptDataTransfer()
{
  // Unlike WiFi.mode(WIFI_AP);, WiFi.mode(WIFI_AP_STA); allows us to stay connected to the AP we connected to in STA mode, at the same time as we can receive connections from other stations. 
  // We cannot send data to the AP in STA_AP mode though, that requires STA mode. 
  // Switching to STA mode will disconnect all stations connected to the node AP (though they can request a reconnect even while we are in STA mode).
  WiFiMode_t storedWiFiMode = WiFi.getMode();
  WiFi.mode(WIFI_STA);
  delay(1);
  transmission_status_t transmissionOutcome = attemptDataTransferKernel();
  WiFi.mode(storedWiFiMode);
  delay(1);
  
  return transmissionOutcome;
}

/**
 * Helper function that contains the core functionality for the data transfer process with a connected AP.
 *
 * @return: A status code based on the outcome of the data transfer attempt.
 */
transmission_status_t ESP8266WiFiMesh::attemptDataTransferKernel()
{
  WiFiClient currClient;
  currClient.setTimeout(_stationModeTimeoutMs);

  /* Connect to the node's server */
  if (!currClient.connect(SERVER_IP_ADDR, _serverPort)) 
  {
    fullStop(currClient);
    verboseModePrint(F("Server unavailable"));
    return TS_CONNECTION_FAILED;
  }  

  transmission_status_t transmissionOutcome = exchangeInfo(currClient);
  if (transmissionOutcome <= 0)
  {
    verboseModePrint(F("Transmission failed during exchangeInfo."));
    return transmissionOutcome;
  }
  
  currClient.stop();
  optimistic_yield(10000);

  return transmissionOutcome;
}

void ESP8266WiFiMesh::initiateConnectionToAP(const String &targetSSID, int targetChannel, uint8_t *targetBSSID)
{
  if(targetChannel == NETWORK_INFO_DEFAULT_INT)
    WiFi.begin( targetSSID.c_str(), _meshPassword.c_str() ); // Without giving channel and BSSID, connection time is longer.
  else if(targetBSSID == NULL)
    WiFi.begin( targetSSID.c_str(), _meshPassword.c_str(), targetChannel ); // Without giving channel and BSSID, connection time is longer.
  else
    WiFi.begin( targetSSID.c_str(), _meshPassword.c_str(), targetChannel, targetBSSID );
}

/**
 * Connect to the AP at SSID and transmit the mesh instance's current message.
 *
 * @param targetSSID The name of the AP the other node has set up.
 * @param targetChannel The WiFI channel of the AP the other node has set up.
 * @param targetBSSID The mac address of the AP the other node has set up.
 * @return: A status code based on the outcome of the connection and data transfer process.
 * 
 */
transmission_status_t ESP8266WiFiMesh::connectToNode(const String &targetSSID, int targetChannel, uint8_t *targetBSSID)
{
  if(staticIPActivated && !lastSSID.isEmpty() && lastSSID != targetSSID) // So we only do this once per connection, in case there is a performance impact.
  {
    #if LWIP_VERSION_MAJOR >= 2
    // Can be used with Arduino core for ESP8266 version 2.4.2 or higher with lwIP2 enabled to keep static IP on even during network switches.
    WiFiMode_t storedWiFiMode = WiFi.getMode();
    WiFi.mode(WIFI_OFF);
    WiFi.mode(storedWiFiMode);
      optimistic_yield(10000);
    
    #else
    // Disable static IP so that we can connect to other servers via DHCP (DHCP is slower but required for connecting to more than one server, it seems (possible bug?)).
    disableStaticIP();
    verboseModePrint(F("\nConnecting to a different network. Static IP deactivated to make this possible."));
    
    #endif
  }
  lastSSID = targetSSID;
  
  verboseModePrint(F("Connecting... "), false);
  initiateConnectionToAP(targetSSID, targetChannel, targetBSSID);

  int connectionStartTime = millis();
  int attemptNumber = 1;

  int waitingTime = millis() - connectionStartTime;
  while((WiFi.status() == WL_DISCONNECTED) && waitingTime <= _connectionAttemptTimeoutMs)
  {
    if(waitingTime > attemptNumber * _connectionAttemptTimeoutMs) // _connectionAttemptTimeoutMs can be replaced (lowered) if you want to limit the time allowed for each connection attempt.
    {
      verboseModePrint(F("... "), false);
      WiFi.disconnect();
      optimistic_yield(10000);
      initiateConnectionToAP(targetSSID, targetChannel, targetBSSID);
      attemptNumber++;
    }
    delay(1);
    waitingTime = millis() - connectionStartTime;
  }

  verboseModePrint(String(waitingTime));
  
  /* If the connection timed out */
  if (WiFi.status() != WL_CONNECTED)
  {
    verboseModePrint(F("Timeout"));
    return TS_CONNECTION_FAILED;
  }

  return attemptDataTransfer();
}

void ESP8266WiFiMesh::attemptTransmission(const String &message, bool concludingDisconnect, bool initialDisconnect, bool noScan, bool scanAllWiFiChannels)
{
  setMessage(message);
  
  if(initialDisconnect)
  {
    WiFi.disconnect();
    optimistic_yield(10000);
  }

  latestTransmissionOutcomes.clear();
    
  if(WiFi.status() == WL_CONNECTED)
  {
    transmission_status_t transmissionResult = attemptDataTransfer();
    latestTransmissionOutcomes.push_back(TransmissionResult(connectionQueue.back(), transmissionResult));
  }
  else
  {
    if(!noScan)
    {
      verboseModePrint(F("Scanning... "), false);
      
      /* Scan for APs */
      connectionQueue.clear();

      // If scanAllWiFiChannels is true scanning will cause the WiFi radio to cycle through all WiFi channels.
      // This means existing WiFi connections are likely to break or work poorly if done frequently.
      int n = 0;
      if(scanAllWiFiChannels)
      {
        n = WiFi.scanNetworks(false, _scanHidden);
      }
      else
      {
        // Scan function argument overview: scanNetworks(bool async = false, bool show_hidden = false, uint8 channel = 0, uint8* ssid = NULL)
        n = WiFi.scanNetworks(false, _scanHidden, _meshWiFiChannel);
      }
      
      _networkFilter(n, *this); // Update the connectionQueue.
    }
    
    for(NetworkInfo &currentNetwork : connectionQueue)
    {
      WiFi.disconnect();
      optimistic_yield(10000);

      String currentSSID;
      int currentWiFiChannel = NETWORK_INFO_DEFAULT_INT;
      uint8_t *currentBSSID = NULL;

      // If an SSID has been assigned, it is prioritized over an assigned networkIndex since the networkIndex is more likely to change.
      if(!currentNetwork.SSID.isEmpty())
      {
        currentSSID = currentNetwork.SSID;
        currentWiFiChannel = currentNetwork.wifiChannel;
        currentBSSID = currentNetwork.BSSID;
      }
      else // Use only networkIndex
      {
        currentSSID = WiFi.SSID(currentNetwork.networkIndex);
        currentWiFiChannel = WiFi.channel(currentNetwork.networkIndex);
        currentBSSID = WiFi.BSSID(currentNetwork.networkIndex);
      }

      if(_verboseMode) // Avoid string generation if not required
      {
        verboseModePrint(String(F("AP acquired: ")) + currentSSID + String(F(", Ch:")) + String(currentWiFiChannel) + ' ', false);

        if(currentNetwork.networkIndex != NETWORK_INFO_DEFAULT_INT)
        {
          verboseModePrint(String('(') + String(WiFi.RSSI(currentNetwork.networkIndex)) + String(F("dBm) ")) +
                           (WiFi.encryptionType(currentNetwork.networkIndex) == ENC_TYPE_NONE ? String(F("open")) : ""), false);
        }

        verboseModePrint(F("... "), false);
      }

      transmission_status_t transmissionResult = connectToNode(currentSSID, currentWiFiChannel, currentBSSID);

      latestTransmissionOutcomes.push_back(TransmissionResult{.origin = currentNetwork, .transmissionStatus = transmissionResult});
    }
  }

  if(WiFi.status() == WL_CONNECTED && staticIP != emptyIP && !staticIPActivated)
  {
    verboseModePrint(F("Reactivating static IP to allow for faster re-connects."));
    setStaticIP(staticIP);
  }

  // If we do not want to be connected at end of transmission, disconnect here so we can re-enable static IP first (above).
  if(concludingDisconnect)
  {
    WiFi.disconnect();
    optimistic_yield(10000);
  }
}

void ESP8266WiFiMesh::acceptRequest()
{
  ////////////////////////////<DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////
  if(_handler != NULL)
  {
    while (true) {
      _client = _server.available();
      if (!_client)
        break;

      if (!waitForClient(_client, _apModeTimeoutMs)) {
        continue;
      }

      /* Read in request and pass it to the supplied handler */
      String request = _client.readStringUntil('\r');
      _client.readStringUntil('\n');

      String response = _handler(request);

      /* Send the response back to the client */
      if (_client.connected())
        _client.println(response);
    }
  }
  else
  {
  ////////////////////////////</DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////
    while (true) {
      WiFiClient _client = _server.available();
      
      if (!_client)
        break;

      if (!waitForClientTransmission(_client, _apModeTimeoutMs) || !_client.available()) {
        continue;
      }
      
      /* Read in request and pass it to the supplied requestHandler */
      String request = _client.readStringUntil('\r');
      optimistic_yield(10000);
      _client.flush();
      
      String response = _requestHandler(request, *this);

      /* Send the response back to the client */
      if (_client.connected())
      {
        verboseModePrint("Responding"); // Not storing strings in flash (via F()) to avoid performance impacts when using the string.
        _client.print(response + '\r');
        _client.flush();
        optimistic_yield(10000);
      }
    }
  }
}


void ESP8266WiFiMesh::verboseModePrint(const String &stringToPrint, bool newline)
{
  if(_verboseMode)
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}
