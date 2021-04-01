:orphan:

Soft Access Point Class
-----------------------

Section below is ESP8266 specific as `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ documentation does not cover soft access point. The API description is broken down into three short chapters. They cover how to setup soft-AP, manage connection, and obtain information on soft-AP interface configuration.

Table of Contents
-----------------

-  `Set up Network <#set-up-network>`__

   -  `softAP <#softap>`__
   -  `softAPConfig <#softapconfig>`__

-  `Manage Network <#manage-network>`__

   -  `softAPdisconnect <#softapdisconnect>`__
   -  `softAPgetStationNum <#softapgetstationnum>`__

-  `Network Configuration <#network-configuration>`__

   -  `softAPIP <#softapip>`__
   -  `softAPmacAddress <#softapmacaddress>`__

Set up Network
~~~~~~~~~~~~~~

This section describes functions to set up and configure ESP8266 in the soft access point (soft-AP) mode.

softAP
^^^^^^

Set up a soft access point to establish a Wi-Fi network.

The simplest version (`an overload in C++
terms <https://en.wikipedia.org/wiki/Function_overloading>`__) of this function requires only one parameter and is used to set up an open Wi-Fi network.

.. code:: cpp

    WiFi.softAP(ssid)

To set up pre-shared key protected network, or to configure additional network parameters, use the following overload:

.. code:: cpp

    WiFi.softAP(ssid, psk, channel, hidden, max_connection)

The first parameter of this function is required, remaining four are optional.

Meaning of all parameters is as follows:

- ``ssid`` - character string containing network SSID (max. 32 characters) 
- ``psk`` - optional character string with a pre-shared key. For WPA2-PSK network it should be minimum 8 characters long and not longer than 64 characters. If not specified, the access point will be open for anybody to connect.
- ``channel`` - optional parameter to set Wi-Fi channel, from 1 to 13. Default channel = 1. 
- ``hidden`` - optional parameter, if set to ``true`` will hide SSID. 
- ``max_connection`` - optional parameter to set max simultaneous connected stations, `from 0 to 8 <https://bbs.espressif.com/viewtopic.php?f=46&t=481&p=1832&hilit=max_connection#p1832>`__. Defaults to 4. Once the max number has been reached, any other station that wants to connect will be forced to wait until an already connected station disconnects.

Function will return ``true`` or ``false`` depending on result of setting the soft-AP.

Notes:

- The network established by softAP will have default IP address of 192.168.4.1. This address may be changed using ``softAPConfig`` (see below). 
- Even though ESP8266 can operate in soft-AP + station mode, it actually has only one hardware channel. Therefore in soft-AP + station mode, the soft-AP channel will default to the number used by station. For more information how this may affect operation of stations connected to ESP8266's soft-AP, please check `this FAQ entry <https://bbs.espressif.com/viewtopic.php?f=10&t=324>`__ on Espressif forum.

softAPConfig
^^^^^^^^^^^^

Configure the soft access point's network interface.

.. code:: cpp

    softAPConfig (local_ip, gateway, subnet) 

All parameters are the type of ``IPAddress`` and defined as follows:

- ``local_ip`` - IP address of the soft access point 
- ``gateway`` - gateway IP address
- ``subnet`` - subnet mask

Function will return ``true`` or ``false`` depending on result of changing the configuration.

*Example code:*

.. code:: cpp

    #include <ESP8266WiFi.h>

    IPAddress local_IP(192,168,4,22);
    IPAddress gateway(192,168,4,9);
    IPAddress subnet(255,255,255,0);

    void setup()
    {
      Serial.begin(115200);
      Serial.println();

      Serial.print("Setting soft-AP configuration ... ");
      Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

      Serial.print("Setting soft-AP ... ");
      Serial.println(WiFi.softAP("ESPsoftAP_01") ? "Ready" : "Failed!");

      Serial.print("Soft-AP IP address = ");
      Serial.println(WiFi.softAPIP());
    }

    void loop() {}

*Example output:*

::

    Setting soft-AP configuration ... Ready
    Setting soft-AP ... Ready
    Soft-AP IP address = 192.168.4.22

Manage Network
~~~~~~~~~~~~~~

Once soft-AP is established you may check the number of stations connected, or shut it down, using the following functions.

softAPgetStationNum
^^^^^^^^^^^^^^^^^^^

Get the count of the stations that are connected to the soft-AP interface.

.. code:: cpp

    WiFi.softAPgetStationNum() 

*Example code:*

.. code:: cpp

    Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());

*Example output:*

::

    Stations connected to soft-AP = 2

Note: the maximum number of stations that may be connected to ESP8266 soft-AP is 4 by default. This can be changed from 0 to 8 via the ``max_connection`` argument of the softAP method.

softAPdisconnect
^^^^^^^^^^^^^^^^

Disconnect stations from the network established by the soft-AP.

.. code:: cpp

    WiFi.softAPdisconnect(wifioff) 

Function will set currently configured SSID and pre-shared key of the soft-AP to null values. The parameter ``wifioff`` is optional. If set to ``true`` it will switch the soft-AP mode off.

Function will return ``true`` if operation was successful or ``false`` if otherwise.

Network Configuration
~~~~~~~~~~~~~~~~~~~~~

Functions below provide IP and MAC address of ESP8266's soft-AP.

softAPIP
^^^^^^^^

Return IP address of the soft access point's network interface.

.. code:: cpp

    WiFi.softAPIP() 

Returned value is of ``IPAddress`` type.

*Example code:*

.. code:: cpp

    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());

*Example output:*

::

    Soft-AP IP address = 192.168.4.1

softAPmacAddress
^^^^^^^^^^^^^^^^

Return MAC address of soft access point. This function comes in two versions, which differ in type of returned values. First returns a pointer, the second a ``String``.

Pointer to MAC
''''''''''''''

.. code:: cpp

    WiFi.softAPmacAddress(mac)

Function accepts one parameter ``mac`` that is a pointer to memory location (an ``uint8_t`` array the size of 6 elements) to save the mac address. The same pointer value is returned by the function itself.

*Example code:*

.. code:: cpp

    uint8_t macAddr[6];
    WiFi.softAPmacAddress(macAddr);
    Serial.printf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);

*Example output:*

::

    MAC address = 5e:cf:7f:8b:10:13

MAC as a String
'''''''''''''''

Optionally you can use function without any parameters that returns a ``String`` type value.

.. code:: cpp

    WiFi.softAPmacAddress()

*Example code:*

.. code:: cpp

    Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());

*Example output:*

::

    MAC address = 5E:CF:7F:8B:10:13

For code samples please refer to separate section with `examples <soft-access-point-examples.rst>`__ dedicated specifically to the Soft Access Point Class.
