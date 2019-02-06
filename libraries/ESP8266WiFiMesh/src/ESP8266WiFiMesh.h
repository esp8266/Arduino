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

const String WIFI_MESH_EMPTY_STRING = "";

class ESP8266WiFiMesh
{

private:
    String _SSID;
    String _meshName;
    String _nodeID;
    uint16_t _serverPort;
    String _meshPassword;
    uint8 _meshWiFiChannel;
    bool _verboseMode;
    WiFiServer _server;
    uint32_t _lwipVersion[3];
    static const uint32_t lwipVersion203Signature[3];
    String _message = WIFI_MESH_EMPTY_STRING;
    bool _scanHidden = false;
    bool _apHidden = false;
    uint8_t _maxAPStations = 4;
    int32_t _connectionAttemptTimeoutMs = 10000;
    int _stationModeTimeoutMs = 5000; // int is the type used in the Arduino core for this particular API, not uint32_t, which is why we use int here.
    uint32_t _apModeTimeoutMs = 4500;

    static String lastSSID;
    static bool staticIPActivated;
    static IPAddress staticIP;
    static IPAddress gateway;
    static IPAddress subnetMask;
    static ESP8266WiFiMesh *apController;

    typedef std::function<String(const String &, ESP8266WiFiMesh &)> requestHandlerType;
    typedef std::function<transmission_status_t(const String &, ESP8266WiFiMesh &)> responseHandlerType;
    typedef std::function<void(int, ESP8266WiFiMesh &)> networkFilterType;

    requestHandlerType _requestHandler;
    responseHandlerType _responseHandler;
    networkFilterType _networkFilter;

    void updateNetworkNames(const String &newMeshName = WIFI_MESH_EMPTY_STRING, const String &newNodeID = WIFI_MESH_EMPTY_STRING);
    void verboseModePrint(const String &stringToPrint, bool newline = true);
    void fullStop(WiFiClient &currClient);
    void initiateConnectionToAP(const String &targetSSID, int targetChannel = NETWORK_INFO_DEFAULT_INT, uint8_t *targetBSSID = NULL);
    transmission_status_t connectToNode(const String &targetSSID, int targetChannel = NETWORK_INFO_DEFAULT_INT, uint8_t *targetBSSID = NULL);
    transmission_status_t exchangeInfo(WiFiClient &currClient);
    bool waitForClientTransmission(WiFiClient &currClient, uint32_t maxWait);
    transmission_status_t attemptDataTransfer();
    transmission_status_t attemptDataTransferKernel();
    void storeLwipVersion();
    bool atLeastLwipVersion(const uint32_t minLwipVersion[3]);




    ////////////////////////////<DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////

    typedef std::function<String(String)> compatibilityLayerHandlerType;

    String _ssidPrefix;
    uint32_t _chipID;

    compatibilityLayerHandlerType _handler = NULL;

    WiFiClient  _client;

    void connectToNode(const String &targetSSID, const char *message);
    bool exchangeInfo(const char *message, WiFiClient &currClient);
    bool waitForClient(WiFiClient &currClient, int maxWait);
    void attemptScanKernel(const char *message);

    ////////////////////////////</DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////



public:

    ////////////////////////////<DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////

    /**
        WiFiMesh Constructor method. Creates a WiFi Mesh Node, ready to be initialised.

        @chipID A unique identifier number for the node.
        @handler The callback handler for dealing with received messages. Takes a string as an argument which
                is the string received from another node and returns the string to send back.

    */
    ESP8266WiFiMesh(uint32_t chipID, compatibilityLayerHandlerType handler);

    /**
        Scan for other nodes, and exchange the chosen message with any that are found.

        @message The message to send to all other nodes.

    */
    void attemptScan(const String &message);
    void attemptScan(char *message);

    template<size_t Size>
    void attemptScan(char (&message)[Size]);

    ////////////////////////////</DEPRECATED> TODO: REMOVE IN 2.5.0////////////////////////////

    ~ESP8266WiFiMesh();

