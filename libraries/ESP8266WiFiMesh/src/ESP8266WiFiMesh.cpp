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

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <WiFiServer.h>

#include "ESP8266WiFiMesh.h"

#define SERVER_IP_ADDR      "192.168.4.1"

const IPAddress ESP8266WiFiMesh::empty_IP = IPAddress();
const uint32_t ESP8266WiFiMesh::lwip_version_203_signature[3] {2,0,3};

String ESP8266WiFiMesh::last_ssid = "";
bool ESP8266WiFiMesh::static_IP_activated = false;

// IP needs to be at the same subnet as server gateway (192.168.4 in this case). Station gateway ip must match ip for server.
IPAddress ESP8266WiFiMesh::static_IP = empty_IP;
IPAddress ESP8266WiFiMesh::gateway = IPAddress(192,168,4,1);
IPAddress ESP8266WiFiMesh::subnet_mask = IPAddress(255,255,255,0);

std::vector<NetworkInfo> ESP8266WiFiMesh::connection_queue = {};
std::vector<TransmissionResult> ESP8266WiFiMesh::latest_transmission_outcomes = {};

ESP8266WiFiMesh::ESP8266WiFiMesh(std::function<String(String, ESP8266WiFiMesh *)> requestHandler, std::function<transmission_status_t(String, ESP8266WiFiMesh *)> responseHandler, 
                                 std::function<void(int, ESP8266WiFiMesh *)> networkFilter, String mesh_password, String mesh_name, String node_id, bool verbose_mode, uint8 mesh_wifi_channel, int server_port) 
                                 : _server(server_port), _lwip_version{0, 0, 0}
{
  updateNetworkNames(mesh_name, (node_id != "" ? node_id : Uint64ToString(ESP.getChipId())));
  _requestHandler = requestHandler;
  _responseHandler = responseHandler;
  _mesh_wifi_channel = mesh_wifi_channel;
  _server_port = server_port;
  _mesh_password = mesh_password;
  _verbose_mode = verbose_mode;
  _networkFilter = networkFilter;
  storeLwipVersion();
}

void ESP8266WiFiMesh::verboseModePrint(String string_to_print, bool newline)
{
  if(_verbose_mode)
  {
    if(newline)
      Serial.println(string_to_print);
    else
      Serial.print(string_to_print);
  }
}

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating ssid strings containing controversial words. 
 * 
 * @param number The number to convert to a string with radix "base".
 * @param base The radix to convert "number" into. Must be between 2 and 36.
 * @returns A string of "number" encoded in radix "base".
 */
String ESP8266WiFiMesh::Uint64ToString(uint64_t number, byte base)
{
  String result = "";

  while(number > 0)
  {
    result = String((uint32_t)(number % base), base) + result;
    number /= base;
  }

  return (result == "" ? "0" : result);
}

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating ssid strings containing controversial words. 
 * 
 * @param string The string to convert to uint64_t. String must use radix "base".
 * @param base The radix of "string". Must be between 2 and 36.
 * @returns A uint64_t of the string, using radix "base" during decoding.
 */
uint64_t ESP8266WiFiMesh::StringToUint64(String string, byte base)
{
  uint64_t result = 0;

  char current_character[1];
  for(uint32_t i = 0; i < string.length(); i++)
  {
    result *= base;
    current_character[0] = string.charAt(i);
    result += strtoul(current_character, NULL, base);
  }
  
  return result;
}

/**
 * Calculate the current lwIP version number and store the numbers in the _lwip_version array.
 * lwIP version can be changed in the "Tools" menu of Arduino IDE.
 */
void ESP8266WiFiMesh::storeLwipVersion()
{
  // ESP.getFullVersion() looks something like: 
  // SDK:2.2.1(cfd48f3)/Core:win-2.5.0-dev/lwIP:2.0.3(STABLE-2_0_3_RELEASE/glue:arduino-2.4.1-10-g0c0d8c2)/BearSSL:94e9704
  String full_version = ESP.getFullVersion();

  int i = full_version.indexOf("lwIP:") + 5;
  char current_char = full_version.charAt(i);

  for(int version_part = 0; version_part < 3; version_part++)
  {
    while(!isdigit(current_char))
    {
      current_char = full_version.charAt(++i);
    }
    while(isdigit(current_char))
    {
      _lwip_version[version_part] = 10 * _lwip_version[version_part] + (current_char - '0'); // Left shift and add digit value, in base 10.
      current_char = full_version.charAt(++i);
    }
  }
}

/**
 * Check if the code is running on a version of lwIP that is at least min_lwip_version.
 */
bool ESP8266WiFiMesh::atLeastLwipVersion(const uint32_t min_lwip_version[3])
{ 
  for(int version_part = 0; version_part < 3; version_part++)
  {
    if(_lwip_version[version_part] > min_lwip_version[version_part])
      return true;
    else if(_lwip_version[version_part] < min_lwip_version[version_part])
      return false;
  }

  return true;
}

