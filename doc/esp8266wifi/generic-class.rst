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

Alternatively, check the example sketch `WiFiEvents.ino <https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiEvents/WiFiEvents.ino>`__ available in the examples folder of the ESP8266WiFi library.


persistent
~~~~~~~~~~

.. code:: cpp

    WiFi.persistent(persistent)

Starting from version 3 of this core, **persistence is disabled by default
and WiFi does not start automatically at boot** (see PR `#7902 <https://github.com/esp8266/Arduino/pull/7902>`__).

Previously, SDK was automatically starting WiFi at boot.  This was probably
intended for the Espressif AT FW which is interactive and preserves WiFi
state across reboots.  This behavior is generally irrelevant with the
Arduino API because sketches start with ``WiFi.begin()`` or
``WiFi.softAP()``.

This change is harmless with standard sketches: Calls to ``WiFi.mode()`` do
enable radio as usual.  It also smooths current spikes at boot and decreases
DHCP stress.

Known side-effects:

-  ``WiFi.mode()`` must be called before changing mac addresses with ``wifi_set_macaddr({SOFTAP,STATION}_IF, ...)``.

Legacy behavior can be restored by calling ``enableWiFiAtBootTime()`` from
anywhere in the code (it is a weak void function intended to play with the
linker).

.. code:: cpp

    #include <ESP8266WiFi.h>

    void setup () {
    #ifdef WIFI_IS_OFF_AT_BOOT
        enableWiFiAtBootTime(); // can be called from anywhere with the same effect
    #endif
        ....
    }

When legacy behavior is restored thanks to this call,
ESP8266 is able to reconnect to the last used WiFi network or establishes the same Access Point upon power up or reset.
By default, these settings are written to specific sectors of flash memory every time they are changed in ``WiFi.begin(ssid, passphrase)`` or ``WiFi.softAP(ssid, passphrase, channel)``, and when ``WiFi.disconnect`` or ``WiFi.softAPdisconnect`` is invoked.
Frequently calling these functions could cause wear on the flash memory (see issue `#1054 <https://github.com/esp8266/Arduino/issues/1054>`__).

Once ``WiFi.persistent(false)`` is called, ``WiFi.begin``, ``WiFi.disconnect``, ``WiFi.softAP``, or ``WiFi.softAPdisconnect`` only changes the current in-memory WiFi settings, and does not affect the WiFi settings stored in flash memory.

mode
~~~~

.. code:: cpp

    bool mode(WiFiMode_t m)

Switches to one of the regular WiFi modes, where ``m`` is one of:

-  ``WIFI_OFF``: turn WiFi off.
-  ``WIFI_STA``: switch to `Station (STA) <readme.rst#station>`__ mode.
-  ``WIFI_AP``: switch to `Access Point (AP) <readme.rst#soft-access-point>`__ mode.
-  ``WIFI_AP_STA``: enable both Station (STA) and Access Point (AP) mode.

getMode
~~~~~~~

.. code:: cpp

    WiFiMode_t getMode()

Gets the current WiFi mode (one out of four regular modes above).

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

forceSleepBegin
~~~~~~~~~~~~~~~

.. code:: cpp

    bool  forceSleepBegin (uint32 sleepUs=0)

Saves the currently set WiFi mode and starts forced modem sleep for the specified time (us)

forceSleepWake
~~~~~~~~~~~~~~

.. code:: cpp

    bool  forceSleepWake ()

Called after `forceSleepBegin()`. Restores the previous WiFi mode and attempts reconnection when STA was active.

shutdown and resumeFromShutdown
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    bool  shutdown (WiFiState& state)
    bool  shutdown (WiFiState& state, uint32 sleepUs)
    bool  resumeFromShutdown (WiFiState& state)
    bool  shutdownValidCRC (const WiFiState& state)

Stores the STA interface IP configuration in the specified ``state`` struct and calls ``forceSleepBegin(sleepUs)``.
Restores STA interface configuration from the ``state`` and calls ``forceSleepWake()``.

These methods are intended to be used in low-power scenarios, e.g. where ESP.deepSleep is used between actions to preserve battery power. It is the user's responsibility to preserve the WiFiState between ``shutdown()`` and ``resumeFromShutdown()`` by storing it in the RTC user data and/or flash memory.

See `WiFiShutdown.ino <https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiShutdown/WiFiShutdown.ino>`__ for an example of usage.

Other Function Calls
~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    int32_t  channel (void)
    WiFiSleepType_t  getSleepMode ()
    bool  enableSTA (bool enable)
    bool  enableAP (bool enable)
    int  hostByName (const char *aHostname, IPAddress &aResult)


Also, when using NONOS SDK v3:

.. code:: cpp

    uint8_t getListenInterval ();
    bool isSleepLevelMax ();


Documentation for the above functions is not yet prepared.

For code samples please refer to separate section with `examples <generic-examples.rst>`__ dedicated specifically to the Generic Class.
