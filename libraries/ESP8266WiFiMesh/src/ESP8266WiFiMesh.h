/*
  ESP8266WiFiMesh.h - Mesh network node
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

#ifndef __WIFIMESH_H__
#define __WIFIMESH_H__

#include <WiFiClient.h> 
#include <WiFiServer.h>
#include <functional>
#include <vector>
#include "NetworkInfo.h"
#include "TransmissionResult.h"

#define ENABLE_STATIC_IP_OPTIMIZATION // Requires Arduino core for ESP8266 version 2.4.2 or higher and lwIP2 (lwIP can be changed in "Tools" menu of Arduino IDE).
#define ENABLE_WIFI_SCAN_OPTIMIZATION // Requires Arduino core for ESP8266 version 2.4.2 or higher. Scan time should go from about 2100 ms to around 60 ms if channel 1 (standard) is used.

class ESP8266WiFiMesh {

private:
  String _ssid;
  String _mesh_name;
  String _node_id;
  int _server_port;
  String _mesh_password;
  uint8 _mesh_wifi_channel;
  bool _verbose_mode;
  WiFiServer  _server;
  uint32_t _lwip_version[3];
  static const uint32_t lwip_version_203_signature[3];
  String _message = "";

  static String last_ssid;
  static bool static_IP_activated;
  static IPAddress static_IP;
  static IPAddress gateway;
  static IPAddress subnet_mask;

  std::function<String(String, ESP8266WiFiMesh *)> _requestHandler;
  std::function<transmission_status_t(String, ESP8266WiFiMesh *)> _responseHandler;
  std::function<void(int, ESP8266WiFiMesh *)> _networkFilter;

  void updateNetworkNames(String new_mesh_name, String new_node_id);
  void verboseModePrint(String string_to_print, bool newline = true);
  void fullStop(WiFiClient curr_client);
  void initiateConnectionToAP(String target_ssid, int target_channel = NETWORK_INFO_DEFAULT_INT, uint8_t *target_bssid = NULL);
  transmission_status_t connectToNode(String target_ssid, int target_channel = NETWORK_INFO_DEFAULT_INT, uint8_t *target_bssid = NULL);
  transmission_status_t exchangeInfo(WiFiClient curr_client);
  bool waitForClientTransmission(WiFiClient curr_client, int max_wait);
  transmission_status_t attemptDataTransfer();
  transmission_status_t attemptDataTransferKernel();
  void storeLwipVersion();
  bool atLeastLwipVersion(const uint32_t min_lwip_version[3]);
  
public:

  /**
   * WiFiMesh Constructor method. Creates a WiFi Mesh Node, ready to be initialised.
   *
   * @param requestHandler The callback handler for dealing with received requests. Takes a string as an argument which
   *          is the request string received from another node and returns the string to send back.
   * @param responseHandler The callback handler for dealing with received responses. Takes a string as an argument which
   *          is the response string received from another node. Returns a transmission status code as a transmission_status_t.
   * @param networkFilter The callback handler for deciding which WiFi networks to connect to.
   * @param mesh_password The WiFi password for the mesh network.
   * @param mesh_name The name of the mesh network. Used as prefix for the node SSID and to find other network nodes in the example network filter function.
   * @param node_id The id for this mesh node. Used as suffix for the node SSID. If set to "", the id will default to ESP.getChipId().
   * @param verbose_mode Determines if we should print the events occurring in the library to Serial. Off by default.
   * @param mesh_wifi_channel The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
   *                    WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
   *                    This can cause problems if several ESP8266WiFiMesh instances exist on the same ESP8266 and use different WiFi channels. 
   *                    In such a case, whenever the station of one ESP8266WiFiMesh instance connects to an AP, it will silently force the 
   *                    WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
   *                    make it impossible for other stations to detect the APs whose WiFi channels have changed.
   * @param server_port The server port used by the AP of the ESP8266WiFiMesh instance. If multiple APs exist on a single ESP8266, each requires a separate server port. 
   *              If two AP:s on the same ESP8266 are using the same server port, you must call deactivateAP on the active AP before calling activateAP on the inactive AP.                   
   * 
   */
  ESP8266WiFiMesh(std::function<String(String, ESP8266WiFiMesh *)> requestHandler, std::function<transmission_status_t(String, ESP8266WiFiMesh *)> responseHandler, 
                  std::function<void(int, ESP8266WiFiMesh *)> networkFilter, String mesh_password, String mesh_name = "Mesh_Node", String node_id = "", 
                  bool verbose_mode = false, uint8 mesh_wifi_channel = 1, int server_port = 4011);
  
  /** 
  * A vector that contains the WiFi-scan network indicies to connect to. 
  * The connection_queue vector is cleared before each new scan and filled via the networkFilter callback function once the scan completes.
  * WiFi connections will start with connection_queue[0] and then incrementally proceed to higher vector positions. 
  * Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
  */
  static std::vector<NetworkInfo> connection_queue;

  /** 
  * A vector with the TransmissionResult for each AP to which a transmission was attempted during the latest attemptTransmission call.
  * The latest_transmission_outcomes vector is cleared before each new transmission attempt.
  * Connection attempts are indexed in the same order they were attempted.
  * Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
  */
  static std::vector<TransmissionResult> latest_transmission_outcomes;

  /**
   * Initialises the node.
   */
  void begin();

  /**
   * Each AP requires a separate server port. If two AP:s are using the same server port, you must call deactivateAP on the active AP before calling activateAP on the inactive AP.
   */
  void activateAP(String new_mesh_name = "", String new_node_id = "");
  void deactivateAP(String new_mesh_name = "", String new_node_id = "");
  void restartAP(String new_mesh_name = "", String new_node_id = "");

  String getMeshName();
  String getNodeID();

  String getMessage();
  void setMessage(String new_message);

  /**
   * If AP connection already exists, send message only to this AP.
   * Otherwise, scan for other networks, send the scan result to networkFilter and then transmit the message to the networks found in connection_queue.
   *
   * @param message The message to send to other nodes. It will be stored in the class instance until replaced via attemptTransmission or setMessage.
   * @param concluding_disconnect Disconnect from AP once transmission is complete.
   * @param initial_disconnect Disconnect from any currently connected AP before attempting transmission.
   * @param no_scan Do not scan for new networks and do not call networkFilter function. Will only use the data already in connection_queue for the transmission.
   */
  void attemptTransmission(String message, bool concluding_disconnect = true, bool initial_disconnect = false, bool no_scan = false);

  /**
   * If any clients are connected, accept their requests and call the requestHandler function for each one.
   */
  void acceptRequest();

  /**
   * Set a static IP address for the node and activate use of static IP.
   * The static IP needs to be at the same subnet as the server's gateway.
   */
  void setStaticIP(IPAddress new_IP);
  
  IPAddress getStaticIP();
  void disableStaticIP();

  /**
   * An empty IPAddress. Used as default when no IP is set.
   */
  static const IPAddress empty_IP;

  static String Uint64ToString(uint64_t number, byte base = 16);
  static uint64_t StringToUint64(String string, byte base = 16);

  std::function<void(int, ESP8266WiFiMesh *)> getNetworkFilter();
  void setNetworkFilter(std::function<void(int, ESP8266WiFiMesh *)> networkFilter);
};

#endif