void ESP8266WiFiMesh::updateNetworkNames(String new_mesh_name, String new_node_id)
{
  _mesh_name = new_mesh_name;
  _node_id = new_node_id;
  _ssid = _mesh_name + _node_id;
}

void ESP8266WiFiMesh::setStaticIP(IPAddress new_IP)
{
  // Comment out the line below to remove static IP and use DHCP instead. 
  // DHCP makes WiFi connection happen slower, but there is no need to care about manually giving different IPs to the nodes and less need to worry about used IPs giving "Server unavailable" issues. 
  // Static IP has faster connection times (50 % of DHCP) and will make sending of data to a node that is already transmitting data happen more reliably. 
  // Note that after WiFi.config(static_IP, gateway, subnet_mask) is used, static IP will always be active, even for new connections, unless WiFi.config(0u,0u,0u); is called.
  WiFi.config(new_IP, gateway, subnet_mask); 
  static_IP_activated = true;
  static_IP = new_IP;
}

IPAddress ESP8266WiFiMesh::getStaticIP()
{
  if(static_IP_activated)
    return static_IP;

  return empty_IP;
}

void ESP8266WiFiMesh::disableStaticIP()
{
  WiFi.config(0u,0u,0u);
  yield();
  static_IP_activated = false;
}

void ESP8266WiFiMesh::begin()
{
  ////////////////////////////<DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////
  if(_handler != NULL)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( _ssid.c_str() );
    _server.begin();
  }
  else
  {
  ////////////////////////////</DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////
    WiFi.mode(WIFI_AP_STA);
    
    #ifdef ENABLE_STATIC_IP_OPTIMIZATION
    if(atLeastLwipVersion(lwip_version_203_signature))
    {
      verboseModePrint("lwIP version is at least 2.0.3. Static ip optimizations enabled.\n");
    }
    else
    {
      verboseModePrint("lwIP version is less than 2.0.3. Static ip optimizations disabled.\n");
    }
    #endif
  }
}

void ESP8266WiFiMesh::activateAP(String new_mesh_name, String new_node_id)
{
  if(new_mesh_name != "" || new_node_id != "")
    updateNetworkNames(new_mesh_name != "" ? new_mesh_name : _mesh_name, new_node_id  != "" ? new_node_id : _node_id);
    
  WiFi.softAP( _ssid.c_str(), _mesh_password.c_str(), _mesh_wifi_channel ); // Note that a maximum of 5 stations can be connected at a time to each AP
  _server.begin(); // Actually calls _server.stop()/_server.close() first.
}

void ESP8266WiFiMesh::deactivateAP(String new_mesh_name, String new_node_id)
{
  _server.stop();
  WiFi.softAPdisconnect();

  if(new_mesh_name != "" || new_node_id != "")
    updateNetworkNames(new_mesh_name != "" ? new_mesh_name : _mesh_name, new_node_id  != "" ? new_node_id : _node_id);
}

void ESP8266WiFiMesh::restartAP(String new_mesh_name, String new_node_id)
{
  deactivateAP(new_mesh_name, new_node_id);
  activateAP();
}

String ESP8266WiFiMesh::getMeshName() {return _mesh_name;}
String ESP8266WiFiMesh::getNodeID() {return _node_id;}

String ESP8266WiFiMesh::getMessage() {return _message;}
void ESP8266WiFiMesh::setMessage(String new_message) {_message = new_message;}

std::function<void(int, ESP8266WiFiMesh *)> ESP8266WiFiMesh::getNetworkFilter() {return _networkFilter;}
void ESP8266WiFiMesh::setNetworkFilter(std::function<void(int, ESP8266WiFiMesh *)> networkFilter) {_networkFilter = networkFilter;}

/**
 * Disconnect completely from a network.
 */
void ESP8266WiFiMesh::fullStop(WiFiClient curr_client)
{
  curr_client.stop();
  yield();
  WiFi.disconnect();
  yield();
}

/**
 * Wait for a WiFiClient to transmit
 *
 * @returns: True if the client is ready, false otherwise.
 * 
 */
bool ESP8266WiFiMesh::waitForClientTransmission(WiFiClient curr_client, int max_wait)
{
  int wait = max_wait;
  while(curr_client.connected() && !curr_client.available() && wait--)
    delay(3);

  /* Return false if the client isn't ready to communicate */
  if (WiFi.status() == WL_DISCONNECTED && !curr_client.available())
  {
    verboseModePrint("Disconnected!"); 
    return false;
  }
  
  return true;
}

/**
 * Send the mesh instance's current message then read back the other node's response
 * and pass that to the user-supplied responseHandler.
 *
 * @param curr_client The client to which the message should be transmitted.
 * @returns: A status code based on the outcome of the exchange.
 * 
 */