    /**
        WiFiMesh Constructor method. Creates a WiFi Mesh Node, ready to be initialised.

        @param requestHandler The callback handler for dealing with received requests. Takes a string as an argument which
                is the request string received from another node and returns the string to send back.
        @param responseHandler The callback handler for dealing with received responses. Takes a string as an argument which
                is the response string received from another node. Returns a transmission status code as a transmission_status_t.
        @param networkFilter The callback handler for deciding which WiFi networks to connect to.
        @param meshPassword The WiFi password for the mesh network.
        @param meshName The name of the mesh network. Used as prefix for the node SSID and to find other network nodes in the example network filter function.
        @param nodeID The id for this mesh node. Used as suffix for the node SSID. If set to "", the id will default to ESP.getChipId().
        @param verboseMode Determines if we should print the events occurring in the library to Serial. Off by default.
        @param meshWiFiChannel The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
                          WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
                          This can cause problems if several ESP8266WiFiMesh instances exist on the same ESP8266 and use different WiFi channels.
                          In such a case, whenever the station of one ESP8266WiFiMesh instance connects to an AP, it will silently force the
                          WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly
                          make it impossible for other stations to detect the APs whose WiFi channels have changed.
        @param serverPort The server port used by the AP of the ESP8266WiFiMesh instance. If multiple APs exist on a single ESP8266, each requires a separate server port.
                          If two AP:s on the same ESP8266 are using the same server port, they will not be able to have both server instances active at the same time.
                          This is managed automatically by the activateAP method.

    */
    ESP8266WiFiMesh(requestHandlerType requestHandler, responseHandlerType responseHandler, networkFilterType networkFilter,
                    const String &meshPassword, const String &meshName = "MeshNode_", const String &nodeID = WIFI_MESH_EMPTY_STRING, bool verboseMode = false,
                    uint8 meshWiFiChannel = 1, uint16_t serverPort = 4011);

    /**
        A vector that contains the NetworkInfo for each WiFi network to connect to.
        The connectionQueue vector is cleared before each new scan and filled via the networkFilter callback function once the scan completes.
        WiFi connections will start with connectionQueue[0] and then incrementally proceed to higher vector positions.
        Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
    */
    static std::vector<NetworkInfo> connectionQueue;

    /**
        A vector with the TransmissionResult for each AP to which a transmission was attempted during the latest attemptTransmission call.
        The latestTransmissionOutcomes vector is cleared before each new transmission attempt.
        Connection attempts are indexed in the same order they were attempted.
        Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
    */
    static std::vector<TransmissionResult> latestTransmissionOutcomes;

    /**
        @returns True if latest transmission was successful (i.e. latestTransmissionOutcomes is not empty and all entries have transmissionStatus TS_TRANSMISSION_COMPLETE). False otherwise.
    */
    static bool latestTransmissionSuccessful();

    /**
        Initialises the node.
    */
    void begin();

    /**
        Each AP requires a separate server port. If two AP:s are using the same server port, they will not be able to have both server instances active at the same time.
        This is managed automatically by the activateAP method.
    */
    void activateAP();
    void deactivateAP();
    void restartAP();

    /**
        Get the ESP8266WiFiMesh instance currently in control of the ESP8266 AP.
        Note that the result will be nullptr when there is no active AP controller.
        If another instance takes control over the AP after the pointer is created,
        the created pointer will still point to the old AP instance.

        @returns A pointer to the ESP8266WiFiMesh instance currently in control of the ESP8266 AP,
                or nullptr if there is no active AP controller.
    */
    static ESP8266WiFiMesh * getAPController();

    /**
        Check if this ESP8266WiFiMesh instance is in control of the ESP8266 AP.

        @returns True if this ESP8266WiFiMesh instance is in control of the ESP8266 AP. False otherwise.
    */
    bool isAPController();

    /**
        Change the WiFi channel used by this ESP8266WiFiMesh instance.
        Will also change the WiFi channel for the active AP if this ESP8266WiFiMesh instance is the current AP controller and it is possible to change channel.

        WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
        This can cause problems if several ESP8266WiFiMesh instances exist on the same ESP8266 and use different WiFi channels.
        In such a case, whenever the station of one ESP8266WiFiMesh instance connects to an AP, it will silently force the
        WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly
        make it impossible for other stations to detect the APs whose WiFi channels have changed.

        @param newWiFiChannel The WiFi channel to change to. Valid values are integers from 1 to 13.

    */
    void setWiFiChannel(uint8 newWiFiChannel);
    uint8 getWiFiChannel();

    /**
        Change the mesh name used by this ESP8266WiFiMesh instance.
        Will also change the mesh name (SSID prefix) for the active AP if this ESP8266WiFiMesh instance is the current AP controller.

        @param newMeshName The mesh name to change to.
    */
    void setMeshName(const String &newMeshName);
    String getMeshName();

    /**
        Change the node id used by this ESP8266WiFiMesh instance.
        Will also change the node id (SSID suffix) for the active AP if this ESP8266WiFiMesh instance is the current AP controller.

        @param newNodeID The node id to change to.
    */
    void setNodeID(const String &newNodeID);
    String getNodeID();

    /**
        Change the SSID (mesh name + node id) used by this ESP8266WiFiMesh instance.
        Will also change the SSID for the active AP if this ESP8266WiFiMesh instance is the current AP controller.

        @param newMeshName The mesh name to change to. Will be the SSID prefix.
        @param newNodeID The node id to change to. Will be the SSID suffix.
    */
    void setSSID(const String &newMeshName, const String &newNodeID);
    String getSSID();

    /**
        Set the message that will be sent to other nodes when calling attemptTransmission.

        @param newMessage The message to send.
    */
    void setMessage(const String &newMessage);
    String getMessage();

