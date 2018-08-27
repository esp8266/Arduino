:orphan:

Scan Class
~~~~~~~~~~

This class is represented in `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ by `scanNetworks() <https://www.arduino.cc/en/Reference/WiFiScanNetworks>`__ function. Developers of esp8266 / Arduino core extend this functionality by additional methods and properties.

Documentation of this class is divided into two parts. First covers functions to scan for available networks. Second describes what information is collected during scanning process and how to access it.

Scan for Networks
~~~~~~~~~~~~~~~~~

Scanning for networks takes hundreds of milliseconds to complete. This may be done in a single run when we are triggering scan process, waiting for completion, and providing result - all by a single function. Another option is to split this into steps, each done by a separate function. This way we can execute other tasks while scanning is in progress. This is called asynchronous scanning. Both methods of scanning are documented below.

scanNetworks
^^^^^^^^^^^^

Scan for available Wi-Fi networks in one run and return the number of networks that has been discovered.

.. code:: cpp

    WiFi.scanNetworks() 

There is on `overload <https://en.wikipedia.org/wiki/Function_overloading>`__ of this function that accepts two optional parameters to provide extended functionality of asynchronous scanning as well as looking for hidden networks.

.. code:: cpp

    WiFi.scanNetworks(async, show_hidden) 

Both function parameters are of ``boolean`` type. They provide the flowing functionality: \* ``asysnc`` - if set to ``true`` then scanning will start in background and function will exit without waiting for result. To check for result use separate function ``scanComplete`` that is described below. \* ``show_hidden`` - set it to ``true`` to include in scan result networks with hidden SSID.

scanComplete
^^^^^^^^^^^^

Check for result of asynchronous scanning.

.. code:: cpp

    WiFi.scanComplete() 

On scan completion function returns the number of discovered networks.

If scan is not done, then returned value is < 0 as follows: \* Scanning still in progress: -1 \* Scanning has not been triggered: -2

scanDelete
^^^^^^^^^^

Delete the last scan result from memory.

.. code:: cpp

    WiFi.scanDelete() 

scanNetworksAsync
^^^^^^^^^^^^^^^^^

Start scanning for available Wi-Fi networks. On completion execute another function.

.. code:: cpp

    WiFi.scanNetworksAsync(onComplete, show_hidden) 

| Function parameters: \* ``onComplete`` - the event handler executed
  when the scan is done
| \* ``show_hidden`` - optional ``boolean`` parameter, set it to
  ``true`` to scan for hidden networks

*Example code:*

.. code:: cpp

    #include "ESP8266WiFi.h"

    void prinScanResult(int networksFound)
    {
      Serial.printf("%d network(s) found\n", networksFound);
      for (int i = 0; i < networksFound; i++)
      {
        Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
      }
    }


    void setup()
    {
      Serial.begin(115200);
      Serial.println();

      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);

      WiFi.scanNetworksAsync(prinScanResult);
    }


    void loop() {}

*Example output:*

::

    5 network(s) found
    1: Tech_D005107, Ch:6 (-72dBm)
    2: HP-Print-A2-Photosmart 7520, Ch:6 (-79dBm)
    3: ESP_0B09E3, Ch:9 (-89dBm) open
    4: Hack-4-fun-net, Ch:9 (-91dBm)
    5: UPC Wi-Free, Ch:11 (-79dBm)

Show Results
~~~~~~~~~~~~

Functions below provide access to result of scanning. It does not matter if scanning has been done in synchronous or asynchronous mode, scan results are available using the same API.

Individual results are accessible by providing a \`networkItem' that identifies the index (zero based) of discovered network.

SSID
^^^^

Return the SSID of a network discovered during the scan.

.. code:: cpp

    WiFi.SSID(networkItem) 

Returned SSID is of the ``String`` type. The ``networkItem`` is a zero based index of network discovered during scan.

encryptionType
^^^^^^^^^^^^^^

Return the encryption type of a network discovered during the scan.

.. code:: cpp

    WiFi.encryptionType(networkItem) 

Function returns a number that encodes encryption type as follows: \* 5
: ``ENC_TYPE_WEP`` - WEP \* 2 : ``ENC_TYPE_TKIP`` - WPA / PSK \* 4 :
``ENC_TYPE_CCMP`` - WPA2 / PSK \* 7 : ``ENC_TYPE_NONE`` - open network
\* 8 : ``ENC_TYPE_AUTO`` - WPA / WPA2 / PSK

The ``networkItem`` is a zero based index of network discovered during scan.

RSSI
^^^^

Return the `RSSI <https://en.wikipedia.org/wiki/Received_signal_strength_indication>`__ (Received Signal Strength Indication) of a network discovered during the scan.

.. code:: cpp

    WiFi.RSSI(networkItem) 

Returned RSSI is of the ``int32_t`` type. The ``networkItem`` is a zero based index of network discovered during scan.

BSSID
^^^^^

Return the `BSSID <https://en.wikipedia.org/wiki/Service_set_(802.11_network)#Basic_service_set_identification_.28BSSID.29>`__ (Basic Service Set Identification) that is another name of MAC address of a network discovered during the scan.

.. code:: cpp

    WiFi.BSSID(networkItem) 

Function returns a pointer to the memory location (an ``uint8_t`` array with the size of 6 elements) where the BSSID is saved.

If you do not like to pointers, then there is another version of this function that returns a ``String``.

.. code:: cpp

    WiFi.BSSIDstr(networkItem) 

The ``networkItem`` is a zero based index of network discovered during scan.

channel
^^^^^^^

Return the channel of a network discovered during the scan.

.. code:: cpp

    WiFi.channel(networkItem) 

Returned channel is of the ``int32_t`` type. The ``networkItem`` is a zero based index of network discovered during scan.

isHidden
^^^^^^^^

Return information if a network discovered during the scan is hidden or not.

.. code:: cpp

    WiFi.isHidden(networkItem)

Returned value if the ``bolean`` type, and ``true`` means that network is hidden. The ``networkItem`` is a zero based index of network discovered during scan.

getNetworkInfo
^^^^^^^^^^^^^^

Return all the network information discussed in this chapter above in a single function call.

.. code:: cpp

    WiFi.getNetworkInfo(networkItem, &ssid, &encryptionType, &RSSI, *&BSSID, &channel, &isHidden) 

The ``networkItem`` is a zero based index of network discovered during scan. All other input parameters are passed to function by reference. Therefore they will be updated with actual values retrieved for particular ``networkItem``. The function itself returns ``boolean`` ``true`` or ``false`` to confirm if information retrieval was successful or not.

*Example code:*

.. code:: cpp

    int n = WiFi.scanNetworks(false, true);

    String ssid;
    uint8_t encryptionType;
    int32_t RSSI;
    uint8_t* BSSID;
    int32_t channel;
    bool isHidden;

    for (int i = 0; i < n; i++)
    {
      WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s %s\n", i + 1, ssid.c_str(), channel, RSSI, encryptionType == ENC_TYPE_NONE ? "open" : "", isHidden ? "hidden" : "");
    }

*Example output:*

::

    6 network(s) found
    1: Tech_D005107, Ch:6 (-72dBm)
    2: HP-Print-A2-Photosmart 7520, Ch:6 (-79dBm)
    3: ESP_0B09E3, Ch:9 (-89dBm) open
    4: Hack-4-fun-net, Ch:9 (-91dBm)
    5: , Ch:11 (-77dBm)  hidden
    6: UPC Wi-Free, Ch:11 (-79dBm)

For code samples please refer to separate section with `examples <scan-examples.rst>`__ dedicated specifically to the Scan Class.
