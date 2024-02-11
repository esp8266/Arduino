:orphan:

Generic
-------

In the first `example <readme.rst#quick-start>`__ of the ESP8266WiFi library documentation we have discussed how to check when module connects to the Wi-Fi network. We were waiting until connection is established. If network is not available, the module could wait like that for ever doing nothing else. Another `example <scan-examples.rst#async-scan>`__ on the Wi-Fi asynchronous scan mode demonstrated how to wait for scan result and do in parallel something else - blink a LED not disturbing the blink pattern. Let's apply similar functionality when connecting the module to an access point.

Table of Contents
-----------------

-  `Introduction <#introduction>`__
-  `What are the Tasks? <#what-are-the-tasks>`__
-  `Event Driven Methods <#event-driven-methods>`__
-  `Register the Events <#register-the-events>`__
-  `The Code <#the-code>`__
-  `Check the Code <#check-the-code>`__
-  `Conclusion <#conclusion>`__

Introduction
~~~~~~~~~~~~

In example below we will show another cool example of getting ESP perform couple of tasks at the same time and with very little programming.

What are the Tasks?
~~~~~~~~~~~~~~~~~~~

We would like to write a code that will inform us that connection to Wi-Fi network has been established or lost. At the same time we want to perform some time critical task. We will simulate it with a blinking LED. Generic class provides specific, event driven methods, that will be executed asynchronously, depending on e.g. connection status, while we are already doing other tasks.

Event Driven Methods
~~~~~~~~~~~~~~~~~~~~

The list of all such methods is provided in `Generic Class <generic-class.rst>`__ documentation.

We would like to use two of them: \* ``onStationModeGotIP`` called when station is assigned IP address. This assignment may be done by DHCP client or by executing ``WiFi.config(...)``. \* ``onStationModeDisconnected`` called when station is disconnected from Wi-Fi network. The reason of disconnection does not matter. Event will be triggered both if disconnection is done from the code by executing ``WiFi.disconnect()``, because the Wi-Fi signal is weak, or because the access point is switched off.

Register the Events
~~~~~~~~~~~~~~~~~~~

To get events to work we need to complete just two steps:

1. Declare the event handler:

.. code:: cpp
    WiFiEventHandler disconnectedEventHandler;

2. Select particular event (in this case ``onStationModeDisconnected``)
   and add the code to be executed when event is fired.

``cpp   disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event)   {     Serial.println("Station disconnected");   });`` If this event is fired the code will print out information that station has been disconnected.

That's it. It is all we need to do.

The Code
~~~~~~~~

The complete code, including both methods discussed at the beginning, is provided below.

.. code:: cpp

    #include <ESP8266WiFi.h>

    const char* ssid = "********";
    const char* password = "********";

    WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

    bool ledState;


    void setup()
    {
      Serial.begin(115200);
      Serial.println();

      pinMode(LED_BUILTIN, OUTPUT);

      gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event)
      {
        Serial.print("Station connected, IP: ");
        Serial.println(WiFi.localIP());
      });

      disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event)
      {
        Serial.println("Station disconnected");
      });

      Serial.printf("Connecting to %s ...\n", ssid);
      WiFi.begin(ssid, password);
    }


    void loop()
    {
      digitalWrite(LED_BUILTIN, ledState);
      ledState = !ledState;
      delay(250);
    }

Check the Code
~~~~~~~~~~~~~~

After uploading above sketch and opening a serial monitor we should see a similar log:

::

    Connecting to sensor-net ...
    Station connected, IP: 192.168.1.10

If you switch off the access point, and put it back on, you will see the following:

::

    Station disconnected
    Station disconnected
    Station disconnected
    Station connected, IP: 192.168.1.10

The process of connection, disconnection and printing messages is done in background of the ``loop()`` that is responsible for blinking the LED. Therefore the blink pattern all the time remains undisturbed.

Conclusion
~~~~~~~~~~

Check out events from generic class. They will help you to write more compact code. Use them to practice splitting your code into separate tasks that are executed asynchronously.

For review of functions included in generic class, please refer to the `Generic Class <generic-class.rst>`__ documentation.
