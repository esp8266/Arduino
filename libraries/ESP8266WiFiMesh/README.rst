ESP8266 WiFi Mesh
=================

A library for turning your ESP8266 into a mesh network node.

The library has been tested and works with Arduino core for ESP8266 version 2.3.0 (with default lwIP) and 2.4.2 or higher (with lwIP 1.4 and lwIP2).

Usage
-----

The basic operation of a mesh node is as follows:
The ``attemptTransmission`` method of the ESP8266WiFiMesh instance is called with a message to send to other nodes in the mesh network. If the node is already connected to an AP, the message is sent only to that AP. Otherwise a WiFi scan is performed. The scan results are sent to the ``networkFilter`` callback function of the ESP8266WiFiMesh instance which adds the AP:s of interest to the ``connection_queue`` vector. The message is then transmitted to the networks in the ``connection_queue``, and the response from each AP is sent to the ``responseHandler`` callback of the ESP8266WiFiMesh instance. The outcome from each transmission attempt can be found in the ``latest_transmission_outcomes`` vector. 
The node receives messages from other nodes by calling the ``acceptRequest`` method of the ESP8266WiFiMesh instance. These received messages are passed to the ``requestHandler`` callback of the mesh instance. For each received message the return value of ``requestHandler`` is sent to the other node as a response to the message.

For more details, see the included example. The main functions to modify in the example are ``manageRequest`` (``requestHandler``), ``manageResponse`` (``responseHandler``) and ``networkFilter``.

Note that this library can use static IP:s for the nodes to speed up connection times. To enable this, use the ``setStaticIP`` method after calling the ``begin`` method, as in the included example. Ensure that nodes connecting to the same AP have distinct static IP:s. Node IP:s need to be at the same subnet as the server gateway (192.168.4 for this library by default). It may also be worth noting that station gateway IP must match the IP for the server on the nodes, though this is the default setting for the library.

When Arduino core for ESP8266 version 2.4.2 or higher is used, there are optimizations available for WiFi scans and static IP use to reduce the time it takes for nodes to connect to each other. These optimizations are enabled by default. To take advantage of the static IP optimizations you also need to use lwIP2. The lwIP version can be changed in the Tools menu of Arduino IDE.

If you are using a core version prior to 2.4.2 it is possible to disable the WiFi scan and static ip optimizations by commenting out the ``ENABLE_STATIC_IP_OPTIMIZATION`` and ``ENABLE_WIFI_SCAN_OPTIMIZATION`` defines in ESP8266WiFiMesh.h. Press Ctrl+K in the Arduino IDE while an example from the mesh library is opened, to open the library folder (or click "Show Sketch Folder" in the Sketch menu). ESP8266WiFiMesh.h can then be found at ESP8266WiFiMesh/src. Edit the file with any text editor.

It is possible to have several ESP8266WiFiMesh instances running on every ESP8266 (e.g. to communicate with different mesh networks). However, because the ESP8266 has one WiFi radio only one AP per ESP8266 can be active at a time. Also note that if the ESP8266WiFiMesh instances use different WiFi channels, active APs are forced to use the same WiFi channel as active stations, possibly causing AP disconnections.

While it is possible to connect to other nodes by only giving their SSID, e.g. ``ESP8266WiFiMesh::connection_queue.push_back(NetworkInfo("NodeSSID"));``, it is recommended that AP WiFi channel and AP BSSID are given as well, to minimize connection delay.

Also, remember to change the default mesh network WiFi password!

Some things to keep in mind
---------------------------

This library uses the standard Arduino core for ESP8266 WiFi functions. Therefore, other code that also uses these WiFi functions may cause conflicts with the library, resulting in strange behaviour.

A maximum of 5 stations can be connected at a time to each AP.

Unlike ``WiFi.mode(WIFI_AP)``, the ``WiFi.mode(WIFI_AP_STA)`` which is used in this library allows nodes to stay connected to an AP they connect to while in STA mode, at the same time as they can receive connections from other stations. Nodes cannot send data to an AP while in STA_AP mode though, that requires STA mode. Switching to STA mode will disconnect all stations connected to the node AP (though they can request a reconnect even while the previous AP node is in STA mode).

Scanning for networks (e.g. via the ``attemptTransmission`` method) without the WiFi scan optimizations for core version 2.4.2 mentioned above, causes the WiFi radio to cycle through all WiFi channels which means existing WiFi connections are likely to break or work poorly if done frequently.