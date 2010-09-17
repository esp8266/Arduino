/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.c/socket.h
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/ 
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#include "w5100.h"
#include "socket.h"
#include "Ethernet.h"
#include "Udp.h"

/* Start UDP socket, listening at local port PORT */
void UdpClass::begin(uint16_t port) {
  _port = port;
  _sock = 0; //TODO: should not be hardcoded
  socket(_sock, SnMR::UDP, _port, 0);
}

/* Send packet contained in buf of length len to peer at specified ip, and port */
/* Use this function to transmit binary data that might contain 0x00 bytes*/
/* This function returns sent data size for success else -1. */
uint16_t UdpClass::sendPacket(uint8_t * buf, uint16_t len,  uint8_t * ip, uint16_t port){
  return sendto(_sock,(const uint8_t *)buf,len,ip,port);
}

/* Send  zero-terminated string str as packet to peer at specified ip, and port */
/* This function returns sent data size for success else -1. */
uint16_t UdpClass::sendPacket(const char str[], uint8_t * ip, uint16_t port){	
  // compute strlen
  const char *s;
  for(s = str; *s; ++s);
  uint16_t len = (s-str);
  // send packet
  return sendto(_sock,(const uint8_t *)str,len,ip,port);
}
/* Is data available in rx buffer? Returns 0 if no, number of available bytes if yes. 
 * returned value includes 8 byte UDP header!*/
int UdpClass::available() {
  return W5100.getRXReceivedSize(_sock);
}


/* Read a received packet into buffer buf (which is of maximum length len); */
/* store calling ip and port as well. Call available() to make sure data is ready first. */
/* NOTE: I don't believe len is ever checked in implementation of recvfrom(),*/
/*       so it's easy to overflow buffer. so we check and truncate. */
/* returns number of bytes read, or negative number of bytes we would have needed if we truncated */
int UdpClass::readPacket(uint8_t * buf, uint16_t bufLen, uint8_t *ip, uint16_t *port) {
  int packetLen = available()-8; //skip UDP header;
  if(packetLen < 0 ) return 0; // no real data here	
  if(packetLen > (int)bufLen) {
    //packet is too large - truncate
    //HACK - hand-parse the UDP packet using TCP recv method
    uint8_t tmpBuf[8];
    int i;
    //read 8 header bytes and get IP and port from it
    recv(_sock,tmpBuf,8);
    ip[0] = tmpBuf[0];
    ip[1] = tmpBuf[1];
    ip[2] = tmpBuf[2];
    ip[3] = tmpBuf[3];
    *port = tmpBuf[4];
    *port = (*port << 8) + tmpBuf[5];

    //now copy first (bufLen) bytes into buf		
    for(i=0;i<(int)bufLen;i++) {
      recv(_sock,tmpBuf,1);
      buf[i]=tmpBuf[0];
    }

    //and just read the rest byte by byte and throw it away
    while(available()) {
      recv(_sock,tmpBuf,1);
    }

    return (-1*packetLen);

    //ALTERNATIVE: requires stdlib - takes a bunch of space
    /*//create new buffer and read everything into it
     		uint8_t * tmpBuf = (uint8_t *)malloc(packetLen);
     		recvfrom(_sock,tmpBuf,packetLen,ip,port);
     		if(!tmpBuf) return 0; //couldn't allocate
     		// copy first bufLen bytes
     		for(unsigned int i=0; i<bufLen; i++) {
     			buf[i]=tmpBuf[i];
     		}
     		//free temp buffer
     		free(tmpBuf);
     		*/


  } 
  return recvfrom(_sock,buf,bufLen,ip,port);
}

/* Read a received packet, throw away peer's ip and port.  See note above. */
int UdpClass::readPacket(uint8_t * buf, uint16_t len) {
  uint8_t ip[4];
  uint16_t port[1];
  return recvfrom(_sock,buf,len,ip,port);
}

int UdpClass::readPacket(char * buf, uint16_t bufLen, uint8_t *ip, uint16_t &port) {
uint16_t myPort;
uint16_t ret = readPacket( (byte*)buf, bufLen, ip, &myPort);
port = myPort;
return ret;
}




/* Create one global object */
UdpClass Udp;
