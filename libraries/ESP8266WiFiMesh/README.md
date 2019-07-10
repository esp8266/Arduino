ESP8266 WiFi Mesh
=================

A library for turning your ESP8266 into a mesh network node.

The library has been tested and works with Arduino core for ESP8266 version 2.6.0 (with lwIP2). It may work with earlier and later core releases, but this has not been tested during development.

**Note:** This mesh library has been rewritten for core release 2.6.0. The old method signatures have been retained for compatibility purposes, but will be removed in core release 3.0.0. If you are still using these old method signatures please consider migrating to the new API shown in the `EspnowMeshBackend.h` or `TcpIpMeshBackend.h` source files.

Usage
-----

The basic operation of a mesh node is as follows:

The `attemptTransmission` method of the ESP8266WiFiMesh instance is called with a message to send to other nodes in the mesh network. If the node is already connected to an AP, the message is sent only to that AP. Otherwise a WiFi scan is performed. The scan results are sent to the `networkFilter` callback function of the ESP8266WiFiMesh instance which adds the AP:s of interest to the `connectionQueue` vector. The message is then transmitted to the networks in the `connectionQueue`, and the response from each AP is sent to the `responseHandler` callback of the ESP8266WiFiMesh instance. The outcome from each transmission attempt can be found in the `latestTransmissionOutcomes` vector.

The node receives messages from other nodes by calling the `acceptRequest` method of the ESP8266WiFiMesh instance. These received messages are passed to the `requestHandler` callback of the mesh instance. For each received message the return value of `requestHandler` is sent to the other node as a response to the message.

For more details, see the included example. The main functions to modify in the example are `manageRequest` (`requestHandler`), `manageResponse` (`responseHandler`) and `networkFilter`. There is also more information to be found in the source code comments. An example is the ESP8266WiFiMesh constructor comment, which is shown below for reference: 
```
/**
* WiFiMesh Constructor method. Creates a WiFi Mesh Node, ready to be initialised.
*
* @param requestHandler The callback handler for dealing with received requests. Takes a string as an argument which
*          is the request string received from another node and returns the string to send back.
* @param responseHandler The callback handler for dealing with received responses. Takes a string as an argument which
*          is the response string received from another node. Returns a transmission status code as a transmission_status_t.
* @param networkFilter The callback handler for deciding which WiFi networks to connect to.
* @param meshPassword The WiFi password for the mesh network.
* @param meshName The name of the mesh network. Used as prefix for the node SSID and to find other network nodes in the example network filter function.
* @param nodeID The id for this mesh node. Used as suffix for the node SSID. If set to "", the id will default to ESP.getChipId().
* @param verboseMode Determines if we should print the events occurring in the library to Serial. Off by default.
* @param meshWiFiChannel The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
*                    WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
*                    This can cause problems if several ESP8266WiFiMesh instances exist on the same ESP8266 and use different WiFi channels. 
*                    In such a case, whenever the station of one ESP8266WiFiMesh instance connects to an AP, it will silently force the 
*                    WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
*                    make it impossible for other stations to detect the APs whose WiFi channels have changed.
* @param serverPort The server port used by the AP of the ESP8266WiFiMesh instance. If multiple APs exist on a single ESP8266, each requires a separate server port. 
*                    If two AP:s on the same ESP8266 are using the same server port, they will not be able to have both server instances active at the same time.                  
*                    This is managed automatically by the activateAP method.
* 
*/
ESP8266WiFiMesh(requestHandlerType requestHandler, responseHandlerType responseHandler, networkFilterType networkFilter, 
                const String &meshPassword, const String &meshName = "MeshNode_", const String &nodeID = WIFI_MESH_EMPTY_STRING, bool verboseMode = false, 
                uint8 meshWiFiChannel = 1, int serverPort = 4011);
```

### Note

* This library can use static IP:s for the nodes to speed up connection times. To enable this, use the `setStaticIP` method after calling the `begin` method, as in the included example. When using static IP, the following is good to keep in mind:

  Ensure that nodes connecting to the same AP have distinct static IP:s.

  Node IP:s need to be at the same subnet as the server gateway (192.168.4 for this library by default).

  Station gateway IP must match the IP for the server on the nodes. This is the default setting for the library.

  Static IP is a global setting (for now), meaning that all ESP8266WiFiMesh instances on the same ESP8266 share the same static IP settings.

* When Arduino core for ESP8266 version 2.4.2 or higher is used, there are optimizations available for WiFi scans and static IP use to reduce the time it takes for nodes to connect to each other. These optimizations are enabled by default. To take advantage of the static IP optimizations you also need to use lwIP2. The lwIP version can be changed in the Tools menu of Arduino IDE.

* The WiFi scan optimization mentioned above works by making WiFi scans only search through the same WiFi channel as the ESP8266WiFiMesh instance is using. If you would like to scan all WiFi channels instead, set the `scanAllWiFiChannels` argument of the `attemptTransmission` method to `true`. Note that scanning all WiFi channels will slow down scans considerably and make it more likely that existing WiFi connections will break during scans. Also note that if the ESP8266 has an active AP, that AP will switch WiFi channel to match that of any other AP the ESP8266 connects to (compare next bullet point). This can make it impossible for other nodes to detect the AP if they are scanning the wrong WiFi channel. To remedy this, force the AP back on the original channel by using the `restartAP` method of the current AP controller once the ESP8266 has disconnected from the other AP. This would typically be done like so:

  ```
  if(ESP8266WiFiMesh *apController = ESP8266WiFiMesh::getAPController()) // Make sure apController is not nullptr
    apController->restartAP();
  ```

* It is possible to have several ESP8266WiFiMesh instances running on every ESP8266 (e.g. to communicate with different mesh networks). However, because the ESP8266 has one WiFi radio only one AP per ESP8266 can be active at a time. Also note that if the ESP8266WiFiMesh instances use different WiFi channels, active APs are forced to use the same WiFi channel as active stations, possibly causing AP disconnections.

* While it is possible to connect to other nodes by only giving their SSID, e.g. `ESP8266WiFiMesh::connectionQueue.push_back(NetworkInfo("NodeSSID"));`, it is recommended that AP WiFi channel and AP BSSID are given as well, to minimize connection delay.

* Also, remember to change the default mesh network WiFi password!

General Information
---------------------------

* This library uses the standard Arduino core for ESP8266 WiFi functions. Therefore, other code that also uses these WiFi functions may cause conflicts with the library, resulting in strange behaviour.

* By default, a maximum of 4 stations can be connected at a time to each AP. This can be changed to a value in the range 0 to 8 via the `setMaxAPStations` method. Once the max number has been reached, any other station that wants to connect will be forced to wait until an already connected station disconnects. The more stations that are connected, the more memory is required.

* Unlike `WiFi.mode(WIFI_AP)`, the `WiFi.mode(WIFI_AP_STA)` which is used in this library allows nodes to stay connected to an AP they connect to while in STA mode, at the same time as they can receive connections from other stations. Nodes cannot send data to an AP while in STA_AP mode though, that requires STA mode. Switching to STA mode will sometimes disconnect stations connected to the node AP (though they can request a reconnect even while the previous AP node is in STA mode).

* Scanning for networks (e.g. via the `attemptTransmission` method) without the WiFi scan optimizations for core version 2.4.2 mentioned above, causes the WiFi radio to cycle through all WiFi channels which means existing WiFi connections are likely to break or work poorly if done frequently.