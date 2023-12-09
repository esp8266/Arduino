:orphan:

UDP Class
---------

Methods documented for `WiFiUDP Class <https://www.arduino.cc/en/Reference/WiFiUDPConstructor>`__ in `Arduino <https://github.com/arduino/Arduino>`__

1.  `begin() <https://www.arduino.cc/en/Reference/WiFiUDPBegin>`__
2.  `available() <https://www.arduino.cc/en/Reference/WiFiUDPAvailable>`__
3.  `beginPacket() <https://www.arduino.cc/en/Reference/WiFiUDPBeginPacket>`__
4.  `endPacket() <https://www.arduino.cc/en/Reference/WiFiUDPEndPacket>`__
5.  `write() <https://www.arduino.cc/en/Reference/WiFiUDPWrite>`__
6.  `parsePacket() <https://www.arduino.cc/en/Reference/WiFiUDPParsePacket>`__
7.  `peek() <https://www.arduino.cc/en/Reference/WiFiUDPPeek>`__
8.  `read() <https://www.arduino.cc/en/Reference/WiFiUDPRead>`__
9.  `flush() <https://www.arduino.cc/en/Reference/WiFiUDPFlush>`__
10. `stop() <https://www.arduino.cc/en/Reference/WiFIUDPStop>`__
11. `remoteIP() <https://www.arduino.cc/en/Reference/WiFiUDPRemoteIP>`__
12. `remotePort() <https://www.arduino.cc/en/Reference/WiFiUDPRemotePort>`__

Methods and properties described further down are specific to ESP8266.
They are not covered in `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ documentation. Before they are fully documented please refer to information below.

Multicast UDP
~~~~~~~~~~~~~

.. code:: cpp

    uint8_t  beginMulticast (IPAddress multicast, uint16_t port) 
    virtual int  beginPacketMulticast (IPAddress multicastAddress, uint16_t port, IPAddress interfaceAddress, int ttl=1) 
    IPAddress  destinationIP () 
    uint16_t  localPort ()

The ``WiFiUDP`` class supports sending and receiving multicast packets on STA interface. When sending a multicast packet, replace ``udp.beginPacket(addr, port)`` with ``udp.beginPacketMulticast(addr, port, WiFi.localIP())``. When listening to multicast packets, replace ``udp.begin(port)`` with ``udp.beginMulticast(multicast_ip_addr, port)``. You can use ``udp.destinationIP()`` to tell whether the packet received was sent to the multicast or unicast address.

For code samples please refer to separate section with `examples <udp-examples.rst>`__ dedicated specifically to the UDP Class.