transmission_status_t ESP8266WiFiMesh::exchangeInfo(WiFiClient curr_client)
{
  verboseModePrint("Transmitting");
    
  curr_client.print(getMessage() + "\r");
  yield();

  if (!waitForClientTransmission(curr_client, 1000))
  {
    fullStop(curr_client);
    return TS_CONNECTION_FAILED;
  }

  if (!curr_client.available()) 
  {
    verboseModePrint("No response!");
    return TS_TRANSMISSION_FAILED; // WiFi.status() != WL_DISCONNECTED so we do not want to use fullStop(curr_client) here since that would force the node to scan for WiFi networks.
  }

  String response = curr_client.readStringUntil('\r');
  yield();
  curr_client.flush();

  /* Pass data to user callback */
  return _responseHandler(response, this);
}

/**
 * Handle data transfer process with a connected AP.
 *
 * @returns: A status code based on the outcome of the data transfer attempt.
 */
transmission_status_t ESP8266WiFiMesh::attemptDataTransfer()
{
  // Unlike WiFi.mode(WIFI_AP);, WiFi.mode(WIFI_AP_STA); allows us to stay connected to the AP we connected to in STA mode, at the same time as we can receive connections from other stations. 
  // We cannot send data to the AP in STA_AP mode though, that requires STA mode. 
  // Switching to STA mode will disconnect all stations connected to the node AP (though they can request a reconnect even while we are in STA mode).
  WiFi.mode(WIFI_STA);
  delay(1);
  transmission_status_t transmission_outcome = attemptDataTransferKernel();
  WiFi.mode(WIFI_AP_STA); 
  delay(1);
  
  return transmission_outcome;
}

/**
 * Helper function that contains the core functionality for the data transfer process with a connected AP.
 *
 * @returns: A status code based on the outcome of the data transfer attempt.
 */
transmission_status_t ESP8266WiFiMesh::attemptDataTransferKernel()
{
  WiFiClient curr_client;
  
  /* Connect to the node's server */
  if (!curr_client.connect(SERVER_IP_ADDR, _server_port)) 
  {
    fullStop(curr_client);
    verboseModePrint("Server unavailable");
    return TS_CONNECTION_FAILED;
  }  

  transmission_status_t transmission_outcome = exchangeInfo(curr_client);
  if (transmission_outcome <= 0)
  {
    verboseModePrint("Transmission failed during exchangeInfo.");
    return transmission_outcome;
  }
  
  curr_client.stop();
  yield();

  return transmission_outcome;
}

void ESP8266WiFiMesh::initiateConnectionToAP(String target_ssid, int target_channel, uint8_t *target_bssid)
{
  if(target_channel == NETWORK_INFO_DEFAULT_INT)
    WiFi.begin( target_ssid.c_str(), _mesh_password.c_str() ); // Without giving channel and bssid, connection time is longer.
  else if(target_bssid == NULL)
    WiFi.begin( target_ssid.c_str(), _mesh_password.c_str(), target_channel ); // Without giving channel and bssid, connection time is longer.
  else
    WiFi.begin( target_ssid.c_str(), _mesh_password.c_str(), target_channel, target_bssid );
}

/**
 * Connect to the AP at ssid and transmit the mesh instance's current message.
 *
 * @param target_ssid The name of the AP the other node has set up.
 * @param target_channel The WiFI channel of the AP the other node has set up.
 * @param target_bssid The mac address of the AP the other node has set up.
 * @returns: A status code based on the outcome of the connection and data transfer process.
 * 
 */
transmission_status_t ESP8266WiFiMesh::connectToNode(String target_ssid, int target_channel, uint8_t *target_bssid)
{
  if(static_IP_activated && last_ssid != "" && last_ssid != target_ssid) // So we only do this once per connection, in case there is a performance impact.
  {
    #ifdef ENABLE_STATIC_IP_OPTIMIZATION
    if(atLeastLwipVersion(lwip_version_203_signature))
    {
      // Can be used with Arduino core for ESP8266 version 2.4.2 or higher with lwIP2 enabled to keep static IP on even during network switches.
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_AP_STA);
      yield();
    }
    else
    {
      // Disable static IP so that we can connect to other servers via DHCP (DHCP is slower but required for connecting to more than one server, it seems (possible bug?)).
      disableStaticIP();
      verboseModePrint("\nConnecting to a different network. Static IP deactivated to make this possible.");
    }
    #else
    // Disable static IP so that we can connect to other servers via DHCP (DHCP is slower but required for connecting to more than one server, it seems (possible bug?)).
    disableStaticIP();
    verboseModePrint("\nConnecting to a different network. Static IP deactivated to make this possible.");
    #endif
  }
  last_ssid = target_ssid;
  
  verboseModePrint("Connecting... ", false);
  initiateConnectionToAP(target_ssid, target_channel, target_bssid);

  int connection_start_time = millis();
  int attempt_number = 1;

  int waiting_time = millis() - connection_start_time;
  while((WiFi.status() == WL_DISCONNECTED) && waiting_time <= 10000)
  {
    if(waiting_time > attempt_number * 10000) // 10000 can be lowered if you want to limit the time allowed for each connection attempt.
    {
      verboseModePrint("... ", false);
      WiFi.disconnect();
      yield();
      initiateConnectionToAP(target_ssid, target_channel, target_bssid);
      attempt_number++;
    }
    delay(1);
    waiting_time = millis() - connection_start_time;
  }

  verboseModePrint(String(waiting_time));
  
  /* If the connection timed out */
  if (WiFi.status() != WL_CONNECTED)
  {
    verboseModePrint("Timeout");
    return TS_CONNECTION_FAILED;
  }

  return attemptDataTransfer();
}

