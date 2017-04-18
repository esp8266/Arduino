/*
  WiFiUdp.h - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified by Ivan Grokhotkov, January 2015 - esp8266 support
*/

#ifndef WIFIUDP_H
#define WIFIUDP_H

#include <Udp.h>
#include <include/slist.h>

#define UDP_TX_PACKET_MAX_SIZE 8192

class UdpContext;

class WiFiUDP : public UDP, public SList<WiFiUDP> {
private:
  UdpContext* _ctx;

public:
  WiFiUDP();  // Constructor
  WiFiUDP(const WiFiUDP& other);
  WiFiUDP& operator=(const WiFiUDP& rhs);
  ~WiFiUDP();

  operator bool() const { return _ctx != 0; }

  // initialize, start listening on specified port. 
  // Returns 1 if successful, 0 if there are no sockets available to use
  virtual uint8_t begin(uint16_t port);	
  // Finish with the UDP connetion
  virtual void stop();
  // join a multicast group and listen on the given port
  uint8_t beginMulticast(IPAddress interfaceAddr, IPAddress multicast, uint16_t port); 

  // Sending UDP packets
  
  // Start building up a packet to send to the remote host specific in ip and port
  // Returns 1 if successful, 0 if there was a problem with the supplied IP address or port
  virtual int beginPacket(IPAddress ip, uint16_t port);
  // Start building up a packet to send to the remote host specific in host and port
  // Returns 1 if successful, 0 if there was a problem resolving the hostname or port
  virtual int beginPacket(const char *host, uint16_t port);
  // Start building up a packet to send to the multicast address
  // multicastAddress - muticast address to send to
  // interfaceAddress - the local IP address of the interface that should be used
  //                    use WiFi.localIP() or WiFi.softAPIP() depending on the interface you need
  // ttl              - multicast packet TTL (default is 1)
  // Returns 1 if successful, 0 if there was a problem with the supplied IP address or port
  virtual int beginPacketMulticast(IPAddress multicastAddress, 
                                   uint16_t port, 
                                   IPAddress interfaceAddress, 
                                   int ttl = 1);
  // Finish off this packet and send it
  // Returns 1 if the packet was sent successfully, 0 if there was an error
  virtual int endPacket();
  // Write a single byte into the packet
  virtual size_t write(uint8_t);
  // Write size bytes from buffer into the packet
  virtual size_t write(const uint8_t *buffer, size_t size);
  
  using Print::write;

  // Start processing the next available incoming packet
  // Returns the size of the packet in bytes, or 0 if no packets are available
  virtual int parsePacket();
  // Number of bytes remaining in the current packet
  virtual int available();
  // Read a single byte from the current packet
  virtual int read();
  // Read up to len bytes from the current packet and place them into buffer
  // Returns the number of bytes read, or 0 if none are available
  virtual int read(unsigned char* buffer, size_t len);
  // Read up to len characters from the current packet and place them into buffer
  // Returns the number of characters read, or 0 if none are available
  virtual int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };
  // Return the next byte from the current packet without moving on to the next byte
  virtual int peek();
  virtual void flush();	// Finish reading the current packet

  // Return the IP address of the host who sent the current incoming packet
  virtual IPAddress remoteIP();
  // Return the port of the host who sent the current incoming packet
  virtual uint16_t remotePort();
  // Return the destination address for incoming packets,
  // useful to distinguish multicast and ordinary packets
  IPAddress destinationIP();
  // Return the local port for outgoing packets
  uint16_t localPort();

  static void stopAll();
  static void stopAllExcept(WiFiUDP * exC);

};

#endif //WIFIUDP_H

