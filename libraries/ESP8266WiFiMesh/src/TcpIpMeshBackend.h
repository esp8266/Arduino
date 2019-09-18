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

// ESP-NOW is faster for small data payloads (up to a few kB, split over multiple messages). Transfer of up to 234 bytes takes 4 ms.
// In general ESP-NOW transfer time can be approximated with the following function: transferTime = ceil(bytesToTransfer / 234.0)*3 ms.
// If you only transfer 234 bytes at a time, this adds up to around 56kB/s. Finally a chance to relive the glory of the olden days
// when people were restricted to V90 dial-up modems for internet access!
// TCP-IP takes longer to connect (around 1000 ms), and an AP has to disconnect all connected stations in order to transfer data to another AP, 
// but this backend has a much higher data transfer speed than ESP-NOW once connected (100x faster or so).

#ifndef __TCPIPMESHBACKEND_H__
#define __TCPIPMESHBACKEND_H__

#include <WiFiClient.h> 
#include <WiFiServer.h>
#include <functional>
#include <vector>
#include "MeshBackendBase.h"
#include "TcpIpNetworkInfo.h"

class TcpIpMeshBackend : public MeshBackendBase {

public:

  /**
   * WiFiMesh Constructor method. Creates a WiFi Mesh Node, ready to be initialised.
   *
   * @param requestHandler The callback handler for dealing with received requests. Takes a string as an argument which
   *          is the request string received from another node and returns the string to send back.
   * @param responseHandler The callback handler for dealing with received responses. Takes a string as an argument which
   *          is the response string received from another node. Returns a transmission status code as a transmission_status_t.
   * @param networkFilter The callback handler for deciding which WiFi networks to connect to.
   * @param meshPassword The WiFi password for the mesh network.
   * @param ssidPrefix The prefix (first part) of the node SSID.
   * @param ssidSuffix The suffix (last part) of the node SSID.
   * @param verboseMode Determines if we should print the events occurring in the library to Serial. Off by default. This setting is separate for each TcpIpMeshBackend instance.
   * @param meshWiFiChannel The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
   *                    WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
   *                    This can cause problems if several mesh instances exist on the same ESP8266 and use different WiFi channels. 
   *                    In such a case, whenever the station of one mesh instance connects to an AP, it will silently force the 
   *                    WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
   *                    make it impossible for other stations to detect the APs whose WiFi channels have changed.
   * @param serverPort The server port used both by the AP of the TcpIpMeshBackend instance and when the instance connects to other APs. 
   *                   If multiple APs exist on a single ESP8266, each requires a separate server port. 
   *                   If two AP:s on the same ESP8266 are using the same server port, they will not be able to have both server instances active at the same time.                  
   *                   This is managed automatically by the activateAP method.
   * 
   */
  TcpIpMeshBackend(requestHandlerType requestHandler, responseHandlerType responseHandler, networkFilterType networkFilter, 
                  const String &meshPassword, const String &ssidPrefix, const String &ssidSuffix, bool verboseMode = false, 
                  uint8 meshWiFiChannel = 1, uint16_t serverPort = 4011);

  /** 
  * Returns a vector that contains the NetworkInfo for each WiFi network to connect to.
  * This vector is unique for each mesh backend.
  * The connectionQueue vector is cleared before each new scan and filled via the networkFilter callback function once the scan completes.
  * WiFi connections will start with connectionQueue[0] and then incrementally proceed to higher vector positions. 
  * Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
  */
  std::vector<TcpIpNetworkInfo> & connectionQueue();

  /** 
  * Returns a vector with the TransmissionOutcome for each AP to which a transmission was attempted during the latest attemptTransmission call.
  * This vector is unique for each mesh backend.
  * The latestTransmissionOutcomes vector is cleared before each new transmission attempt.
  * Connection attempts are indexed in the same order they were attempted.
  * Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
  */
  std::vector<TransmissionOutcome> & latestTransmissionOutcomes() override;
  
  /**
   * Initialises the node.
   */
  void begin() override;