void ESP8266WiFiMesh::attemptTransmission(String message, bool concluding_disconnect, bool initial_disconnect, bool no_scan)
{
  setMessage(message);
  
  if(initial_disconnect)
  {
    WiFi.disconnect();
    yield();
  }

  latest_transmission_outcomes.clear();
    
  if(WiFi.status() == WL_CONNECTED)
  {
    transmission_status_t transmission_result = attemptDataTransfer();
    latest_transmission_outcomes.push_back(TransmissionResult(connection_queue.back(), transmission_result));
  }
  else
  {
    if(!no_scan)
    {
      verboseModePrint("Scanning... ", false);
      
      /* Scan for APs */
      connection_queue.clear();

      // If Arduino core for ESP8266 version < 2.4.2 scanning will cause the WiFi radio to cycle through all WiFi channels.
      // This means existing WiFi connections are likely to break or work poorly if done frequently.
      #ifdef ENABLE_WIFI_SCAN_OPTIMIZATION
      // Scan function argument overview: scanNetworks(bool async = false, bool show_hidden = false, uint8 channel = 0, uint8* ssid = NULL)
      int n = WiFi.scanNetworks(false, false, _mesh_wifi_channel);
      #else
      int n = WiFi.scanNetworks();
      #endif
      
      _networkFilter(n, this); // Update the connection_queue.
    }
    
    for(NetworkInfo &current_network : connection_queue)
    {
      WiFi.disconnect();
      yield();

      String current_ssid = "";
      int current_wifi_channel = NETWORK_INFO_DEFAULT_INT;
      uint8_t *current_bssid = NULL;

      // If an SSID has been assigned, it is prioritized over an assigned network_index since the network_index is more likely to change.
      if(current_network.ssid != "")
      {
        current_ssid = current_network.ssid;
        current_wifi_channel = current_network.wifi_channel;
        current_bssid = current_network.bssid;
      }
      else // Use only network_index
      {
        current_ssid = WiFi.SSID(current_network.network_index);
        current_wifi_channel = WiFi.channel(current_network.network_index);
        current_bssid = WiFi.BSSID(current_network.network_index);
      }

      if(_verbose_mode) // Avoid string generation if not required
      {
        verboseModePrint("AP acquired: " + current_ssid + ", Ch:" + String(current_wifi_channel) + " ", false);

        if(current_network.network_index != NETWORK_INFO_DEFAULT_INT)
        {
          verboseModePrint("(" + String(WiFi.RSSI(current_network.network_index)) + "dBm) " + 
                           (WiFi.encryptionType(current_network.network_index) == ENC_TYPE_NONE ? "open" : ""), false);
        }

        verboseModePrint("... ", false);
      }

      transmission_status_t transmission_result = connectToNode(current_ssid, current_wifi_channel, current_bssid);

      latest_transmission_outcomes.push_back(TransmissionResult{.origin = current_network, .transmission_status = transmission_result});
    }
  }

  if(WiFi.status() == WL_CONNECTED && static_IP != empty_IP && !static_IP_activated)
  {
    verboseModePrint("Reactivating static IP to allow for faster re-connects.");
    setStaticIP(static_IP);
  }

  // If we do not want to be connected at end of transmission, disconnect here so we can re-enable static IP first (above).
  if(concluding_disconnect)
  {
    WiFi.disconnect();
    yield();
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

      if (!waitForClient(_client, 1500)) {
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

      if (!waitForClientTransmission(_client, 1500) || !_client.available()) {
        continue;
      }

      /* Read in request and pass it to the supplied requestHandler */
      String request = _client.readStringUntil('\r');
      yield();
      _client.flush();

      String response = _requestHandler(request, this);

      /* Send the response back to the client */
      if (_client.connected())
      {
        verboseModePrint("Responding");
        _client.print(response + "\r");
        _client.flush();
        yield();
      }
    }
  }
}
