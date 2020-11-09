:orphan:

Scan
~~~~

To connect a mobile phone to a hot spot, you typically open Wi-Fi settings app, list available networks and then pick the hot spot you need. You can also list the networks with ESP8266 and here is how.

Simple Scan
~~~~~~~~~~~

This example shows the bare minimum code we need to check for the list of available networks.

Disconnect
^^^^^^^^^^

To start with, enable module in station mode and then disconnect.

.. code:: cpp

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

Running ``WiFi.disconnect()`` is to shut down a connection to an access point that module may have automatically made using previously saved credentials.

Scan for Networks
^^^^^^^^^^^^^^^^^

After some delay to let the module disconnect, go to scanning for available networks:

.. code:: cpp

    int n = WiFi.scanNetworks();

Now just check if returned ``n`` if greater than 0 and list found networks:

.. code:: cpp

    for (int i = 0; i < n; i++)
    {
      Serial.println(WiFi.SSID(i));
    }

This is that simple.

Complete Example
^^^^^^^^^^^^^^^^

The sketch should have obligatory ``#include <ESP8266WiFi.h>`` and looks as follows:

.. code:: cpp

    #include "ESP8266WiFi.h"

    void setup()
    {
      Serial.begin(115200);
      Serial.println();

      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
    }

    void loop()
    {
      Serial.print("Scan start ... ");
      int n = WiFi.scanNetworks();
      Serial.print(n);
      Serial.println(" network(s) found");
      for (int i = 0; i < n; i++)
      {
        Serial.println(WiFi.SSID(i));
      }
      Serial.println();

      delay(5000);
    }

Example in Action
^^^^^^^^^^^^^^^^^

Upload this sketch to ESP module and open a serial monitor. If there are access points around (sure there are) you will see a similar list repeatedly printed out:

::

    Scan start ... 5 network(s) found
    Tech_D005107
    HP-Print-A2-Photosmart 7520
    ESP_0B09E3
    Hack-4-fun-net
    UPC Wi-Free

When looking for the text ``scan start ...`` displayed, you will notice that it takes noticeable time for the following text ``n network(s) found`` to show up. This is because execution of ``WiFi.scanNetworks()`` takes time and our program is waiting for it to complete before moving to the next line of code. What if at the same time we would like ESP to run time critical process (e.g. animation)
that should not be disturbed?

It turns out that this is fairly easy to do by scanning networks in async mode.

Check it out in next example below that will also demonstrate printing out other parameters of available networks besides SSID.

Async Scan
~~~~~~~~~~

What we like to do, is to trigger process of scanning for networks and then return to executing code inside the ``loop()``. Once scanning is complete, at a convenient time, we will check the list of networks. The "time critical process" will be simulated by a blinking LED at 250ms period.

We would like the blinking pattern not be disturbed at any time.

No delay()
^^^^^^^^^^

To implement such functionality we should refrain from using any ``delay()`` inside the ``loop()``. Instead we will define period when to trigger particular action. Then inside ``loop()`` we will check ``millis()`` (internal clock that counts milliseconds) and fire the action if the period expires.

Please check how this is done in `BlinkWithoutDelay.ino <BlinkWithoutDelay.ino>`__ example sketch. Identical technique can be used to periodically trigger scanning for Wi-Fi networks.

Setup
^^^^^

First we should define scanning period and internal variable ``lastScanMillis`` that will hold time when the last scan has been made.

.. code:: cpp

    #define SCAN_PERIOD 5000
    long lastScanMillis;

When to Start
^^^^^^^^^^^^^

Then inside the ``loop()`` we will check if ``SCAN_PERIOD`` expired, so it is time to fire next scan:

.. code:: cpp

    if (currentMillis - lastScanMillis > SCAN_PERIOD)
    {
      WiFi.scanNetworks(true);
      Serial.print("\nScan start ... ");
      lastScanMillis = currentMillis;
    }

Please note that ``WiFi.scanNetworks(true)`` has an extra parameter ``true`` that was not present in `previous example <#simple-scan>`__ above. This is an instruction to scan in asynchronous mode, i.e. trigger scanning process, do not wait for result (processing will be done in background) and move to the next line of code. We need to use asynchronous mode otherwise 250ms LED blinking pattern would be disturbed as scanning takes longer than 250ms.

Check When Done
^^^^^^^^^^^^^^^

Finally we should periodically check for scan completion to print out the result once ready. To do so, we will use function ``WiFi.scanComplete()``, that upon completion returns the number of found networks. If scanning is still in progress it returns -1. If scanning has not been triggered yet, it would return -2.

.. code:: cpp

    int n = WiFi.scanComplete();
    if(n >= 0)
    {
      Serial.printf("%d network(s) found\n", n);
      for (int i = 0; i < n; i++)
      {
        Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
      }
      WiFi.scanDelete();
    }

Please note function ``WiFi.scanDelete()`` that is deleting scanning result from memory, so it is not printed out over and over again on each ``loop()`` run.

Complete Example
^^^^^^^^^^^^^^^^

Complete sketch is below. The code inside ``setup()`` is the same as described in `previous example <#simple-scan>`__ except for an additional ``pinMode()`` to configure the output pin for LED.

.. code:: cpp

    #include "ESP8266WiFi.h"

    #define BLINK_PERIOD 250
    long lastBlinkMillis;
    boolean ledState;

    #define SCAN_PERIOD 5000
    long lastScanMillis;


    void setup()
     {
      Serial.begin(115200);
      Serial.println();

      pinMode(LED_BUILTIN, OUTPUT);

      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
    }

    void loop()
    {
      long currentMillis = millis();

      // blink LED
      if (currentMillis - lastBlinkMillis > BLINK_PERIOD)
      {
        digitalWrite(LED_BUILTIN, ledState);
        ledState = !ledState;
        lastBlinkMillis = currentMillis;
      }

      // trigger Wi-Fi network scan
      if (currentMillis - lastScanMillis > SCAN_PERIOD)
      {
        WiFi.scanNetworks(true);
        Serial.print("\nScan start ... ");
        lastScanMillis = currentMillis;
      }

      // print out Wi-Fi network scan result upon completion
      int n = WiFi.scanComplete();
      if(n >= 0)
      {
        Serial.printf("%d network(s) found\n", n);
        for (int i = 0; i < n; i++)
        {
          Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
        }
        WiFi.scanDelete();
      }
    }

Example in Action
^^^^^^^^^^^^^^^^^

Upload above sketch to ESP module and open a serial monitor. You should see similar list printed out every 5 seconds:

::

    Scan start ... 5 network(s) found
    1: Tech_D005107, Ch:6 (-72dBm)
    2: HP-Print-A2-Photosmart 7520, Ch:6 (-79dBm)
    3: ESP_0B09E3, Ch:9 (-89dBm) open
    4: Hack-4-fun-net, Ch:9 (-91dBm)
    5: UPC Wi-Free, Ch:11 (-79dBm)

Check the LED. It should be blinking undisturbed four times per second.

Conclusion
~~~~~~~~~~

The scan class API provides comprehensive set of methods to do scanning in both synchronous as well as in asynchronous mode. Therefore we can easy implement code that is doing scanning in background without disturbing other processes running on ESP8266 module.

For the list of functions provided to manage scan mode please refer to the `Scan Class <scan-class.rst>`__ documentation.
