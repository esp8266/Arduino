:orphan:

Server Class
------------

Methods documented for the `Server Class <https://www.arduino.cc/en/Reference/WiFiServerConstructor>`__ in `Arduino <https://github.com/arduino/Arduino>`__

1. `WiFiServer() <https://www.arduino.cc/en/Reference/WiFiServer>`__
2. `begin() <https://www.arduino.cc/en/Reference/WiFiServerBegin>`__
3. `available() <https://www.arduino.cc/en/Reference/WiFiServerAvailable>`__
4. `write() <https://www.arduino.cc/en/Reference/WiFiServerWrite>`__
5. `print() <https://www.arduino.cc/en/Reference/WiFiServerPrint>`__
6. `println() <https://www.arduino.cc/en/Reference/WiFiServerPrintln>`__

In ESP8266WiFi library the ``ArduinoWiFiServer`` class implements ``available`` and the write-to-all-clients functionality as described in the Arduino WiFi library reference. The PageServer example shows how ``available`` and the write-to-all-clients works. 

For most use cases the basic WiFiServer class of the ESP8266WiFi library is suitable.

Methods and properties described further down are specific to ESP8266. They are not covered in `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ documentation. Before they are fully documented please refer to information below.

accept
~~~~~~

Method ``accept()`` returns a waiting client connection. `accept() is documented <https://www.arduino.cc/en/Reference/EthernetServerAccept>`__ for the Arduino Ethernet library.

available
~~~~~~~~~
.. deprecated:: 3.1.0
  see ``accept``

``available`` in the ESP8266WiFi library's WiFiServer class doesn't work as documented for the Arduino WiFi library. It works the same way as ``accept``.

write (write to all clients) not supported
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Please note that the ``write`` method on the ``WiFiServer`` object is not implemented and returns failure always.  Use the returned
``WiFiClient`` object from the ``WiFiServer::accept()`` method to communicate with individual clients.  If you need to send
the exact same packets to a series of clients, your application must maintain a list of connected clients and iterate over them manually.

setNoDelay
~~~~~~~~~~

.. code:: cpp

    setNoDelay(nodelay)

With ``nodelay`` set to ``true``, this function will to disable `Nagle algorithm <https://en.wikipedia.org/wiki/Nagle%27s_algorithm>`__.

This algorithm is intended to reduce TCP/IP traffic of small packets sent over the network by combining a number of small outgoing messages, and sending them all at once. The downside of such approach is effectively delaying individual messages until a big enough packet is assembled.

*Example:*

.. code:: cpp

    server.begin();
    server.setNoDelay(true);

By default, ``nodelay`` value will depends on global ``WiFiClient::getDefaultNoDelay()`` (currently false by default).

However, a call to ``wiFiServer.setNoDelay()`` will override ``NoDelay`` for all new ``WiFiClient`` provided by the calling instance (``wiFiServer``).

Other Function Calls
~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    bool  hasClient () 
    size_t  hasClientData () 
    bool  hasMaxPendingClients ()
    bool  getNoDelay () 
    virtual size_t  write (const uint8_t *buf, size_t size) 
    uint8_t  status () 
    void  close () 
    void  stop ()

Documentation for the above functions is not yet prepared.

For code samples please refer to separate section with `examples <server-examples.rst>`__ dedicated specifically to the Server Class.
