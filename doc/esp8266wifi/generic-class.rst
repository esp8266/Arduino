:orphan:

Generic Class
-------------

Methods and properties described in this section are specific to ESP8266. They are not covered in `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ documentation. Before they are fully documented please refer to information below.

onEvent
~~~~~~~

.. code:: cpp

    void  onEvent (WiFiEventCb cb, WiFiEvent_t event=WIFI_EVENT_ANY) __attribute__((deprecated))


WiFiEventHandler
~~~~~~~~~~~~~~~~

.. code:: cpp

    WiFiEventHandler  onStationModeConnected (std::function< void(const WiFiEventStationModeConnected &)>)
    WiFiEventHandler  onStationModeDisconnected (std::function< void(const WiFiEventStationModeDisconnected &)>)
    WiFiEventHandler  onStationModeAuthModeChanged (std::function< void(const WiFiEventStationModeAuthModeChanged &)>)
    WiFiEventHandler  onStationModeGotIP (std::function< void(const WiFiEventStationModeGotIP &)>)
    WiFiEventHandler  onStationModeDHCPTimeout (std::function< void(void)>)
    WiFiEventHandler  onSoftAPModeStationConnected (std::function< void(const WiFiEventSoftAPModeStationConnected &)>)
    WiFiEventHandler  onSoftAPModeStationDisconnected (std::function< void(const WiFiEventSoftAPModeStationDisconnected &)>)

It should be noted that when an WiFi interface goes down, all WiFiClients are stopped, and all WiFiServers stop serving. When the interface comes up, it is up to the user to reconnect the relevant WiFiClients and bring the WiFiServers back up. 
For the WiFi station interface, it is suggested to set a callback for onStationModeDisconnected() that shuts down the user app's WiFiClients and WiFiServers (resource cleanup), and another callback for onStationModeGotIP() that brings them back up.
For the SoftAP interface, when the interface is brought up, any servers should be brought up as well.

A detailed explanation of ``WiFiEventHandler`` can be found in the section with `examples :arrow\_right: <generic-examples.rst>`__ dedicated specifically to the Generic Class..

Alternatively, check the example sketch `WiFiEvents.ino <https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiEvents/WiFiEvents.ino>`__ available inside examples folder of the ESP8266WiFi library.


persistent
~~~~~~~~~~

.. code:: cpp

    WiFi.persistent(persistent)

ESP8266 is able to reconnect to the last used Wi-Fi network or establishes the same Access Point upon power up or reset.
By default, these settings are written to specific sectors of flash memory every time they are changed in ``WiFi.begin(ssid, passphrase)`` or ``WiFi.softAP(ssid, passphrase, channel)``, and when ``WiFi.disconnect`` or ``WiFi.softAPdisconnect`` is invoked.
Frequently calling these functions could cause wear on the flash memory (see issue `#1054 <https://github.com/esp8266/Arduino/issues/1054>`__).

Once ``WiFi.persistent(false)`` is called, ``WiFi.begin``, ``WiFi.disconnect``, ``WiFi.softAP``, or ``WiFi.softAPdisconnect`` only changes the current in-memory Wi-Fi settings, and does not affect the Wi-Fi settings stored in flash memory.

mode
~~~~

.. code:: cpp

    WiFi.mode(m)

-  ``WiFi.mode(m)``: set mode to ``WIFI_AP``, ``WIFI_STA``,
   ``WIFI_AP_STA`` or ``WIFI_OFF``

getMode
~~~~~~~

.. code:: cpp

    WiFiMode_t WiFi.getMode()

-  ``WiFi.getMode()``: return current Wi-Fi mode (one out of four modes above)

WiFi power management, DTIM
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    bool setSleepMode (WiFiSleepType_t type, int listenInterval=0)

Sleep mode type is ``WIFI_NONE_SLEEP``, ``WIFI_LIGHT_SLEEP`` or ``WIFI_MODEM_SLEEP``.

(``listenInterval`` appeared in esp8266-arduino core v2.5.0 using the last
V2 revision of nonos-sdk before V3)

Quoting nonos-sdk datasheet:

* ``NONE``: disable power saving

* ``LIGHT`` or ``MODEM``: TCP timer rate raised from 250ms to 3s

When ``listenInterval`` is set to 1..10, in ``LIGHT`` or ``MODEM`` mode,
station wakes up every (DTIM-interval * ``listenInterval``).  This saves
power but station interface may miss broadcast data.

Otherwise (default value 0), station wakes up at every DTIM-interval
(configured in the access-point).

Quoting wikipedia:

A Delivery Traffic Indication Map (DTIM) is a kind of Traffic Indication Map
(TIM) which informs the clients about the presence of buffered
multicast/broadcast data on the access point.  It is generated within the
periodic beacon at a frequency specified by the DTIM Interval.  Beacons are
packets sent by an access point to synchronize a wireless network.


setOutputPower
~~~~~~~~~~~~~~

.. code:: cpp

    void WiFi.setOutputPower(float dBm)

Sets the max transmit power, in dBm. Values range from 0 to 20.5 [dBm] inclusive, and should be multiples of 0.25.
This is essentially a thin wrapper around the SDK's system_phy_set_max_tpw() api call.

If wifi connection issues are encountered due to signal noise, one thing to try is to reduce the Tx power.
This has been found effective in cases where STA mode is in use with 802.11n phy (default). Reducing to
e.g.: 17.5dBm or slightly lower can reduce noise and improve connectivity, although max range will also be reduced.

setPhyMode
~~~~~~~~~~

.. code:: cpp

    bool  setPhyMode (WiFiPhyMode_t mode)

Sets the WiFi radio phy mode. Argument is an enum of type WiFiPhyMode_t, valid values are:
-  ``WIFI_PHY_MODE_11B``: 802.11b mode
-  ``WIFI_PHY_MODE_11G``: 802.11g mode
-  ``WIFI_PHY_MODE_11N``: 802.11n mode

Per the NONOS SDK API Reference document, the AP mode only supports b/g, see notes in section on wifi_set_phy_mode() api.
Returns true success, false otherwise.

Some experiments have shown that 802.11b mode has longest LOS range, while 802.11n mode has longest indoor range.

It has been observed that some wifi routers may degrade from 802.11n to g/b if an ESP8266 in g/b phy mode connects to them. That 
means that the entire wifi connectivity of all devices are impacted.

getPhyMode
~~~~~~~~~~

.. code:: cpp

    WiFiPhyMode_t  getPhyMode (WiFiPhyMode_t mode)

Gets the WiFi radio phy mode that is currently set.

Other Function Calls
~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    int32_t  channel (void)
    WiFiSleepType_t  getSleepMode ()
    bool  enableSTA (bool enable)
    bool  enableAP (bool enable)
    bool  forceSleepBegin (uint32 sleepUs=0)
    bool  forceSleepWake ()
    int  hostByName (const char *aHostname, IPAddress &aResult)

    appeared with SDK pre-V3:
    uint8_t getListenInterval ();
    bool isSleepLevelMax ();


Documentation for the above functions is not yet prepared.

For code samples please refer to separate section with `examples <generic-examples.rst>`__ dedicated specifically to the Generic Class.