    /**
   * If AP connection already exists, and the initialDisconnect argument is set to false, send message only to the already connected AP.
   * Otherwise, scan for other networks, send the scan result to networkFilter and then transmit the message to the networks found in connectionQueue.
   *
   * @param message The message to send to other nodes. It will be stored in the class instance until replaced via attemptTransmission or setMessage.
   * @param concludingDisconnect Disconnect from AP once transmission is complete. Defaults to true.
   * @param initialDisconnect Disconnect from any currently connected AP before attempting transmission. Defaults to false.
   * @param scan Scan for new networks and call the networkFilter function with the scan results. When set to false, only the data already in connectionQueue will be used for the transmission.
   * @param scanAllWiFiChannels Scan all WiFi channels during a WiFi scan, instead of just the channel the MeshBackendBase instance is using.
   *                               Scanning all WiFi channels takes about 2100 ms, compared to just 60 ms if only channel 1 (standard) is scanned.
   *                               Note that if the ESP8266 has an active AP, that AP will switch WiFi channel to match that of any other AP the ESP8266 connects to.
   *                               This can make it impossible for other nodes to detect the AP if they are scanning the wrong WiFi channel.
   */
  void attemptTransmission(const String &message, bool scan, bool scanAllWiFiChannels, bool concludingDisconnect, bool initialDisconnect = false);

  void attemptTransmission(const String &message, bool scan = true, bool scanAllWiFiChannels = false) override;

  /**
   * Transmit message to a single recipient without changing the local transmission state (apart from connecting to the recipient if required). 
   * Will not change connectionQueue, latestTransmissionOutcomes or stored message.
   * 
   * Note that if wifiChannel and BSSID are missing from recipientInfo, connection time will be longer.
   */
  transmission_status_t attemptTransmission(const String &message, const TcpIpNetworkInfo &recipientInfo, bool concludingDisconnect = true, bool initialDisconnect = false);
  
  /**
   * If any clients are connected, accept their requests and call the requestHandler function for each one.
   */
  void acceptRequest();

  /**
   * Get the TCP/IP message that is currently scheduled for transmission.
   * Unlike the getMessage() method, this will be correct even when the single recipient attemptTransmission method is used.
   */
  String getCurrentMessage();

  /**
   * Set a static IP address for the ESP8266 and activate use of static IP.
   * The static IP needs to be at the same subnet as the server's gateway.
   */
  void setStaticIP(const IPAddress &newIP);
  IPAddress getStaticIP();
  void disableStaticIP();

  /**
   * An empty IPAddress. Used as default when no IP is set.
   */
  static const IPAddress emptyIP;

  /** 
   * Set the server port used both by the AP of the TcpIpMeshBackend instance and when the instance connects to other APs. 
   * If multiple APs exist on a single ESP8266, each requires a separate server port. 
   * If two AP:s on the same ESP8266 are using the same server port, they will not be able to have both server instances active at the same time.                  
   * This is managed automatically by the activateAP method.
   * Will also change the setting for the active AP (via an AP restart)
   * if this TcpIpMeshBackend instance is the current AP controller.
   *                    
   * @param serverPort The server port to use.
   *                    
   */
  void setServerPort(uint16_t serverPort);
  uint16_t getServerPort();

  /**
   * Set the maximum number of stations that can simultaneously be connected to the AP controlled by this TcpIpMeshBackend instance. 
   * This number is 4 by default.
   * Once the max number has been reached, any other station that wants to connect will be forced to wait until an already connected station disconnects.
   * The more stations that are connected, the more memory is required.
   * Will also change the setting for the active AP (via an AP restart)
   * if this TcpIpMeshBackend instance is the current AP controller.
   *
   * @param maxAPStations The maximum number of simultaneous station connections allowed. Valid values are 0 to 8.
   */
  void setMaxAPStations(uint8_t maxAPStations);
  bool getMaxAPStations();

