:orphan:

Client Class
------------

Methods documented for `Client <https://www.arduino.cc/en/Reference/WiFiClientConstructor>`__ in `Arduino <https://github.com/arduino/Arduino>`__

1.  `WiFiClient() <https://www.arduino.cc/en/Reference/WiFiClient>`__
2.  `connected() <https://www.arduino.cc/en/Reference/WiFiClientConnected>`__
3.  `connect() <https://www.arduino.cc/en/Reference/WiFiClientConnect>`__
4.  `write() <https://www.arduino.cc/en/Reference/WiFiClientWrite>`__
5.  `print() <https://www.arduino.cc/en/Reference/WiFiClientPrint>`__
6.  `println() <https://www.arduino.cc/en/Reference/WiFiClientPrintln>`__
7.  `available() <https://www.arduino.cc/en/Reference/WiFiClientAvailable>`__
8.  `read() <https://www.arduino.cc/en/Reference/WiFiClientRead>`__
9.  `flush() <https://www.arduino.cc/en/Reference/WiFiClientFlush>`__
10. `stop() <https://www.arduino.cc/en/Reference/WiFIClientStop>`__

Methods and properties described further down are specific to ESP8266. They are not covered in `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ documentation. Before they are fully documented please refer to information below.

flush and stop
~~~~~~~~~~~~~~

``flush(timeoutMs)`` and ``stop(timeoutMs)`` both have now an optional argument: ``timeout`` in millisecond, and both return a boolean.

Default input value 0 means that effective value is left at the discretion of the implementer.

``flush()`` returning ``true`` indicates that output data have effectively been sent, and ``false`` that a timeout has occurred.

``stop()`` returns ``false`` in case of an issue when closing the client (for instance a timed-out ``flush``). Depending on implementation, its parameter can be passed to ``flush()``.

abort
~~~~~

.. code:: cpp

   void abort();


Originally proposed in `#8738 <https://github.com/esp8266/Arduino/pull/8738>`__
Unlike ``stop()``, immediately shuts down internal connection object.

Under usual circumstances, we either enter ``CLOSE_WAIT`` or ``TIME_WAIT`` state. But, the connection object is not freed right away, and requires us to either
* wait until ``malloc()`` returns ``NULL`` when our TCP stack tries to allocate memory for a new connection
* manually call ``tcp_kill_timewait()`` to forcibly stop the 'oldest' connection

This API frees up resources used by the connection. Consider using it instead of ``stop()`` if your application handles a lot of clients and frequently runs out of available heap memory.

*Example:*

.. code:: cpp

   # define MIN_HEAP_FREE 20000 // or whatever min available heap memory convienent for your application 
   auto client = server.accept();
   // ... do something with the client object ...
   if (ESP.getFreeHeap() >= MIN_HEAP_FREE) {
     client.stop();
   } else {
     client.abort();
   }

setNoDelay
~~~~~~~~~~

.. code:: cpp

    setNoDelay(nodelay)

With ``nodelay`` set to ``true``, this function will to disable `Nagle algorithm <https://en.wikipedia.org/wiki/Nagle%27s_algorithm>`__.

This algorithm is intended to reduce TCP/IP traffic of small packets sent over the network by combining a number of small outgoing messages, and sending them all at once. The downside of such approach is effectively delaying individual messages until a big enough packet is assembled.

*Example:*

.. code:: cpp

    client.setNoDelay(true);

getNoDelay
~~~~~~~~~~

Returns whether NoDelay is enabled or not for the current connection.

setSync
~~~~~~~

This is an experimental API that will set the client in synchronized mode.
In this mode, every ``write()`` is flushed.  It means that after a call to
``write()``, data are ensured to be received where they went sent to (that is
``flush`` semantic).

When set to ``true`` in ``WiFiClient`` implementation,

- It slows down transfers, and implicitly disable the Nagle algorithm.

- It also allows to avoid a temporary copy of data that otherwise consumes
  at most ``TCP_SND_BUF`` = (2 * ``MSS``) bytes per connection,

getSync
~~~~~~~

Returns whether Sync is enabled or not for the current connection.

setDefaultNoDelay and setDefaultSync
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These set the default value for both ``setSync`` and ``setNoDelay`` for
every future instance of ``WiFiClient`` (including those coming from
``WiFiServer.available()`` by default).

Default values are false for both ``NoDelay`` and ``Sync``.

This means that Nagle is enabled by default *for all new connections*.

getDefaultNoDelay and getDefaultSync
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Return the values to be used as default for NoDelay and Sync for all future connections.

Other Function Calls
~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    uint8_t  status () 
    virtual size_t  write (const uint8_t *buf, size_t size) 
    size_t  write_P (PGM_P buf, size_t size) 
    size_t  write (Stream &stream) 
    size_t  write (Stream &stream, size_t unitSize) __attribute__((deprecated)) 
    virtual int  read (uint8_t *buf, size_t size) 
    virtual int  peek () 
    virtual size_t  peekBytes (uint8_t *buffer, size_t length) 
    size_t  peekBytes (char *buffer, size_t length) 
    virtual  operator bool () 
    IPAddress  remoteIP () 
    uint16_t  remotePort () 
    IPAddress  localIP () 
    uint16_t  localPort () 

Documentation for the above functions is not yet available.
