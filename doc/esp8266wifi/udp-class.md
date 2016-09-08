---
title: ESP8266WiFi UDP Class
---

[ESP8266WiFi Library :back:](readme.md#udp)


## UDP Class

Methods documented for [WiFiUDP Class](https://www.arduino.cc/en/Reference/WiFiUDPConstructor) in [Arduino](https://github.com/arduino/Arduino)

1.  [begin()](https://www.arduino.cc/en/Reference/WiFiUDPBegin)
2.  [available()](https://www.arduino.cc/en/Reference/WiFiUDPAvailable)
3.  [beginPacket()](https://www.arduino.cc/en/Reference/WiFiUDPBeginPacket)
4.  [endPacket()](https://www.arduino.cc/en/Reference/WiFiUDPEndPacket)
5.  [write()](https://www.arduino.cc/en/Reference/WiFiUDPWrite)
6.  [parsePacket()](https://www.arduino.cc/en/Reference/WiFiUDPParsePacket)
7.  [peek()](https://www.arduino.cc/en/Reference/WiFiUDPPeek)
8.  [read()](https://www.arduino.cc/en/Reference/WiFiUDPRead)
9.  [flush()](https://www.arduino.cc/en/Reference/WiFiUDPFlush)
10.  [stop()](https://www.arduino.cc/en/Reference/WiFIUDPStop)
11.  [remoteIP()](https://www.arduino.cc/en/Reference/WiFiUDPRemoteIP)
12.  [remotePort()](https://www.arduino.cc/en/Reference/WiFiUDPRemotePort)


Methods and properties described further down are specific to ESP8266. They are not covered in [Arduino WiFi library](https://www.arduino.cc/en/Reference/WiFi) documentation. Before they are fully documented please refer to information below.


### Multicast UDP

```cpp
uint8_t  beginMulticast (IPAddress interfaceAddr, IPAddress multicast, uint16_t port) 
virtual int  beginPacketMulticast (IPAddress multicastAddress, uint16_t port, IPAddress interfaceAddress, int ttl=1) 
IPAddress  destinationIP () 
uint16_t  localPort ()
```

The `WiFiUDP` class supports sending and receiving multicast packets on STA interface. When sending a multicast packet, replace `udp.beginPacket(addr, port)` with `udp.beginPacketMulticast(addr, port, WiFi.localIP())`. When listening to multicast packets, replace `udp.begin(port)` with `udp.beginMulticast(WiFi.localIP(), multicast_ip_addr, port)`. You can use `udp.destinationIP()` to tell whether the packet received was sent to the multicast or unicast address.


For code samples please refer to separate section with [examples :arrow_right:](udp-examples.md) dedicated specifically to the UDP Class.