ESP8266 WiFi Mesh
=================

A library for turning your ESP8266 into a mesh network node.

The library has been tested and works with Arduino core for ESP8266 version 2.3.0 (with default lwIP) and 2.4.1 (with lwIP 1.4). Random disconnects and crashes seem to happen with the most recent master branch, so better stick to the release builds for now.

Usage
-----

See the included example. The main functions to modify are manageRequest and manageResponse.

If using Arduino core for ESP8266 version 2.4.1, opt for lwIP v1.4 for now (can be changed in the Tools menu of Arduino IDE). Nodes seem to be unable to connect to more than one other node when using lwIP 2.0!

Note that this library can use static IP:s for the nodes to speed up initial connection. To enable this, use the setStaticIP method after calling the begin method, as in the included example. Ensure that nodes connecting to the same AP have distinct static IP:s. Node IP:s need to be at the same subnet as the server gateway (192.168.4 for this library by default). It may also be worth noting that station gateway IP must match the IP for the server on the nodes, though this is the default setting for the library.

Also, remember to change the default mesh network WiFi password!

Some things to keep in mind
---------------------------

A maximum of 5 stations can be connected at a time to each AP.

Unlike WiFi.mode(WIFI_AP), the WiFi.mode(WIFI_AP_STA) which is used in this library allows nodes to stay connected to an AP they connect to while in STA mode, at the same time as they can receive connections from other stations. Nodes cannot send data to an AP while in STA_AP mode though, that requires STA mode. Switching to STA mode will disconnect all stations connected to the node AP (though they can request a reconnect even while the previous AP node is in STA mode).

Scanning for networks (e.g. via the attemptTransmission method) causes the WiFi radio to cycle through all WiFi channels which means existing WiFi connections are likely to break or work poorly if done frequently.