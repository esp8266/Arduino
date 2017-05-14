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

setNoDelay
~~~~~~~~~~

.. code:: cpp

    setNoDelay(nodelay)

With ``nodelay`` set to ``true``, this function will to disable `Nagle algorithm <https://en.wikipedia.org/wiki/Nagle%27s_algorithm>`__.

This algorithm is intended to reduce TCP/IP traffic of small packets sent over the network by combining a number of small outgoing messages, and sending them all at once. The downside of such approach is effectively delaying individual messages until a big enough packet is assembled.

*Example:*

.. code:: cpp

    client.setNoDelay(true);

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
    bool  getNoDelay () 

Documentation for the above functions is not yet prepared.

For code samples please refer to separate section with `examples
:arrow\_right: <client-examples.md>`__ dedicated specifically to the Client Class.
