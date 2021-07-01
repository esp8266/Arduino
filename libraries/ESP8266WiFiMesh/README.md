# ESP8266 WiFi Mesh

## Contents
1. [Overview](#Overview)
2. [How does it work?](#Work)
3. [The first step](#Start)
4. [TcpIpMeshBackend](#TcpIpMeshBackendMore)
   * [Usage](#TcpIpMeshBackendUsage)
   * [Note](#TcpIpMeshBackendNote)
   * [General Information](#TcpIpMeshBackendGeneral)
5. [EspnowMeshBackend](#EspnowMeshBackendMore)
   * [Usage](#EspnowMeshBackendUsage)
   * [Note](#EspnowMeshBackendNote)
   * [Callbacks](#EspnowMeshBackendCallbacks)
   * [Encryption](#EspnowMeshBackendEncryption)
      * [CCMP](#CCMP)
      * [AEAD](#AEAD)
6. [FloodingMesh](#FloodingMeshMore)
   * [Usage](#FloodingMeshUsage)
   * [Note](#FloodingMeshNote)
   * [Serialization and the internal state of a node](#FloodingMeshSerialization)
7. [FAQ](#FAQ)
   * [My ESP8266 crashes on start-up when I use the library!](#FAQStartupCrash)
   * [The node does not remember the SSID I assign to it!](#FAQSSIDAmnesia)
   * [I want to control the WiFi mode myself.](#FAQModeControl)
   * [I have a lot of interference from all the nodes that are close to each other. What can I do?](#FAQInterference)
   * [How do I change the interval of the WiFi AP beacon broadcast?](#FAQBeaconInterval)
   * [My ESP is ignoring the WiFi AP beacon broadcast interval settings you just told me about above! (a.k.a. How do I change the WiFi scan mode to passive?)](#FAQPassiveScan)
   * [My internet is slower when I connect the ESP8266 to my router!](#FAQSlowRouter)


## <a name="Overview"></a>Overview

This is a library for creating a mesh network using the ESP8266.

The library has been tested and works with Arduino Core for ESP8266 version 3.0.0 (with lwIP2). It may work with earlier and later core releases, but this has not been tested during development.

**Note:** This mesh library has been extensively rewritten for core release 3.0.0. The old method signatures have been retained for compatibility purposes, but will be removed in core release 3.0.X. If you are still using these old method signatures please consider migrating to the new API shown in the `EspnowMeshBackend.h` or `TcpIpMeshBackend.h` source files.

## <a name="Work"></a>How does it work?

The ESP8266 WiFi Mesh library is a cake, metaphorically speaking. At the bottom you have the general ESP8266 Arduino Core WiFi functionality. On top of this two mesh backends have been created (`EspnowMeshBackend` and `TcpIpMeshBackend`), a yummy filling that completely covers the bottom. Then at the very top over the backends is the beautiful and delicious frosting: `FloodingMesh`. `FloodingMesh` is an actual mesh network implementation that uses the `EspnowMeshBackend`.

Eating the cake in its current form is a process which involves all the layers. However, if you prefer to be your own pastry chef it is easy to use both the `EspnowMeshBackend` and the `TcpIpMeshBackend` separately from `FloodingMesh`, perhaps to construct your own mesh network architecture or just to simplify the usage of TCP/IP or ESP-NOW. If you have made a nice mesh architecture with this library that you would like to share with the rest of the world, feel free to make a PR with it!

In general ESP-NOW is faster than TCP/IP for small data payloads (up to a few kB). The data segment of a standard ESP-NOW transmission is 234 bytes, which takes around 2-4 ms to transmit.

TCP/IP takes longer to connect (around 1000 ms), and an AP has to disconnect all connected stations in order to transfer data to another AP. However, this backend has a much higher data transfer speed than ESP-NOW once connected (100x faster or so).

## <a name="Start"></a>The first step

There are plenty of details to the operations of the library, but if you want to get started quickly you really only need to know this: In the example folder of the library there is a file called `HelloMesh.ino`. Upload it to a few ESP8266 and you have a working mesh network. Change the `useLED` variable to `true` if you have built-in LEDs on your ESP8266s to illustrate how the message is spread through the network. Change the `floodingMesh.broadcast` calls to modify what the mesh nodes are transmitting to each other. Change the code of the `meshMessageHandler` to modify how mesh nodes react to received transmissions.

Finally, three things are important to note:

1. This library uses the standard Arduino Core for ESP8266 WiFi functions. Therefore, other code that also uses these WiFi functions (e.g. `WiFi.mode()`) may cause conflicts with the library, resulting in strange behaviour. See "[I want to control the WiFi mode myself](#FAQModeControl)" in the FAQ for ideas on how to work around this.
2. Both the `EspnowMeshBackend` and the `TcpIpMeshBackend` can be used simultaneously on the same node. However, since there is only one WiFi radio on the ESP8266, only one backend at a time will be responsible for the settings of this radio (SSID, WiFi channel etc.). The backend in control is known as the `APController` in the library. Both backends can still send messages, regardless of who is `APController`.
3. The `MeshBackendBase`, `EspnowMeshBackend`, `TcpIpMeshBackend` and `FloodingMesh` source files are meant to be the main front-ends of the library and are all extensively documented. If you wonder about how something is working, chances are good that you will find an answer in the documentation of those files.

## <a name="TcpIpMeshBackendMore"></a>TcpIpMeshBackend

### <a name="TcpIpMeshBackendUsage"></a>Usage

The basic operation of the TCP/IP mesh backend is as follows:

The `attemptTransmission` method of the TcpIpMeshBackend instance is called with a message to send to other nodes in the mesh network. If the node is already connected to an AP, the message is sent only to that AP. Otherwise the default behaviour is for a  WiFi scan to be performed. The scan results are sent to the `networkFilter` callback function of the TcpIpMeshBackend instance which adds the AP:s of interest to the `connectionQueue` vector. The message is then transmitted to the networks in the `connectionQueue`, and the response from each AP is sent to the `responseHandler` callback of the TcpIpMeshBackend instance. The outcome from each transmission attempt can be found in the `latestTransmissionOutcomes` vector.

The node receives messages from other TCP/IP nodes by calling the `acceptRequests` method of the TcpIpMeshBackend instance. These received messages are passed to the `requestHandler` callback of the mesh instance. For each received message the return value of `requestHandler` is sent to the other node as a response to the message.

For more details, see the included HelloTcpIp example. The main functions to modify in the example are `manageRequest` (`requestHandler`), `manageResponse` (`responseHandler`), `networkFilter` and `exampleTransmissionOutcomesUpdateHook`. There is also much more information to be found in the source code comments.

### <a name="TcpIpMeshBackendNote"></a>Note

* This library can use static IP:s for the nodes to speed up connection times. To enable this, use the `setStaticIP` method after calling the `begin` method, as in the included example. When using static IP, the following is good to keep in mind:

  Ensure that nodes connecting to the same AP have distinct static IP:s.

  Node IP:s need to be at the same subnet as the server gateway (192.168.4 for this library by default).

  Station gateway IP must match the IP for the server on the nodes. This is the default setting for the library.

  Static IP is a global setting (for now), meaning that all TcpIpMeshBackend instances on the same ESP8266 share the same static IP settings.

* Scanning all WiFi channels (e.g. via the `attemptTransmission` method with the `scanAllWiFiChannels` argument set to `true`) will slow down scans considerably and make it more likely that existing WiFi connections will break during scans.

* If the ESP8266 has an active AP, that AP will switch WiFi channel to match that of any other AP the TcpIpMeshBackend of the ESP8266 connects to (compare next bullet point). This can make it impossible for other nodes to detect the AP if they are scanning the wrong WiFi channel. To remedy this, force the AP back on the original channel by using the `restartAP` method of the current AP controller once the ESP8266 has disconnected from the other AP. This would typically be done like so:

  ```
  if(MeshBackendBase *apController = MeshBackendBase::getAPController()) // Make sure apController is not nullptr
    apController->restartAP();
  ```

* It is possible to have several TcpIpMeshBackend instances running on every ESP8266 (e.g. to communicate with different mesh networks). However, because the ESP8266 has one WiFi radio only one AP per ESP8266 can be active at a time. Also note that if the TcpIpMeshBackend instances use different WiFi channels, active APs are forced to use the same WiFi channel as active stations, possibly causing AP disconnections.

* While it is possible to connect to other nodes by only giving their SSID, e.g. `TcpIpMeshBackend::connectionQueue().emplace_back("NodeSSID");`, it is recommended that AP WiFi channel and AP BSSID are given as well, to minimize connection delay.

* Also, remember to change the default mesh network WiFi password!

### <a name="TcpIpMeshBackendGeneral"></a>General Information

* By default, a maximum of 4 stations can be connected at a time to each AP. This can be changed to a value in the range 0 to 8 via the `setMaxAPStations` method. Once the max number has been reached, any other station that wants to connect will be forced to wait until an already connected station disconnects. The more stations that are connected, the more memory is required.

* Unlike `WiFi.mode(WIFI_AP)`, the `WiFi.mode(WIFI_AP_STA)` which is used in this library allows TCP/IP nodes to stay connected to an AP they connect to while in STA mode, at the same time as they can receive connections from other stations. Nodes cannot send data to an AP while in STA_AP mode though, that requires STA mode. Switching to STA mode will sometimes disconnect stations connected to the node AP (though they can request a reconnect even while the previous AP node is in STA mode).

## <a name="EspnowMeshBackendMore"></a>EspnowMeshBackend

Unlike the TcpIpMeshBackend, the ESP-NOW backend uses pure callbacks even for message reception. This means that whenever `delay()` is called or the `loop()` function returns, the ESP-NOW backend will automatically check if an ESP-NOW message has been received and send it to the correct callback. There is no need to call `acceptRequests` as for the TcpIpMeshBackend. As a result of this, it is possible to receive an ingoing ESP-NOW transmission at the same time as an outgoing ESP-NOW transmission is in progress. This will likely be noted as a spike in the usual transmission time, the size of which will depend on the execution time of `requestHandler`/`responseHandler` (determined by transmission type).

Some ESP-NOW tasks cannot be securely handled via callbacks. To manage this there are `espnowDelay` and `performEspnowMaintenance` functions available which handle these tasks separately. Either of these methods should be called regularly when your node has some time over for handling background tasks.

### <a name="EspnowMeshBackendUsage"></a>Usage

There are two primary ways to send an ESP-NOW message: `broadcast` and `attemptTransmission`.

If `broadcast` is used, the message is sent to all surrounding nodes in one transmission without any WiFi scan. When the surrounding nodes receive the broadcast they will send it to the `broadcastFilter` callback of the EspnowMeshBackend instance, and based on the return value of this callback either accept or reject the broadcast. The `broadcastFilter` callback is also responsible for removing any metadata from the broadcast.

If `attemptTransmission` is used, a WiFi scan is by default performed before the transmission. The scan results are sent to the `networkFilter` callback function of the EspnowMeshBackend instance which adds the AP:s of interest to the `connectionQueue` vector. The message is then transmitted to the nodes in the `connectionQueue`. The outcome from each transmission attempt can be found in the `latestTransmissionOutcomes` vector.

Regardless of whether `broadcast` or `attemptTransmission` is used, when a node receives a message (and it is accepted), the message is passed to the `requestHandler` callback of the EspnowMeshBackend instance. For each received message the return value of `requestHandler` is stored as a response in the `responsesToSend` waiting list. These stored responses will then be sent whenever `performEspnowMaintenance` (or `espnowDelay`) is called.

When the response is received by the node that sent the request, the response message is forwarded to the `responseHandler` callback of the EspnowMeshBackend instance that sent the request.

To be completely clear, requests are actually passed to the `broadcastFilter` and `requestHandler` callbacks belonging to the `EspnowRequestManager` of the node, but as long as there is only one EspnowMeshBackend instance on the node this will be the `EspnowRequestManager`. Also, since received ESP-NOW messages are handled via a callback, there is no need to call `acceptRequests` to receive messages, unlike with the TcpIpMeshBackend.

The EspnowMeshBackend has a few different options for encrypting messages. This is described in greater detail in the [Encryption](#EspnowMeshBackendEncryption) section below.

More information can be found in the source code comments and in the included HelloEspnow example. The main functions to modify in the example are `manageRequest` (`requestHandler`), `manageResponse` (`responseHandler`), `networkFilter` and `broadcastFilter`.

### <a name="EspnowMeshBackendNote"></a>Note

* `yield()` can cause crashes when using ESP-NOW, since the command requires code to be run in the CONT context. If you are having problems with this, use `delay()` instead. 

* This library uses the ESP8266 modules' MAC addresses to keep track of transmissions. So if you need to change the MAC addresses do so with care and preferably before any transmission is made.
Turning the AP off will make it impossible to send information to the node AP mac. However, it will still be possible to send the data to the station mac. 
To do this, send over the station mac to the transmitting node and then manually add it to the `connectionQueue` whenever a transmission should be made to that node. 

* If the available heap goes under `criticalHeapLevel()` bytes (6000 bytes by default), the ESP-NOW backend will temporarily cease accepting new incoming ESP-NOW requests in an attempt to avoid running out of RAM. Warning messages about this will also be printed to the Serial Monitor, assuming `printWarnings()` is `true` (this is the default value).

* During very heavy load the `performEspnowMaintenance` method may occasionally need to process requests for tens of milliseconds. Since this won't happen until the method is called, you can choose when this is done. Callbacks can be executed while the request processing is ongoing, but note that they should have a very fast execution time in this case. Also be sure to take into account the callback restrictions mentioned [below](#EspnowMeshBackendCallbacks).

* When `WiFi.mode(WIFI_STA)` is used, nodes are unable to receive ESP-NOW broadcast messages. All nodes can however still receive direct ESP-NOW messages to their STA mac. Nodes seem to continue transmitting successfully to the correct (broadcast) MAC regardless of WiFi mode, only message reception is affected. Different combinations of ESP-NOW roles do not seem to have any influence on the outcome. Stripping out all library code and only using the bare minimum required for a broadcast does not change the outcome. Thus, this issue seems to be unfixable until corrected by Espressif. 

  During testing it seemed for a while as though some nodes were able to receive ESP-NOW broadcasts even when in STA mode. There was no obvious difference between the nodes for which this worked and those for which it did not, so what caused this is unknown. Possibly the issue could have been caused by something stored on the nodes, perhaps a different persistent WiFi config or something similar. It is of course also possible that there was an error made during testing, but the event is noted here as it could be an avenue for further investigation.

* Although ESP-NOW responses will generally be sent in the order they were created, this is not guaranteed to be the case. For example, response order will be mixed up if some responses first fail to transmit while others transmit successfully. Use the `ResponseTransmittedHook`callback if this behaviour should be modified.

### <a name="EspnowMeshBackendCallbacks"></a>Callbacks

For maximum performance and minimum RAM usage it is very important that your callbacks and hooks can be handled quickly (within a few milliseconds, preferably), as node performance can start to suffer quickly otherwise, particularly if transmission intensity is high. Be especially wary of long Serial prints, as these require a lot of time to complete. If transmission activity is very low, it is however possible to have callbacks which take a long time to complete. In these cases, even a callback execution time of multiple seconds can be acceptable. Of course, you would get problems with other parts of the Arduino Core framework (like watch dog timer resets) if you don't call `delay()` or `ESP.wdtFeed()` within that time.

Certain methods of the EspnowMeshBackend (e.g. `attemptTransmission`, `broadcast`, `espnowDelay` and `performEspnowMaintenance`) should not be used within callbacks, since this can mess with the internal state of the backend. These methods are all using a `MutexTracker` component to enforce this requirement via asserts, so if your nodes are crashing for unknown reasons when using callbacks, make sure to check the Serial Monitor to see if there are any mutex error messages!

One way to resolve such errors is to simply always call the sensitive methods from the `loop()` instead of from a callback, possibly just storing the received value for later inside the callback. [PolledTimeout](https://github.com/esp8266/Arduino/blob/master/cores/esp8266/PolledTimeout.h) can be helpful for time tracking in this case. 

If a callback with the sensitive methods is required, it has been reported that the methods in `TaskScheduler.h` of the [TaskScheduler library](https://github.com/arkhipenko/TaskScheduler) work well when scheduling tasks. It can in this role be used as a replacement of the [Ticker](https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#ticker) functionality in the Arduino Core.

The reason the callback limitations exist is that during a transmission the library will only get an ack from the receiver when `delay()` is used. Yet `delay()` also calls all other background tasks, including user callbacks, and these must thus be safe to execute during ongoing transmissions.

### <a name="EspnowMeshBackendEncryption"></a>Encryption

There are two separate methods for encrypting a message with the ESP-NOW backend. One method creates an encrypted connection between two nodes using the built-in CCMP encryption of the ESP8266. The other method simply uses software AEAD to encrypt and decrypt the messages sent.

More in-depth information about the encryption methods of the framework can be found at the top of the EspnowMeshBackend.h and EspnowProtocolInterpreter.h files.

A brief overview of the advantages of each method:

AEAD

* The AEAD encryption does not require any pairing, and is thus faster for single messages than establishing a new encrypted connection before transfer.

* AEAD encryption also works with ESP-NOW broadcasts and supports an unlimited number of nodes, which is not true for encrypted connections.

CCMP 

* Using AEAD will only encrypt the message content, not the transmission metadata. CCMP encryption covers both.

* Encrypted ESP-NOW connections come with built in replay attack protection, which is not provided by the framework when using AEAD encryption. 

* Encrypted ESP-NOW connections also allow `EspnowProtocolInterpreter::aeadMetadataSize` extra message bytes per transmission.

* Transmissions via encrypted connections are also slightly faster than via AEAD once a connection has been established.

#### <a name="CCMP"></a>CCMP

For encrypted connections (managed via such methods as `addEncryptedConnection`, `requestEncryptedConnection` and `requestEncryptedConnectionRemoval`), ESP-NOW [uses](https://www.espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf) [CCMP encryption](https://en.wikipedia.org/wiki/CCMP_(cryptography)).
To handle some idiosyncrasies of ESP-NOW (like having no way in the application layer to know if received information is encrypted or not), a separate API layer has been built on top.
This API layer is provided in the hope that it will be useful, but has not been subject to any cryptographic validation (yet, feel free to have a go at it if you have the knowledge).
The goal of the API layer is to ensure that when an encrypted connection is established, the received encrypted messages will both be marked as encrypted and be trustworthy.

Established encrypted connections can be either permanent or temporary. A permanent encrypted connection can only be removed by explicitly calling `removeEncryptedConnection` or `requestEncryptedConnectionRemoval`. A temporary encrypted connection will expire once the duration has passed, although this duration can be updated through the methods used for adding new encrypted connections.

The maximum number of simultaneous encrypted connections is restricted by the ESP-NOW specifications and is `EspnowProtocolInterpreter::maxEncryptedConnections` (6 by default). If required, a stricter soft upper limit can be used for the number of encrypted connections a node can have when receiving encrypted connection requests, to ensure there is normally some margin to the hard maximum. This is handled via the`setEncryptedConnectionsSoftLimit` method.

The internal state of an encrypted connection will be lost if the ESP8266 is restarted or loses power, meaning encrypted messages will no longer be received. There is however functionality available to serialize the state of an encrypted connection so it can be restored later. The HelloEspnow.ino example file shows how this is done. Of course, a stored state should only be used once, since the communication will otherwise be susceptible to replay attacks. See "[Serialization and the internal state of a node](#FloodingMeshSerialization)" in the FloodingMesh docs for more info.

Some security considerations for CCMP encrypted connections are listed below.

* Part of the separate API layer uses the internal hardware random number generator of the ESP8266 (via `ESP.random()`) to initialize the connection state. This may or may not have enough entropy for your security needs.
For an even more random (but slower) number generation, you may want to replace the use of plain `ESP.random()` with something else.

* Since there is no way to know whether a received transmission is encrypted or not via the default ESP-NOW API, an attacker can send unencrypted ESP-NOW messages which pretend to be encrypted without this being detected by the application. To prevent such attacks from happening, this framework uses an extra 64 bit session key for all encrypted connections. A message is only accepted as encrypted if it has the correct session key. 64 bits are used mainly because the uint64_t datatype is the largest natively supported by the ESP8266 Arduino Core, and because each ESP-NOW transmission has a relatively small maximum capacity of 250 bytes.

* The ESP-NOW CCMP encryption should according to the standard have replay attack protection built in, but there is no official documentation from Espressif about this. The 64 bit session key used for encrypted connections, as described above, will however also ensure replay protection.

* The maximum rate at which a potential attacker can poll a session key (via unencrypted transmissions pretending to be encrypted transmissions) is around 0.3 keys per ms, but in practice this rate would render the node completely unresponsive and is thus easily detected.
Assuming the rate above is used that would mean that an attacker in one day could try 0.3 x 1000 x 60 x 60 x 24 = 25 920 000 keys, which is roughly 1/711 600 000 000 of the total (total is 2^(64) - 2^(32), the top 32 session key bits are all 0 when the transmission is unencrypted).

* Should there be a need for even more security, the user could enhance the library with 128 bit (or more) session keys, or ensure CCMP encrypted messages are sent frequently since this will rehash the session key every time, or frequently remove and re-add the encrypted connections (which will cause the session keys to be randomized or set to the supplied values).

#### <a name="AEAD"></a>Authenticated Encryption with Associated Data (AEAD)

In addition to using encrypted ESP-NOW connections the framework can send automatically encrypted messages (using AEAD) over both encrypted and unencrypted connections. This message encryption is conditioned on the `useEncryptedMessages()` flag of the EspnowMeshBackend. Typically, activating the AEAD encryption would be done like so:
```
espnowBackendInstance.setEspnowMessageEncryptionKey(F("ChangeThisKeySeed_TODO")); // The message encryption key should always be set manually. Otherwise a default key (all zeroes) is used.
espnowBackendInstance.setUseEncryptedMessages(true);
```

The AEAD protocol uses the ChaCha20 stream cipher with Poly1305 for message authentication.
More information about this encryption standard can be found here: https://tools.ietf.org/html/rfc7539 , https://tools.ietf.org/html/rfc8439

## <a name="FloodingMeshMore"></a>FloodingMesh

**Important:** As of now, the ESP8266 must have the AP active to receive mesh messages (either via AP mode (use only if CCMP encryption is not required) or AP+STA mode). Messages can however be transmitted even when the AP is turned off. This is limited by the Espressif binary in the ESP8266 Arduino Core and so cannot be corrected by the library code.

***

As the name implies, FloodingMesh is a simple flooding mesh architecture, which means it stores no mesh network routing data in the nodes but only passes new messages on to all surrounding nodes. It therefore has no RAM overhead for network size, which is important for the ESP8266 since available RAM is very limited. The downside is that there is a lot of network traffic for each sent message, and all nodes use the same WiFi channel, so especially for dense networks a lot of interference will be created. Based on tests, a mesh with 30 nodes close together (-44 dBm RSSI) will work well (1-2 dropped messages of 1000). A mesh with around 160 nodes close together will not work at all (though this would probably be solved by spreading out the nodes more, so the interference is reduced).

The FloodingMesh exclusively uses the `EspnowMeshBackend`. The mesh network size is only limited by available MAC addresses, so the maximum is (2^48)/2 = 140 trillion give or take. However, the maximum throughput of the FloodingMesh is around 100 messages per second with 234 bytes per message, so using the maximum number of nodes is not recommended in most cases. Note that while ASCII characters require 1 message byte each, non-ASCII characters usually require 2 message bytes each.

### <a name="FloodingMeshUsage"></a>Usage

There are two primary ways to send a message in FloodingMesh: `broadcast` and `encryptedBroadcast`.

Messages sent via `encryptedBroadcast` use CCMP encryption. Messages sent via `broadcast` are by default unencrypted, but can optionally be encrypted with AEAD encryption. See the "[Encryption](#EspnowMeshBackendEncryption)" segment of the EspnowMeshBackend documentation for more information on the forms of encryption.

The main advantage of `encryptedBroadcast` over `broadcast` is that replay attack protection comes built-in. However, `encryptedBroadcast` is currently slow and experimental so for now `broadcast` is the recommended method to use. This means that replay attacks must be handled separately in a manner suitable for your application (e.g. by adding a counter to your messages or just by designing your application so repeated messages is not an issue).

When `broadcast` is used, the message is sent to all surrounding nodes in one transmission without any WiFi scan.

When a FloodingMesh node receives a message it will first check in its logs to see if the message ID has been received before. If the message ID is not found, the message will be passed to the `meshMessageHandler` of the FloodingMesh instance.

If `meshMessageHandler` returns `false`, the message will not be propagated from the node. If `meshMessageHandler` returns `true`, the message (including any modifications made to it by the `meshMessageHandler`) will be stored in the `forwardingBacklog`. Messages stored in this way are automatically sent to all surrounding nodes via a new `broadcast` or `encryptedBroadcast` (same method as used for the received message) whenever `performMeshMaintenance()`, `performMeshInstanceMaintenance()` or `floodingMeshDelay` is called.

For advanced users, the behaviour of FloodingMesh can easily be modified on the fly by changing the callbacks of the EspnowMeshBackend instance used by the FloodingMesh. The default behaviour can then be restored by calling the `restore` method for the respective callbacks. E.g. messages to forward in the FloodingMesh are by default stored in the `_defaultRequestHandler`, so call `floodingMeshInstance.getEspnowMeshBackend().setRequestHandler` with your own `requestHandler` function to modify this behaviour.

More details can be found in the source code comments of both FloodingMesh and EspnowMeshBackend, as well as in the included HelloMesh example. The main function to modify in the example is `meshMessageHandler`. You can also change the `useLED` variable in the example to `true` if you have built-in LEDs on your ESP8266s to get visual feedback on how the message is spread through the mesh network.

Note that there is no mesh recovery code in the HelloMesh example. It only selects one node (which is marked via the onboard LED if the `useLED` variable is `true`) and makes it continuously transmit. So if the selected node goes offline, no new transmissions will be made. One way to make the example mesh recover is to add a timeout to re-start the selection process if no message is received after a while. However, in practice you will probably want most or all nodes to broadcast their own messages, not just one selected node, so such a recovery timeout will not be useful in that context.

**I want to know all the nodes in my FloodingMesh. What do I do?**

To get a list of all nodes in the HelloMesh.ino example, you will have to make broadcast transmissions such as `floodingMesh.broadcast("Register MAC");` and then add code to register previously unknown `meshInstance.getOriginMac()` in the `meshMessageHandler`.

**What's the best method to get the number of FloodingMesh nodes around me?**

You could do a WiFi scan if you just want to see the nodes around you (if WiFi AP is enabled). Or you could make the nodes transmit and pick up the MACs with `meshInstance.getEspnowMeshBackend().getSenderMac()` in the `meshMessageHandler`.

### <a name="FloodingMeshNote"></a>Note

Since FloodingMesh is based on EspnowMeshBackend, it shares all the limitations described for that backend above. In addition there are some more specific issues to keep in mind.

* The network needs enough time to re-broadcast messages. In practice, if the mesh transmits more than 100 new messages per second (in total), there is a risk of running out of RAM since more messages will be received by the nodes than they can re-transmit.

* A too low value for `messageLogSize` can result in a broadcast storm since the number of "active" messages will be greater than the log size, resulting in messages that bounce around in the network without end. The message log stores all unique FloodingMesh message IDs seen by a node, with more recent IDs replacing the older ones when `messageLogSize` is reached. This means that a node in a mesh network containing 2 nodes will have to send `messageLogSize + 1` transmissions to cause the message log of the other node to forget the first message, while a node in a mesh network containing 101 nodes will have to send 1 % as many messages (on average) to do the same.

   Use `FloodingMesh::setMessageLogSize` to adapt the log size to your needs. A larger log size will of course lead to a higher RAM usage.

### <a name="FloodingMeshSerialization">Serialization and the internal state of a node

The internal state of a node will be lost if it is restarted or loses power. There is however a method called `serializeMeshState()` available in FloodingMesh to serialize the state of a node so it can be restored later. Of course, a stored state should only be used once, since the communication will otherwise be susceptible to replay attacks.

For the node state of FloodingMesh there are a few things to keep in mind.

1. If you use the serialization functionality everything should just work.
2. If all nodes go to sleep without serializing, they will of course lose their memory but the network will be recreated and work as normal when the nodes wake up.
3. If only some nodes go to sleep without serializing the state, things get more complicated. The following is possible: 
   * If you use `encryptedBroadcast`, the nodes that wake up may silently ignore messages forever from the nodes they used to have an encrypted connection with.
   * If you do not use `encryptedBroadcast` the ESP-NOW backend will by default clear its message ID logs in 2.5 seconds (`logEntryLifetimeMs`) and FloodingMesh will have done the same after 100 new message IDs have been received (`messageLogSize`). Once the logs of both classes have been cleared, things will work as normal. Before that, any new message the awoken node sends may have the same ID as an old message, and will then be silently ignored by the receiver.

The messageID is always used together with the node MAC of the sender. For details on how the ID is generated, check out the `generateMessageID` methods.

It is important to realize that there is no global message ID counter, only the local received message IDs for each node in the network. Automatic resynchronizing with this local value is currently only supported for encrypted connections, which exist exclusively between two nodes. For unencrypted connections, `addUnencryptedConnection` may be used manually for similar purposes.

## <a name="FAQ"></a>FAQ

### <a name="FAQStartupCrash"></a>My ESP8266 crashes on start-up when I use the library!

This could be caused by incorrect arguments to the constructors of the library. Usually you would get a Serial Monitor print of the error in question, but if the constructor is called before you call `Serial.begin(115200)` then there will be nothing to print to. The solution is first to check so that all constructor arguments are valid, e.g. that the mesh password has the correct length and does not contain any forbidden characters. If everything checks out you can try to move all the library constructors you use into the `setup()` function of your sketch, after the position where `Serial.begin(115200)` is called. That should give you a proper error message in the Serial Monitor, so you can locate the problem.

### <a name="FAQSSIDAmnesia"></a>The node does not remember the SSID I assign to it!

All example files use `WiFi.persistent(false)` in the `setup()` function, so if you switch the AP off and on again only by using `WiFi.mode()` without the framework methods (`activateAP`/`deactivateAP`), it is likely your last persisted SSID is used, not the one you set in the FloodingMesh/EspnowMeshBackend/TcpIpMeshBackend constructor. The solution is to always use the framework methods to turn the AP on and off, or to follow the instructions below for controlling WiFi mode.

### <a name="FAQModeControl"></a>I want to control the WiFi mode myself.

By default the mesh library assumes it is the only code in charge of managing the WiFi. So it expects to be the middle man when the user wants to do something WiFi related.

That being said, there are some relatively simple ways to go around this. Note that the steps below are not officially supported and may break in future library versions.

The key to solving this is to note that the only methods of EspnowMeshBackend and FloodingMesh which interact with the WiFi mode is `begin()`, `activateAP()` and `deactivateAP()` (for TcpIpMeshBackend `attemptTransmission` should be added to this list). Let's take a look at the methods:

```
void EspnowMeshBackend::begin()
{
  if(!getAPController()) // If there is no active AP controller
    WiFi.mode(WIFI_STA); // WIFI_AP_STA mode automatically sets up an AP, so we can't use that as default.

  activateEspnow();
}

void MeshBackendBase::activateAP()
{
  // Deactivate active AP to avoid two servers using the same port, which can lead to crashes.
  if(MeshBackendBase *currentAPController = MeshBackendBase::getAPController())
    currentAPController->deactivateAP();

  activateAPHook();
  
  WiFi.mode(WIFI_AP_STA); 

  apController = this;
}

void MeshBackendBase::activateAPHook()
{
  WiFi.softAP( getSSID().c_str(), getMeshPassword().c_str(), getWiFiChannel(), getAPHidden() ); // Note that a maximum of 8 TCP/IP stations can be connected at a time to each AP, max 4 by default.
}

void MeshBackendBase::deactivateAP()
{
  if(isAPController())
  {
    deactivateAPHook();

    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);

    // Since there is no active AP controller now, make the apController variable point to nothing. 
    apController = nullptr;
  }
}

void MeshBackendBase::deactivateAPHook()
{
}
```

As you can see, there is nothing in `activateAP` and `deactivateAP` that you cannot do yourself. You do not have to worry about`apController` since it is only used if the mesh library is actually managing an AP (i.e. if `activateAP()` has been called), and the rest is standard Arduino Core WiFi calls. All you have to do then is to call `begin()` once when your program starts and then take responsibility yourself for activating and deactivating an AP with the correct SSID. Essentially, you would create the following function:

```
void myActivateAP()
{
  WiFi.softAP( SSID, password, WiFiChannel ); // You can store these values in the mesh backend and call the respective getters, but then you also have to set the backend values whenever they change.
  WiFi.mode(WIFI_AP_STA); // Can also be WiFi.mode(WIFI_AP)
}
```

Please note that having an AP active is required when receiving broadcasts with FloodingMesh and EspnowMeshBackend (transmitting broadcasts work even when the AP is off). The regular `attemptTransmission` method will transmit even to nodes that have their AP turned off if the recipient STA MAC is already known (then you can set WiFi mode to any mode you like, apart from `WIFI_OFF`).

When an AP is required, AP+STA mode is used in the ESP-NOW backend to keep compatibility with the TCP/IP backend (both backends can be used at the same time). The reason AP+STA mode is used in the TCP/IP backend can be found in TcpIpMeshBackend.cpp : "Unlike WiFi.mode(WIFI_AP);, WiFi.mode(WIFI_AP_STA); allows us to stay connected to the AP we connected to in STA mode, at the same time as we can receive connections from other stations."
Also, AP+STA mode allows encrypted ESP-NOW connections to recover from failure in some cases.

So in summary, you can solve this by calling `begin()` once and then only using the library methods that do not interact with the WiFi mode. As long as you manage your own AP.

### <a name="FAQInterference"></a>I have a lot of interference from all the nodes that are close to each other. What can I do?

In general, you can switch WiFi channel for some nodes (use only channel 1, 6 and 11 for optimal spread, remember that nodes on different WiFi channels cannot communicate directly with each other), try to improve signal quality, or try to reduce interference by reducing the amount of transmissions in the network.

If using FloodingMesh you can try to experiment with reducing error rates by using the mesh method `void setBroadcastReceptionRedundancy(uint8_t redundancy);` (default 2) at the cost of more RAM.


With both FloodingMesh and the EspnowMeshBackend it is possible to use `floodingMesh.getEspnowMeshBackend().setBroadcastTransmissionRedundancy(uint8_t redundancy)` (default 1) to increase the chance of a message arriving, at the cost of longer transmission times.

For reducing the amount of transmissions in the network, that will either require you to optimize your transmission usage or reduce the amount of background protocol transmissions. The latter option is described in greater detail in the two answers below.

### <a name="FAQBeaconInterval"></a>How do I change the interval of the WiFi AP beacon broadcast?

Currently this requires hacking your Arduino Core source files. At [line 122](https://github.com/esp8266/Arduino/blob/8ee67ab2b53463466fd9f035eef2c542ad9a6775/libraries/ESP8266WiFi/src/ESP8266WiFiAP.cpp#L122) in `ESP8266WiFiAP.cpp` you will find the following line `conf.beacon_interval = 100;` (within the `softAp` method). You can change 100 to any value in the range [100, 60000] ms. If you are having problems with too many AP beacon broadcasts in a mesh network, increasing this value should help you with that. To prevent all nodes from beaconing at the same time, delay initial AP activation by a random value in the range [0, x] and then change `conf.beacon_interval` to x, for some large value x <= 60000 ms (same for all nodes).

### <a name="FAQPassiveScan"></a>My ESP is ignoring the WiFi AP beacon broadcast interval settings you just told me about above! (a.k.a. How do I change the WiFi scan mode to passive?)

The default WiFi scan mode of the ESP8266 is active. This triggers a probe response by all AP:s that receives the probe request from the scan. So setting a different beacon interval time has little effect on the background transmission activity if a lot of active scans happen, since all nodes will start performing probe responses (at the same time) in response to the scans.

However, we can change the scan mode so it is passive instead! That will avoid a flood of probe responses after every scan. The downside is that your scan will only detect the nodes that happen to beacon during the scan time. Since you may be able to use ESP-NOW broadcasts instead of AP beacons for node detection, this is perhaps not a problem if you just want to reduce background transmission activity as much as possible to reduce interference.

Note though, that any device that uses active WiFi scans will trigger probe responses from the ESP8266, including smartphones and laptops. So even if you make all ESPs use passive scans, you can still end up with a lot of probe responses from the ESPs if they are close to other devices. The only way to fix this would be to disable the AP of the ESP8266, which of course will make it impossible to find the node via a WiFi scan, and also seems to make it impossible to receive ESP-NOW broadcasts (sending ESP-NOW broadcasts still work though, see the "[Note](#EspnowMeshBackendNote)" section of the EspnowMeshBackend documentation for more on this).

To change the WiFi scan mode to passive, the following information is helpful:
1. A `scan_config` struct is found in `user_interface.h` (and the ESP8266 API documentation). We want to modify `scan_type`, but note that `scan_time` can also be set here if we want faster or slower scans.
2. In `ESP8266WiFiScan.cpp` one can find the following variable declaration: `struct scan_config config;` around line 87. Adding `config.scan_type = WIFI_SCAN_TYPE_PASSIVE;` after `memset(&config, 0, sizeof(config));` on line 88 will ensure passive scans are used.

### <a name="FAQSlowRouter"></a>My internet is slower when I connect the ESP8266 to my router!
There has been some reports about this happening when the ESP8266 is in AP+STA mode while connected to the router. The ESP8266 automatically switches to 802.11g in AP+STA mode, so if your router normally uses a faster WiFi standard such as 802.11n or 802.11ac the router may change mode of operation to 802.11g. Typically this would result in a maximum WiFi speed of around 30 Mbit/s.

A possible workaround is to use only AP mode or STA mode (see "[I want to control the WiFi mode myself](#FAQModeControl)"), perhaps with an extra ESP8266 in one of these modes as a buffer between your ESP8266 mesh network and your router. Remember that the ESP8266 must have the AP active in order to receive ESP-NOW broadcast messages.

Another possible workaround is to try with a different router or router firmware.
