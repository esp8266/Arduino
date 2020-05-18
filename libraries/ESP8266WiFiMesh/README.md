# ESP8266 WiFi Mesh

## Contents
1. [Overview](#Overview)
   * [How does it work?](#Work)
2. [Tell me more!](#More)
   * [EspnowMeshBackend](#EspnowMeshBackendMore)
   * [TcpIpMeshBackend](#TcpIpMeshBackendMore)
   * [FloodingMesh](#FloodingMeshMore)


## <a name="Overview"></a>Overview

This is a library for creating a mesh network using the ESP8266.

The library has been tested and works with Arduino core for ESP8266 version 2.7.2 (with lwIP2). It may work with earlier and later core releases, but this has not been tested during development.

**Note:** This mesh library has been extensively rewritten for core release 2.7.2. The old method signatures have been retained for compatibility purposes, but will be removed in core release 3.0.0. If you are still using these old method signatures please consider migrating to the new API shown in the `EspnowMeshBackend.h` or `TcpIpMeshBackend.h` source files.

### <a name="Work"></a>How does it work?

The ESP8266 WiFi Mesh library is a cake, metaphorically speaking. At the bottom you have the general ESP8266 Arduino core WiFi functionality. On top of this two mesh backends have been created (`EspnowMeshBackend` and `TcpIpMeshBackend`), a yummy filling that completely covers the bottom. Then at the very top over the backends is the beautiful and delicious frosting: `FloodingMesh`. `FloodingMesh` is an actual mesh network implementation that uses the `EspnowMeshBackend`.

Eating the cake would typically be a process which involves all the layers, but it is completely possible to use both the EspnowMeshBackend and the TcpIpMeshBackend separately from FloodingMesh, perhaps to construct your own mesh network architecture or just to simplify the usage of TCP/IP or ESP-NOW. If you have made a nice mesh architecture with this library that you would like to share with the rest of the world, feel free to make a PR with it!

There is plenty of details to the operations of the library, but if you want to get started quickly you really only need to know this: In the example folder of the library there is a file called `HelloMesh.ino`. Upload it to a few ESP8266 and you have a working mesh network. Change the `useLED` variable to true if you have built-in LEDs on your ESP8266s to illustrate how the message is spread through the network. Change the `floodingMesh.broadcast` calls to modify what the mesh nodes are transmitting to each other.

Finally, three things are important to note:

1. This library uses the standard Arduino core for ESP8266 WiFi functions. Therefore, other code that also uses these WiFi functions (e.g. `WiFi.mode()`) may cause conflicts with the library, resulting in strange behaviour. See the FAQ for ideas on how to work around this.
2. Both the `EspnowMeshBackend` and the `TcpIpMeshBackend` can be used simultaneously on the same node. However, since there is only one WiFi radio on the ESP8266, only one backend at a time will be responsible for the settings of this radio (SSID, WiFi channel etc.). The backend in control is known as the `APController` in the library. Both backends can still send messages, regardless of who is `APController`.
3. The `MeshBackendBase`, `EspnowMeshBackend`, `TcpIpMeshBackend` and `FloodingMesh` source files are supposed to be the main front-ends of the library and are all extensively documented. If you wonder how something is working, chances are good that you will find an answer in the documentation of those files.

## <a name="More"></a>Tell me more!

### <a name="EspnowMeshBackendMore"></a>EspnowMeshBackend

#### Usage

The basic operation of the ESP-NOW mesh backend is as follows:

### <a name="TcpIpMeshBackendMore"></a>TcpIpMeshBackend

#### Usage

The basic operation of the TCP/IP mesh backend is as follows:

The `attemptTransmission` method of the TcpIpMeshBackend instance is called with a message to send to other nodes in the mesh network. If the node is already connected to an AP, the message is sent only to that AP. Otherwise a WiFi scan is performed. The scan results are sent to the `networkFilter` callback function of the TcpIpMeshBackend instance which adds the AP:s of interest to the `connectionQueue` vector. The message is then transmitted to the networks in the `connectionQueue`, and the response from each AP is sent to the `responseHandler` callback of the TcpIpMeshBackend instance. The outcome from each transmission attempt can be found in the `latestTransmissionOutcomes` vector.

The node receives messages from other TCP/IP nodes by calling the `acceptRequest` method of the TcpIpMeshBackend instance. These received messages are passed to the `requestHandler` callback of the mesh instance. For each received message the return value of `requestHandler` is sent to the other node as a response to the message. Since received ESP-NOW messages are handled via a callback, there is no need to call `acceptRequest` to receive these.

For more details, see the included HelloTcpIp example. The main functions to modify in the example are `manageRequest` (`requestHandler`), `manageResponse` (`responseHandler`), `networkFilter` and `exampleTransmissionOutcomesUpdateHook`. There is also much more information to be found in the source code comments.

#### Note

* This library can use static IP:s for the nodes to speed up connection times. To enable this, use the `setStaticIP` method after calling the `begin` method, as in the included example. When using static IP, the following is good to keep in mind:

  Ensure that nodes connecting to the same AP have distinct static IP:s.

  Node IP:s need to be at the same subnet as the server gateway (192.168.4 for this library by default).

  Station gateway IP must match the IP for the server on the nodes. This is the default setting for the library.

  Static IP is a global setting (for now), meaning that all TcpIpMeshBackend instances on the same ESP8266 share the same static IP settings.

* Scanning all WiFi channels (e.g. via the `attemptTransmission` method with the `scanAllWiFiChannels` argument set to true) will slow down scans considerably and make it more likely that existing WiFi connections will break during scans.

* If the ESP8266 has an active AP, that AP will switch WiFi channel to match that of any other AP the TcpIpMeshBackend of the ESP8266 connects to (compare next bullet point). This can make it impossible for other nodes to detect the AP if they are scanning the wrong WiFi channel. To remedy this, force the AP back on the original channel by using the `restartAP` method of the current AP controller once the ESP8266 has disconnected from the other AP. This would typically be done like so:

  ```
  if(MeshBackendBase *apController = MeshBackendBase::getAPController()) // Make sure apController is not nullptr
    apController->restartAP();
  ```

* It is possible to have several TcpIpMeshBackend instances running on every ESP8266 (e.g. to communicate with different mesh networks). However, because the ESP8266 has one WiFi radio only one AP per ESP8266 can be active at a time. Also note that if the TcpIpMeshBackend instances use different WiFi channels, active APs are forced to use the same WiFi channel as active stations, possibly causing AP disconnections.

* While it is possible to connect to other nodes by only giving their SSID, e.g. `TcpIpMeshBackend::connectionQueue().emplace_back("NodeSSID");`, it is recommended that AP WiFi channel and AP BSSID are given as well, to minimize connection delay.

* Also, remember to change the default mesh network WiFi password!

#### General Information

* By default, a maximum of 4 stations can be connected at a time to each AP. This can be changed to a value in the range 0 to 8 via the `setMaxAPStations` method. Once the max number has been reached, any other station that wants to connect will be forced to wait until an already connected station disconnects. The more stations that are connected, the more memory is required.

* Unlike `WiFi.mode(WIFI_AP)`, the `WiFi.mode(WIFI_AP_STA)` which is used in this library allows TCP/IP nodes to stay connected to an AP they connect to while in STA mode, at the same time as they can receive connections from other stations. Nodes cannot send data to an AP while in STA_AP mode though, that requires STA mode. Switching to STA mode will sometimes disconnect stations connected to the node AP (though they can request a reconnect even while the previous AP node is in STA mode).

### <a name="FloodingMeshMore"></a>FloodingMesh

The FloodingMesh exclusively uses the `EspnowMeshBackend`. The mesh network size is only limited by available MAC addresses, so the maximum is (2^48)/2 = 140 trillion give or take. However, the maximum throughput of the FloodingMesh is around 100 messages per second with 234 bytes per message, so using the maximum number of nodes is not recommended in most cases.

As the name implies, FloodingMesh is a simple flooding mesh architecture, which means it stores no mesh network routing data in the nodes but only passes new messages on. It therefore has no RAM overhead for network size, which is important for the ESP8266 since available RAM is very limited. The downside is that there is a lot of network traffic for each sent message, so especially for dense networks a lot of interference will be created. Based on tests, a mesh with 30 nodes close together will work well (1-2 dropped messages of 1000). A mesh with around 160 nodes close together will not work at all (though this would probably be solved by spreading out the nodes more, so the interference is reduced).

#### Usage

Change the `useLED` variable to true if you have built-in LEDs on your ESP8266s to illustrate how the message is spread through the network. Change the `floodingMesh.broadcast` calls to modify what the mesh nodes are transmitting to each other.