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

To see a sample application with ``WiFiEventHandler``, please check separate section with `examples :arrow\_right: <generic-examples.md>`__ dedicated specifically to the Generic Class..

persistent
~~~~~~~~~~

.. code:: cpp

    WiFi.persistent (persistent) 

Module is able to reconnect to last used Wi-Fi network on power up or reset basing on settings stored in specific sectors of flash memory. By default these settings are written to flash each time they are used in functions like ``WiFi.begin(ssid, password)``. This happens no matter if SSID or password has been actually changed.

This might result in some wear of flash memory depending on how often such functions are called.

Setting ``persistent`` to ``false`` will get SSID / password written to flash only if currently used values do not match what is already stored in flash.

Please note that functions ``WiFi.disconnect`` or ``WiFi.softAPdisconnect`` reset currently used SSID / password. If ``persistent`` is set to ``false``, then using these functions will not affect SSID / password stored in flash.

To learn more about this functionality, and why it has been introduced, check issue report `#1054 <https://github.com/esp8266/Arduino/issues/1054>`__.

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

For code samples please refer to separate section with `examples <generic-examples.md>`__ dedicated specifically to the Generic Class.
