:orphan:

Generic Class
-------------

Methods and properties described in this section are specific to ESP8266. They are not covered in `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ documentation. Before they are fully documented please refer to information below.

onEvent
~~~~~~~

.. code:: cpp

    void  onEvent (WiFiEventCb cb, WiFiEvent_t event=WIFI_EVENT_ANY) __attribute__((deprecated))

To see how to use ``onEvent`` please check example sketch `WiFiClientEvents.ino <https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiClientEvents/WiFiClientEvents.ino>`__ available inside examples folder of the ESP8266WiFi library.

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

To see a sample application with ``WiFiEventHandler``, please check separate section with `examples :arrow\_right: <generic-examples.rst>`__ dedicated specifically to the Generic Class..

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

Other Function Calls
~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    int32_t  channel (void)
    bool  setSleepMode (WiFiSleepType_t type)
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

Documentation for the above functions is not yet prepared.

For code samples please refer to separate section with `examples <generic-examples.rst>`__ dedicated specifically to the Generic Class.
