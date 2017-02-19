:orphan:

Station
-------

Example of connecting to an access point has been shown in chapter `Quick Start <readme.md#quick-start>`__. In case connection is lost, ESP8266 will automatically reconnect to the last used access point, once it is again available.

Can we provide more robust connection to Wi-Fi than that?

Table of Contents
-----------------

-  `Introduction <#introduction>`__
-  `Prepare Access Points <#prepare-access-points>`__
-  `Try it Out <#try-it-out>`__
-  `Can we Make it Simpler? <#can-we-make-it-simpler>`__
-  `Conclusion <#conclusion>`__

Introduction
~~~~~~~~~~~~

Following the example in\ `Quick Start <readme.md#quick-start>`__, we would like to go one step further and made ESP connect to next available access point if current connection is lost. This functionality is provided with 'ESP8266WiFiMulti' class and demonstrated in sketch below.

.. code:: cpp

    #include <ESP8266WiFi.h>
    #include <ESP8266WiFiMulti.h>

    ESP8266WiFiMulti wifiMulti;
    boolean connectioWasAlive = true;

    void setup()
    {
      Serial.begin(115200);
      Serial.println();

      wifiMulti.addAP("primary-network-name", "pass-to-primary-network");
      wifiMulti.addAP("secondary-network-name", "pass-to-secondary-network");
      wifiMulti.addAP("tertiary-network-name", "pass-to-tertiary-network");
    }

    void monitorWiFi()
    {
      if (wifiMulti.run() != WL_CONNECTED)
      {
        if (connectioWasAlive == true)
        {
          connectioWasAlive = false;
          Serial.print("Looking for WiFi ");
        }
        Serial.print(".");
        delay(500);
      }
      else if (connectioWasAlive == false)
      {
        connectioWasAlive = true;
        Serial.printf(" connected to %s\n", WiFi.SSID().c_str());
      }
    }

    void loop()
    {
      monitorWiFi();
    }

Prepare Access Points
~~~~~~~~~~~~~~~~~~~~~

To try this sketch in action you need two (or more) access points. In lines below replace ``primary-network-name`` and ``pass-to-primary-network`` with name and password to your primary network. Do the same for secondary network.

.. code:: cpp

    wifiMulti.addAP("primary-network-name", "pass-to-primary-network");
    wifiMulti.addAP("secondary-network-name", "pass-to-secondary-network");

You may add more networks if you have more access points.

.. code:: cpp

    wifiMulti.addAP("tertiary-network-name", "pass-to-tertiary-network");
    ...

Try it Out
~~~~~~~~~~

Now upload updated sketch to ESP module and open serial monitor. Module will first scan for available networks. Then it will select and connect to the network with stronger signal. In case connection is lost, module will connect to next one available.

This process may look something like:

::

    Looking for WiFi ..... connected to sensor-net-1
    Looking for WiFi ....... connected to sensor-net-2
    Looking for WiFi .... connected to sensor-net-1

In above example ESP connected first to ``sensor-net-1``. Then I have switched ``sensor-net-1`` off. ESP discovered that connection is lost and started searching for another configured network. That happened to be ``sensor-net-2`` so ESP connected to it. Then I have switched ``sensor-net-1`` back on and shut down ``sensor-net-2``. ESP reconnected automatically to ``sensor-net-1``.

Function ``monitorWiFi()`` is in place to show when connection is lost by displaying ``Looking for WiFi``. Dots ``....`` are displayed during process of searching for another configured access point. Then a message like ``connected to sensor-net-2`` is shown when connection is established.

Can we Make it Simpler?
~~~~~~~~~~~~~~~~~~~~~~~

Please note that you may simplify this sketch by removing function ``monitorWiFi()`` and putting inside ``loop()`` only ``wifiMulti.run()``. ESP will still reconnect between configured access points if required. Now you won't be able to see it on serial monitor unless you add ``Serial.setDebugOutput(true)`` as described in point `Enable Wi-Fi Diagnostic <readme.md#enable-wi-fi-diagnostic>`__.

Updated sketch for such scenario will look as follows:

.. code:: cpp

    #include <ESP8266WiFi.h>
    #include <ESP8266WiFiMulti.h>

    ESP8266WiFiMulti wifiMulti;

    void setup()
    {
      Serial.begin(115200);
      Serial.setDebugOutput(true);
      Serial.println();

      wifiMulti.addAP("primary-network-name", "pass-to-primary-network");
      wifiMulti.addAP("secondary-network-name", "pass-to-secondary-network");
      wifiMulti.addAP("tertiary-network-name", "pass-to-tertiary-network");
    }

    void loop()
    {
      wifiMulti.run();
    }

That's it! This is really all the code you need to make ESP automatically reconnecting between available networks.

After uploading sketch and opening the serial monitor, the messages will look as below.

*Initial connection to sensor-net-1 on power up:*

::

    f r0, scandone
    f r0, scandone
    state: 0 -> 2 (b0)
    state: 2 -> 3 (0)
    state: 3 -> 5 (10)

    add 0
    aid 1
    cnt
    chg_B1:-40

    connected with sensor-net-1, channel 1
    dhcp client start...
    ip:192.168.1.10,mask:255.255.255.0,gw:192.168.1.9

*Lost connection to sensor-net-1 and establishing connection to sensor-net-2:*

::

    bcn_timout,ap_probe_send_start
    ap_probe_send over, rest wifi status to disassoc
    state: 5 -> 0 (1)
    rm 0
    f r-40, scandone
    f r-40, scandone
    f r-40, scandone
    state: 0 -> 2 (b0)
    state: 2 -> 3 (0)
    state: 3 -> 5 (10)
    add 0

    aid 1
    cnt

    connected with sensor-net-2, channel 11
    dhcp client start...
    ip:192.168.1.102,mask:255.255.255.0,gw:192.168.1.234

*Lost connection to sensor-net-2 and establishing connection back to sensor-net-1:*

::

    bcn_timout,ap_probe_send_start
    ap_probe_send over, rest wifi status to disassoc
    state: 5 -> 0 (1)
    rm 0
    f r-40, scandone
    f r-40, scandone
    f r-40, scandone
    state: 0 -> 2 (b0)
    state: 2 -> 3 (0)
    state: 3 -> 5 (10)
    add 0
    aid 1
    cnt

    connected with sensor-net-1, channel 6
    dhcp client start...
    ip:192.168.1.10,mask:255.255.255.0,gw:192.168.1.9

Conclusion
~~~~~~~~~~

I believe the minimalist sketch with ``ESP8266WiFiMulti`` class is a cool example what ESP8266 can do for us behind the scenes with just couple lines of code.

As shown in above example, reconnecting between access points takes time and is not seamless. Therefore, in practical applications, you will likely need to monitor connection status to decide e.g. if you can send the data to external system or should wait until connection is back.

For detailed review of functions provided to manage station mode please refer to the :doc:`Station Class <station-class>` documentation.
