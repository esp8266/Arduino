/*
  socket.h - Library for Arduino Wifi shield.
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
@file		socket.h
@brief	define function of socket API 
*
*/

#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#define TCP_SOCKET  1
#define UDP_SOCKET  2
#define RAW_SOCKET  3

#define SOCK_NOT_AVAIL  255

#include "wl_definitions.h"
/**
 * The 8-bit signed data type.
 */
typedef char int8;
/**
 * The volatile 8-bit signed data type.
 */
typedef volatile char vint8;
/**
 * The 8-bit unsigned data type.
 */
typedef unsigned char uint8;
/**
 * The volatile 8-bit unsigned data type.
 */
typedef volatile unsigned char vuint8;

/**
 * The 16-bit signed data type.
 */
typedef int int16;
/**
 * The volatile 16-bit signed data type.
 */
typedef volatile int vint16;
/**
 * The 16-bit unsigned data type.
 */
typedef unsigned int uint16;
/**
 * The volatile 16-bit unsigned data type.
 */
typedef volatile unsigned int vuint16;
/**
 * The 32-bit signed data type.
 */
typedef long int32;
/**
 * The volatile 32-bit signed data type.
 */
typedef volatile long vint32;
/**
 * The 32-bit unsigned data type.
 */
typedef unsigned long uint32;
/**
 * The volatile 32-bit unsigned data type.
 */
typedef volatile unsigned long vuint32;

/* bsd */
typedef uint8			u_char;		/**< 8-bit value */
typedef uint16_t 			SOCKET;
typedef uint16			u_short;	/**< 16-bit value */
typedef uint16			u_int;		/**< 16-bit value */
typedef uint32			u_long;		/**< 32-bit value */

extern SOCKET socket(uint8 protocol); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(SOCKET s); // Close socket
extern uint8 connect(SOCKET s, uint8 * addr, uint16 port); // Establish TCP connection (Active connection)
extern void disconnect(SOCKET s); // disconnect the connection
extern uint8 listen(SOCKET s);	// Establish TCP connection (Passive connection)
extern uint16 send(SOCKET s, const uint8 * buf, uint16 len); // Send data (TCP)
extern uint16 recv(SOCKET s, uint8 * buf, uint16 len);	// Receive data (TCP)
extern uint16 sendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port); // Send data (UDP/IP RAW)
extern uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16  *port); // Receive data (UDP/IP RAW)

extern uint16 igmpsend(SOCKET s, const uint8 * buf, uint16 len);
#endif
/* _SOCKET_H_ */