  /**
   * Set the timeout for each attempt to connect to another AP that occurs through the attemptTransmission method by this TcpIpMeshBackend instance.
   * The timeout is 10 000 ms by default.
   * 
   * @param connectionAttemptTimeoutMs The timeout for each connection attempt, in milliseconds.
   */
  void setConnectionAttemptTimeout(int32_t connectionAttemptTimeoutMs);
  int32_t getConnectionAttemptTimeout();

  /**
   * Set the timeout to use for transmissions when this TcpIpMeshBackend instance acts as a station (i.e. when connected to another AP).
   * This will affect the timeout of the attemptTransmission method once a connection to an AP has been established.
   * The timeout is 5 000 ms by default.
   * 
   * @param stationModeTimeoutMs The timeout to use, in milliseconds.
   */
  void setStationModeTimeout(int stationModeTimeoutMs);
  int getStationModeTimeout();

  /**
   * Set the timeout to use for transmissions when this TcpIpMeshBackend instance acts as an AP (i.e. when receiving connections from other stations).
   * This will affect the timeout of the acceptRequest method.
   * The timeout is 4 500 ms by default.
   * Will also change the setting for the active AP (without an AP restart)
   * if this TcpIpMeshBackend instance is the current AP controller.
   *
   * @param apModeTimeoutMs The timeout to use, in milliseconds.
   */
  void setAPModeTimeout(uint32_t apModeTimeoutMs);
  uint32_t getAPModeTimeout();

protected:

  static std::vector<TcpIpNetworkInfo> _connectionQueue;
  static std::vector<TransmissionOutcome> _latestTransmissionOutcomes;

  /**
   * Called just before we activate the AP.
   * Put _server.stop() in deactivateAPHook() in case you use _server.begin() here.
   */
  void activateAPHook() override;

  /**
   * Called just before we deactivate the AP.
   * Put _server.stop() here in case you use _server.begin() in activateAPHook().
   */
  void deactivateAPHook() override;

  /** 
   * Will be true if a transmission initiated by a public method is in progress.
   */
  static bool _tcpIpTransmissionMutex;

  /**
   * Check if there is an ongoing TCP/IP transmission in the library. Used to avoid interrupting transmissions.
   * 
   * @return True if a transmission initiated by a public method is in progress.
   */
  static bool transmissionInProgress();

  /**
   * Set a message that will be sent to other nodes when calling attemptTransmission, instead of the regular getMessage(). 
   * This message is used until clearTemporaryMessage() is called.
   * 
   * @param newMessage The message to send.
   */
  void setTemporaryMessage(const String &newMessage);
  String getTemporaryMessage();
  void clearTemporaryMessage();

private:

  uint16_t _serverPort;
  WiFiServer _server;
  uint8_t _maxAPStations = 4; // Only affects TCP/IP connections, not ESP-NOW connections
  int32_t _connectionAttemptTimeoutMs = 10000;
  int _stationModeTimeoutMs = 5000; // int is the type used in the Arduino core for this particular API, not uint32_t, which is why we use int here.
  uint32_t _apModeTimeoutMs = 4500;

  static String _temporaryMessage;
  static String lastSSID;
  static bool staticIPActivated;
  bool useStaticIP;
  static IPAddress staticIP;
  static IPAddress gateway;
  static IPAddress subnetMask;

  void fullStop(WiFiClient &currClient);
  void initiateConnectionToAP(const String &targetSSID, int targetChannel = NETWORK_INFO_DEFAULT_INT, uint8_t *targetBSSID = NULL);
  transmission_status_t connectToNode(const String &targetSSID, int targetChannel = NETWORK_INFO_DEFAULT_INT, uint8_t *targetBSSID = NULL);
  transmission_status_t exchangeInfo(WiFiClient &currClient);
  bool waitForClientTransmission(WiFiClient &currClient, uint32_t maxWait);
  transmission_status_t attemptDataTransfer();
  transmission_status_t attemptDataTransferKernel();
  transmission_status_t initiateTransmission(const TcpIpNetworkInfo &recipientInfo);
  void enterPostTransmissionState(bool concludingDisconnect);
};

#endif
