/*
  TcpIpMeshBackend
 
  Copyright (c) 2015 Julian Fell and 2019 Anders Löfgren. All rights reserved.
 
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

#include <WiFiClient.h> 
#include <WiFiServer.h>
#include <assert.h>
#include <Schedule.h>

#include "TcpIpMeshBackend.h"
#include "TypeConversionFunctions.h"
#include "MutexTracker.h"
#include "ExpiringTimeTracker.h"

namespace
{
  constexpr char SERVER_IP_ADDR[] PROGMEM = "192.168.4.1";

  String _temporaryMessage;
  String lastSSID;
  bool staticIPActivated = false;

  // IP needs to be at the same subnet as server gateway (192.168.4 in this case). Station gateway ip must match ip for server.
  IPAddress staticIP;
  IPAddress gateway(192,168,4,1);
  IPAddress subnetMask(255,255,255,0);
}

const IPAddress TcpIpMeshBackend::emptyIP;

std::shared_ptr<bool> TcpIpMeshBackend::_tcpIpTransmissionMutex = std::make_shared<bool>(false);
std::shared_ptr<bool> TcpIpMeshBackend::_tcpIpConnectionQueueMutex = std::make_shared<bool>(false);

std::vector<TcpIpNetworkInfo> TcpIpMeshBackend::_connectionQueue = {};
std::vector<TransmissionOutcome> TcpIpMeshBackend::_latestTransmissionOutcomes = {};

TcpIpMeshBackend::TcpIpMeshBackend(const requestHandlerType requestHandler, const responseHandlerType responseHandler, 
                                   const networkFilterType networkFilter, const String &meshPassword, const String &ssidPrefix, 
                                   const String &ssidSuffix, const bool verboseMode, const uint8 meshWiFiChannel, const uint16_t serverPort) 
                                   : MeshBackendBase(requestHandler, responseHandler, networkFilter, MeshBackendType::TCP_IP), _server(serverPort)
{
  setSSID(ssidPrefix, emptyString, ssidSuffix);
  setMeshPassword(meshPassword);
  setVerboseModeState(verboseMode);
  setWiFiChannel(meshWiFiChannel);
  setServerPort(serverPort);
}

std::vector<TcpIpNetworkInfo> & TcpIpMeshBackend::connectionQueue()
{
  MutexTracker connectionQueueMutexTracker(_tcpIpConnectionQueueMutex);
  if(!connectionQueueMutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! connectionQueue locked. Don't call connectionQueue() from callbacks other than NetworkFilter as this may corrupt program state!"))); 
  }
  
  return _connectionQueue;
}

const std::vector<TcpIpNetworkInfo> & TcpIpMeshBackend::constConnectionQueue()
{  
  return _connectionQueue;
}

std::vector<TransmissionOutcome> & TcpIpMeshBackend::latestTransmissionOutcomes()
{
  return _latestTransmissionOutcomes;
}

bool TcpIpMeshBackend::latestTransmissionSuccessful()
{
  return latestTransmissionSuccessfulBase(latestTransmissionOutcomes());
}

void TcpIpMeshBackend::begin()
{
  if(!TcpIpMeshBackend::getAPController()) // If there is no active AP controller
    WiFi.mode(WIFI_STA); // WIFI_AP_STA mode automatically sets up an AP, so we can't use that as default.
  
  #if LWIP_VERSION_MAJOR >= 2
  verboseModePrint(F("lwIP version is at least 2. Static ip optimizations enabled.\n"));
  #else
  verboseModePrint(F("lwIP version is less than 2. Static ip optimizations DISABLED.\n"));
  #endif
}

void TcpIpMeshBackend::activateAPHook()
{
  WiFi.softAP( getSSID().c_str(), getMeshPassword().c_str(), getWiFiChannel(), getAPHidden(), _maxAPStations ); // Note that a maximum of 8 TCP/IP stations can be connected at a time to each AP, max 4 by default.

  _server = WiFiServer(getServerPort()); // Fixes an occasional crash bug that occurs when using the copy constructor to duplicate the AP controller.
  _server.begin(); // Actually calls _server.stop()/_server.close() first.
}

void TcpIpMeshBackend::deactivateAPHook()
{
  _server.stop();
}

bool TcpIpMeshBackend::transmissionInProgress(){return *_tcpIpTransmissionMutex;}

void TcpIpMeshBackend::setTemporaryMessage(const String &newTemporaryMessage) {_temporaryMessage = newTemporaryMessage;}
String TcpIpMeshBackend::getTemporaryMessage() const {return _temporaryMessage;}
void TcpIpMeshBackend::clearTemporaryMessage() {_temporaryMessage.clear();}

String TcpIpMeshBackend::getCurrentMessage() const
{
  String message = getTemporaryMessage();
  
  if(message.isEmpty()) // If no temporary message stored
    message = getMessage();

  return message;
}

void TcpIpMeshBackend::setStaticIP(const IPAddress &newIP)
{
  // Comment out the line below to remove static IP and use DHCP instead. 
  // DHCP makes WiFi connection happen slower, but there is no need to care about manually giving different IPs to the nodes and less need to worry about used IPs giving "Server unavailable" issues. 
  // Static IP has faster connection times (50 % of DHCP) and will make sending of data to a node that is already transmitting data happen more reliably. 
  // Note that after WiFi.config(staticIP, gateway, subnetMask) is used, static IP will always be active, even for new connections, unless WiFi.config(0u,0u,0u); is called.
  WiFi.config(newIP, gateway, subnetMask); 
  staticIPActivated = true;
  staticIP = newIP;
}

IPAddress TcpIpMeshBackend::getStaticIP() const
{
  if(staticIPActivated)
    return staticIP;

  return emptyIP;
}

void TcpIpMeshBackend::disableStaticIP()
{
  WiFi.config(0u,0u,0u);
  yield();
  staticIPActivated = false;
}

void TcpIpMeshBackend::setServerPort(const uint16_t serverPort)
{
  _serverPort = serverPort;
  
  // Apply changes to active AP.
  if(isAPController())
    restartAP();
}

uint16_t TcpIpMeshBackend::getServerPort() const {return _serverPort;}

void TcpIpMeshBackend::setMaxAPStations(const uint8_t maxAPStations)
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

bool TcpIpMeshBackend::getMaxAPStations() const {return _maxAPStations;}

void TcpIpMeshBackend::setConnectionAttemptTimeout(const uint32_t connectionAttemptTimeoutMs)
{
  _connectionAttemptTimeoutMs = connectionAttemptTimeoutMs;
}

uint32_t TcpIpMeshBackend::getConnectionAttemptTimeout() const {return _connectionAttemptTimeoutMs;}

void TcpIpMeshBackend::setStationModeTimeout(const int stationModeTimeoutMs)
{
  _stationModeTimeoutMs = stationModeTimeoutMs;
}

int TcpIpMeshBackend::getStationModeTimeout() const {return _stationModeTimeoutMs;}

void TcpIpMeshBackend::setAPModeTimeout(const uint32_t apModeTimeoutMs)
{
  _apModeTimeoutMs = apModeTimeoutMs;
}

uint32_t TcpIpMeshBackend::getAPModeTimeout() const {return _apModeTimeoutMs;}

/**
 * Disconnect completely from a network.
 */
