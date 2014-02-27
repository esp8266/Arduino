/*
  socket.c - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

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
*/

/*
*
@file		socket.c
@brief	define function of socket API 
*
*/
#include <inttypes.h>
#include "socket.h"

SOCKET socket(uint8 protocol) {return 0;} // Opens a socket(TCP or UDP or IP_RAW mode)
void close(SOCKET s) {} // Close socket
uint8 connect(SOCKET s, uint8 * addr, uint16 port) {return 0;} // Establish TCP connection (Active connection)
void disconnect(SOCKET s) {} // disconnect the connection
uint8 listen(SOCKET s) { return 0;}	// Establish TCP connection (Passive connection)
uint16 send(SOCKET s, const uint8 * buf, uint16 len) { return 0;} // Send data (TCP)
uint16 recv(SOCKET s, uint8 * buf, uint16 len) {return 0;}	// Receive data (TCP)
uint16 sendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port) {return 0;} // Send data (UDP/IP RAW)
uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16  *port) {return 0;} // Receive data (UDP/IP RAW)

uint16 igmpsend(SOCKET s, const uint8 * buf, uint16 len) {return 0;}
