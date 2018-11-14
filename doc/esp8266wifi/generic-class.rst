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
    WiFi.getMode()

-  ``WiFi.mode(m)``: set mode to ``WIFI_AP``, ``WIFI_STA``,
   ``WIFI_AP_STA`` or ``WIFI_OFF``
-  ``WiFi.getMode()``: return current Wi-Fi mode (one out of four modes
   above)

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


Other Function Calls
~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    int32_t  channel (void)
    WiFiSleepType_t  getSleepMode ()
    bool  setPhyMode (WiFiPhyMode_t mode)
    WiFiPhyMode_t  getPhyMode ()
    void  setOutputPower (float dBm)
    WiFiMode_t  getMode ()
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
