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

#define SERVER_IP_ADDR      "192.168.4.1"

const IPAddress TcpIpMeshBackend::emptyIP = IPAddress();

bool TcpIpMeshBackend::_tcpIpTransmissionMutex = false;

String TcpIpMeshBackend::lastSSID = "";
bool TcpIpMeshBackend::staticIPActivated = false;

// IP needs to be at the same subnet as server gateway (192.168.4 in this case). Station gateway ip must match ip for server.
IPAddress TcpIpMeshBackend::staticIP = emptyIP;
IPAddress TcpIpMeshBackend::gateway = IPAddress(192,168,4,1);
IPAddress TcpIpMeshBackend::subnetMask = IPAddress(255,255,255,0);

TcpIpMeshBackend::TcpIpMeshBackend(requestHandlerType requestHandler, responseHandlerType responseHandler, 
                                   networkFilterType networkFilter, const String &meshPassword, const String &ssidPrefix, 
                                   const String &ssidSuffix, bool verboseMode, uint8 meshWiFiChannel, uint16_t serverPort) 
                                   : MeshBackendBase(requestHandler, responseHandler, networkFilter, MB_TCP_IP), _server(serverPort)
{
  setSSID(ssidPrefix, "", ssidSuffix);
  setMeshPassword(meshPassword);
  setVerboseModeState(verboseMode);
  setWiFiChannel(meshWiFiChannel);
  setServerPort(serverPort);
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

bool TcpIpMeshBackend::transmissionInProgress(){return _tcpIpTransmissionMutex;}

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

IPAddress TcpIpMeshBackend::getStaticIP()
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

void TcpIpMeshBackend::setServerPort(uint16_t serverPort)
{
  _serverPort = serverPort;
  
  // Apply changes to active AP.
  if(isAPController())
    restartAP();
}

uint16_t TcpIpMeshBackend::getServerPort() {return _serverPort;}

void TcpIpMeshBackend::setMaxAPStations(uint8_t maxAPStations)
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

bool TcpIpMeshBackend::getMaxAPStations() {return _maxAPStations;}

void TcpIpMeshBackend::setConnectionAttemptTimeout(int32_t connectionAttemptTimeoutMs)
{
  _connectionAttemptTimeoutMs = connectionAttemptTimeoutMs;
}

int32_t TcpIpMeshBackend::getConnectionAttemptTimeout() {return _connectionAttemptTimeoutMs;}

void TcpIpMeshBackend::setStationModeTimeout(int stationModeTimeoutMs)
{
  _stationModeTimeoutMs = stationModeTimeoutMs;
}

int TcpIpMeshBackend::getStationModeTimeout() {return _stationModeTimeoutMs;}

void TcpIpMeshBackend::setAPModeTimeout(uint32_t apModeTimeoutMs)
{
  _apModeTimeoutMs = apModeTimeoutMs;
}

uint32_t TcpIpMeshBackend::getAPModeTimeout() {return _apModeTimeoutMs;}

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
bool TcpIpMeshBackend::waitForClientTransmission(WiFiClient &currClient, uint32_t maxWait)
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
 * @return A status code based on the outcome of the exchange.
 * 
 */
transmission_status_t TcpIpMeshBackend::exchangeInfo(WiFiClient &currClient)
{
  verboseModePrint("Transmitting");  // Not storing strings in flash (via F()) to avoid performance impacts when using the string.
    
  currClient.print(getMessage() + "\r");
  yield();

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
transmission_status_t TcpIpMeshBackend::attemptDataTransfer()
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
 * @return A status code based on the outcome of the data transfer attempt.
 */
transmission_status_t TcpIpMeshBackend::attemptDataTransferKernel()
{
  WiFiClient currClient;
  currClient.setTimeout(_stationModeTimeoutMs);

  /* Connect to the node's server */
  if (!currClient.connect(SERVER_IP_ADDR, getServerPort())) 
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
  yield();

  return transmissionOutcome;
}

void TcpIpMeshBackend::initiateConnectionToAP(const String &targetSSID, int targetChannel, uint8_t *targetBSSID)
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
transmission_status_t TcpIpMeshBackend::connectToNode(const String &targetSSID, int targetChannel, uint8_t *targetBSSID)
{  
  if(staticIPActivated && lastSSID != "" && lastSSID != targetSSID) // So we only do this once per connection, in case there is a performance impact.
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

  int connectionStartTime = millis();
  int attemptNumber = 1;

  int waitingTime = millis() - connectionStartTime;
  while((WiFi.status() == WL_DISCONNECTED) && waitingTime <= _connectionAttemptTimeoutMs)
  {
    if(waitingTime > attemptNumber * _connectionAttemptTimeoutMs) // _connectionAttemptTimeoutMs can be replaced (lowered) if you want to limit the time allowed for each connection attempt.
    {
      verboseModePrint(F("... "), false);
      WiFi.disconnect();
      yield();
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

void TcpIpMeshBackend::attemptTransmission(const String &message, bool scan, bool scanAllWiFiChannels, bool concludingDisconnect, bool initialDisconnect )
{  
  MutexTracker mutexTracker(_tcpIpTransmissionMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && "ERROR! TCP/IP transmission in progress. Don't call attemptTransmission from callbacks as this may corrupt program state! Aborting."); 
    return;
  }
  
  if(initialDisconnect)
  {
    WiFi.disconnect();
    yield();
  }

  setMessage(message);

  latestTransmissionOutcomes.clear();
  
  if(WiFi.status() == WL_CONNECTED)
  {
    transmission_status_t transmissionResult = attemptDataTransfer();
    latestTransmissionOutcomes.push_back(TransmissionResult(connectionQueue.back(), transmissionResult));
  }
  else
  {
    if(scan)
    {
      scanForNetworks(scanAllWiFiChannels);
    }

    for(NetworkInfo &currentNetwork : connectionQueue)
    {
      WiFi.disconnect();
      yield();

      String currentSSID = "";
      int currentWiFiChannel = NETWORK_INFO_DEFAULT_INT;
      uint8_t *currentBSSID = NULL;

      // If an SSID has been assigned, it is prioritized over an assigned networkIndex since the networkIndex is more likely to change.
      if(currentNetwork.SSID != "")
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

      if(verboseMode()) // Avoid string generation if not required
      {
        printAPInfo(currentNetwork.networkIndex, currentSSID, currentWiFiChannel);
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
    yield();
  }
}

void TcpIpMeshBackend::attemptTransmission(const String &message, bool scan, bool scanAllWiFiChannels)
{
  attemptTransmission(message, scan, scanAllWiFiChannels, true, false);
}

void TcpIpMeshBackend::acceptRequest()
{
  MutexTracker mutexTracker(_tcpIpTransmissionMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && "ERROR! TCP/IP transmission in progress. Don't call acceptRequest from TCP/IP callbacks as this may corrupt program state! Aborting."); 
    return;
  }
  
  while (true) {
    WiFiClient _client = _server.available();
    
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
      verboseModePrint("Responding");  // Not storing strings in flash (via F()) to avoid performance impacts when using the string.
      _client.print(response + "\r");
      _client.flush();
      yield();
    }
  }
}