void TcpIpMeshBackend::fullStop(WiFiClient &currClient)
{
  currClient.stop();
  yield();
  WiFi.disconnect();
  yield();
}

/**
 * Wait for a WiFiClient to transmit
 *
 * @return True if the client is ready, false otherwise.
 * 
 */
bool TcpIpMeshBackend::waitForClientTransmission(WiFiClient &currClient, const uint32_t maxWait)
{
  ExpiringTimeTracker timeout(maxWait);
  
  while(currClient.connected() && !currClient.available() && !timeout)
  {
    delay(1);
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
 * @return A status code based on the outcome of the exchange.
 * 
 */
TransmissionStatusType TcpIpMeshBackend::exchangeInfo(WiFiClient &currClient)
{
  verboseModePrint(String(F("Transmitting")));
    
  currClient.print(getCurrentMessage() + '\r');
  yield();

  if (!waitForClientTransmission(currClient, _stationModeTimeoutMs))
  {
    fullStop(currClient);
    return TransmissionStatusType::CONNECTION_FAILED;
  }

  if (!currClient.available()) 
  {
    verboseModePrint(F("No response!"));
    return TransmissionStatusType::TRANSMISSION_FAILED; // WiFi.status() != WL_DISCONNECTED so we do not want to use fullStop(currClient) here since that would force the node to scan for WiFi networks.
  }

  String response = currClient.readStringUntil('\r');
  yield();
  currClient.flush();

  /* Pass data to user callback */
  return getResponseHandler()(response, *this);
}

/**
 * Handle data transfer process with a connected AP.
 *
 * @return A status code based on the outcome of the data transfer attempt.
 */
TransmissionStatusType TcpIpMeshBackend::attemptDataTransfer()
{
  // Unlike WiFi.mode(WIFI_AP);, WiFi.mode(WIFI_AP_STA); allows us to stay connected to the AP we connected to in STA mode, at the same time as we can receive connections from other stations. 
  // We cannot send data to the AP in AP_STA mode though, that requires STA mode. 
  // Switching to STA mode will disconnect all stations connected to the node AP (though they can request a reconnect even while we are in STA mode).
  WiFiMode_t storedWiFiMode = WiFi.getMode();
  WiFi.mode(WIFI_STA);
  delay(1);
  TransmissionStatusType transmissionOutcome = attemptDataTransferKernel();
  WiFi.mode(storedWiFiMode);
  delay(1);
  
  return transmissionOutcome;
}

/**
 * Helper function that contains the core functionality for the data transfer process with a connected AP.
 *
 * @return A status code based on the outcome of the data transfer attempt.
 */
TransmissionStatusType TcpIpMeshBackend::attemptDataTransferKernel()
{
  WiFiClient currClient;
  currClient.setTimeout(_stationModeTimeoutMs);

  /* Connect to the node's server */
  if (!currClient.connect(FPSTR(SERVER_IP_ADDR), getServerPort())) 
  {
    fullStop(currClient);
    verboseModePrint(F("Server unavailable"));
    return TransmissionStatusType::CONNECTION_FAILED;
  }  

  TransmissionStatusType transmissionOutcome = exchangeInfo(currClient);
  if (static_cast<int>(transmissionOutcome) <= 0)
  {
    verboseModePrint(F("Transmission failed during exchangeInfo."));
    return transmissionOutcome;
  }
  
  currClient.stop();
  yield();

  return transmissionOutcome;
}

void TcpIpMeshBackend::initiateConnectionToAP(const String &targetSSID, const int targetChannel, const uint8_t *targetBSSID)
{
  if(targetChannel == NETWORK_INFO_DEFAULT_INT)
    WiFi.begin( targetSSID.c_str(), getMeshPassword().c_str() ); // Without giving channel and BSSID, connection time is longer.
  else if(targetBSSID == NULL)
    WiFi.begin( targetSSID.c_str(), getMeshPassword().c_str(), targetChannel ); // Without giving channel and BSSID, connection time is longer.
  else
    WiFi.begin( targetSSID.c_str(), getMeshPassword().c_str(), targetChannel, targetBSSID );
}

/**
 * Connect to the AP at SSID and transmit the mesh instance's current message.
 *
 * @param targetSSID The name of the AP the other node has set up.
 * @param targetChannel The WiFI channel of the AP the other node has set up.
 * @param targetBSSID The MAC address of the AP the other node has set up.
 * @return A status code based on the outcome of the connection and data transfer process.
 * 
 */
TransmissionStatusType TcpIpMeshBackend::connectToNode(const String &targetSSID, const int targetChannel, const uint8_t *targetBSSID)
{  
  if(staticIPActivated && !lastSSID.isEmpty() && lastSSID != targetSSID) // So we only do this once per connection, in case there is a performance impact.
  {
    #if LWIP_VERSION_MAJOR >= 2
    // Can be used with Arduino core for ESP8266 version 2.4.2 or higher with lwIP2 enabled to keep static IP on even during network switches.
    WiFiMode_t storedWiFiMode = WiFi.getMode();
    WiFi.mode(WIFI_OFF);
    WiFi.mode(storedWiFiMode);
    yield();
    
    #else
    // Disable static IP so that we can connect to other servers via DHCP (DHCP is slower but required for connecting to more than one server, it seems (possible bug?)).
    disableStaticIP();
    verboseModePrint(F("\nConnecting to a different network. Static IP deactivated to make this possible."));
    
    #endif
  }
  lastSSID = targetSSID;
  
  verboseModePrint(F("Connecting... "), false);
  initiateConnectionToAP(targetSSID, targetChannel, targetBSSID);

  int attemptNumber = 1;
  ExpiringTimeTracker connectionAttemptTimeout([this](){ return _connectionAttemptTimeoutMs; });

  while((WiFi.status() == WL_DISCONNECTED) && !connectionAttemptTimeout)
  {
    if(connectionAttemptTimeout.elapsedTime() > attemptNumber * _connectionAttemptTimeoutMs) // _connectionAttemptTimeoutMs can be replaced (lowered) if you want to limit the time allowed for each connection attempt.
    {
      verboseModePrint(F("... "), false);
      WiFi.disconnect();
      yield();
      initiateConnectionToAP(targetSSID, targetChannel, targetBSSID);
      ++attemptNumber;
    }
    
    delay(1);
  }

  verboseModePrint(String(connectionAttemptTimeout.elapsedTime()));
  
  /* If the connection timed out */
  if (WiFi.status() != WL_CONNECTED)
  {
    verboseModePrint(F("Timeout"));
    return TransmissionStatusType::CONNECTION_FAILED;
  }

  return attemptDataTransfer();
}

TransmissionStatusType TcpIpMeshBackend::initiateTransmission(const TcpIpNetworkInfo &recipientInfo)
{
  WiFi.disconnect();
  yield();

  assert(!recipientInfo.SSID().isEmpty()); // We need at least SSID to connect
  String targetSSID = recipientInfo.SSID();
  int32_t targetWiFiChannel = recipientInfo.wifiChannel();
  uint8_t targetBSSID[6] {0};
  recipientInfo.getBSSID(targetBSSID);

  if(verboseMode()) // Avoid string generation if not required
  {
    printAPInfo(recipientInfo);
  }

  return connectToNode(targetSSID, targetWiFiChannel, targetBSSID);
}

void TcpIpMeshBackend::enterPostTransmissionState(const bool concludingDisconnect)
{
  if(WiFi.status() == WL_CONNECTED && staticIP != emptyIP && !staticIPActivated)
  {
    verboseModePrint(F("Reactivating static IP to allow for faster re-connects."));
    setStaticIP(staticIP);
  }

  // If we do not want to be connected at end of transmission, disconnect here so we can re-enable static IP first (above).
  if(concludingDisconnect)
  {
    WiFi.disconnect();
    yield();
  }
}

void TcpIpMeshBackend::attemptTransmission(const String &message, const bool scan, const bool scanAllWiFiChannels, const bool concludingDisconnect, const bool initialDisconnect)
{  
  MutexTracker mutexTracker(_tcpIpTransmissionMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! TCP/IP transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }
  
  if(initialDisconnect)
  {
    WiFi.disconnect();
    yield();
  }

  setMessage(message);

  latestTransmissionOutcomes().clear();
  
  if(WiFi.status() == WL_CONNECTED)
  {
    TransmissionStatusType transmissionResult = attemptDataTransfer();
    latestTransmissionOutcomes().push_back(TransmissionOutcome(constConnectionQueue().back(), transmissionResult));

    getTransmissionOutcomesUpdateHook()(*this);
  }
  else
  {
    if(scan)
    {
      connectionQueue().clear();
      scanForNetworks(scanAllWiFiChannels);
    }

    MutexTracker connectionQueueMutexTracker(_tcpIpConnectionQueueMutex);
    if(!connectionQueueMutexTracker.mutexCaptured())
    {
      assert(false && String(F("ERROR! connectionQueue locked. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    }
    else
    {
      for(const TcpIpNetworkInfo &currentNetwork : constConnectionQueue())
      {
        TransmissionStatusType transmissionResult = initiateTransmission(currentNetwork);
              
        latestTransmissionOutcomes().push_back(TransmissionOutcome{.origin = currentNetwork, .transmissionStatus = transmissionResult});

        if(!getTransmissionOutcomesUpdateHook()(*this))
          break;
      }
    }
  }
  
  enterPostTransmissionState(concludingDisconnect);
}

void TcpIpMeshBackend::attemptTransmission(const String &message, const bool scan, const bool scanAllWiFiChannels)
{
  attemptTransmission(message, scan, scanAllWiFiChannels, true, false);
}

TransmissionStatusType TcpIpMeshBackend::attemptTransmission(const String &message, const TcpIpNetworkInfo &recipientInfo, const bool concludingDisconnect, const bool initialDisconnect)
{  
  MutexTracker mutexTracker(_tcpIpTransmissionMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! TCP/IP transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."))); 
    return TransmissionStatusType::CONNECTION_FAILED;
  }

  TransmissionStatusType transmissionResult = TransmissionStatusType::CONNECTION_FAILED;
  setTemporaryMessage(message);
  
  if(initialDisconnect)
  {
    WiFi.disconnect();
    yield();
  }

  if(WiFi.status() == WL_CONNECTED && WiFi.SSID() == recipientInfo.SSID())
  {
    transmissionResult = attemptDataTransfer();
  }
  else
  {
    transmissionResult = initiateTransmission(recipientInfo);
  }
  
  enterPostTransmissionState(concludingDisconnect);
  clearTemporaryMessage();
  
  return transmissionResult;
}

void TcpIpMeshBackend::acceptRequests()
{
  MutexTracker mutexTracker(_tcpIpTransmissionMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! TCP/IP transmission in progress. Don't call acceptRequests from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }
  
  while (true) {
    WiFiClient _client = _server.accept();
    
    if (!_client)
      break;

    if (!waitForClientTransmission(_client, _apModeTimeoutMs) || !_client.available()) {
      continue;
    }
    
    /* Read in request and pass it to the supplied requestHandler */
    String request = _client.readStringUntil('\r');
    yield();
    _client.flush();
    
    String response = getRequestHandler()(request, *this);

    /* Send the response back to the client */
    if (_client.connected())
    {
      verboseModePrint(String(F("Responding")));
      _client.print(response + '\r');
      _client.flush();
      yield();
    }
  }
}