    /**
        If AP connection already exists, and the initialDisconnect argument is set to false, send message only to the already connected AP.
        Otherwise, scan for other networks, send the scan result to networkFilter and then transmit the message to the networks found in connectionQueue.

        @param message The message to send to other nodes. It will be stored in the class instance until replaced via attemptTransmission or setMessage.
        @param concludingDisconnect Disconnect from AP once transmission is complete.
        @param initialDisconnect Disconnect from any currently connected AP before attempting transmission.
        @param noScan Do not scan for new networks and do not call networkFilter function. Will only use the data already in connectionQueue for the transmission.
        @param scanAllWiFiChannels Scan all WiFi channels during a WiFi scan, instead of just the channel the ESP8266WiFiMesh instance is using.
                                     Scanning all WiFi channels takes about 2100 ms, compared to just 60 ms if only channel 1 (standard) is scanned.
                                     Note that if the ESP8266 has an active AP, that AP will switch WiFi channel to match that of any other AP the ESP8266 connects to.
                                     This can make it impossible for other nodes to detect the AP if they are scanning the wrong WiFi channel.
    */
    void attemptTransmission(const String &message, bool concludingDisconnect = true, bool initialDisconnect = false, bool noScan = false, bool scanAllWiFiChannels = false);

    /**
        If any clients are connected, accept their requests and call the requestHandler function for each one.
    */
    void acceptRequest();

    /**
        Set a static IP address for the ESP8266 and activate use of static IP.
        The static IP needs to be at the same subnet as the server's gateway.
    */
    void setStaticIP(const IPAddress &newIP);
    IPAddress getStaticIP();
    void disableStaticIP();

    /**
        An empty IPAddress. Used as default when no IP is set.
    */
    static const IPAddress emptyIP;

    void setRequestHandler(requestHandlerType requestHandler);
    requestHandlerType getRequestHandler();

    void setResponseHandler(responseHandlerType responseHandler);
    responseHandlerType getResponseHandler();

    void setNetworkFilter(networkFilterType networkFilter);
    networkFilterType getNetworkFilter();

    /**
        Set whether scan results from this ESP8266WiFiMesh instance will include WiFi networks with hidden SSIDs.
        This is false by default.
        The SSID field of a found hidden network will be blank in the scan results.
        WiFi.isHidden(networkIndex) can be used to verify that a found network is hidden.

        @param scanHidden If true, WiFi networks with hidden SSIDs will be included in scan results.
    */
    void setScanHidden(bool scanHidden);
    bool getScanHidden();

    /**
        Set whether the AP controlled by this ESP8266WiFiMesh instance will have a WiFi network with hidden SSID.
        This is false by default.
        Will also change the setting for the active AP if this ESP8266WiFiMesh instance is the current AP controller.

        @param apHidden If true, the WiFi network created will have a hidden SSID.
    */
    void setAPHidden(bool apHidden);
    bool getAPHidden();

    /**
        Set the maximum number of stations that can simultaneously be connected to the AP controlled by this ESP8266WiFiMesh instance.
        This number is 4 by default.
        Once the max number has been reached, any other station that wants to connect will be forced to wait until an already connected station disconnects.
        The more stations that are connected, the more memory is required.
        Will also change the setting for the active AP if this ESP8266WiFiMesh instance is the current AP controller.

        @param maxAPStations The maximum number of simultaneous station connections allowed. Valid values are 0 to 8.
    */
    void setMaxAPStations(uint8_t maxAPStations);
    bool getMaxAPStations();

    /**
        Set the timeout for each attempt to connect to another AP that occurs through the attemptTransmission method by this ESP8266WiFiMesh instance.
        The timeout is 10 000 ms by default.

        @param connectionAttemptTimeoutMs The timeout for each connection attempt, in milliseconds.
    */
    void setConnectionAttemptTimeout(int32_t connectionAttemptTimeoutMs);
    int32_t getConnectionAttemptTimeout();

    /**
        Set the timeout to use for transmissions when this ESP8266WiFiMesh instance acts as a station (i.e. when connected to another AP).
        This will affect the timeout of the attemptTransmission method once a connection to an AP has been established.
        The timeout is 5 000 ms by default.

        @param stationModeTimeoutMs The timeout to use, in milliseconds.
    */
    void setStationModeTimeout(int stationModeTimeoutMs);
    int getStationModeTimeout();

    /**
        Set the timeout to use for transmissions when this ESP8266WiFiMesh instance acts as an AP (i.e. when receiving connections from other stations).
        This will affect the timeout of the acceptRequest method.
        The timeout is 4 500 ms by default.
        Will also change the setting for the active AP if this ESP8266WiFiMesh instance is the current AP controller.

        @param apModeTimeoutMs The timeout to use, in milliseconds.
    */
    void setAPModeTimeout(uint32_t apModeTimeoutMs);
    uint32_t getAPModeTimeout();
};

#endif
