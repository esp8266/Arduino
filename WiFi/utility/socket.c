/*
*
@file		socket.c
@brief	define function of socket API 
*
*/
#include <inttypes.h>
#include "socket.h"

SOCKET socket(uint8 protocol) {} // Opens a socket(TCP or UDP or IP_RAW mode)
void close(SOCKET s) {} // Close socket
uint8 connect(SOCKET s, uint8 * addr, uint16 port) {} // Establish TCP connection (Active connection)
void disconnect(SOCKET s) {} // disconnect the connection
uint8 listen(SOCKET s) {}	// Establish TCP connection (Passive connection)
uint16 send(SOCKET s, const uint8 * buf, uint16 len) {} // Send data (TCP)
uint16 recv(SOCKET s, uint8 * buf, uint16 len) {}	// Receive data (TCP)
uint16 sendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port) {} // Send data (UDP/IP RAW)
uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16  *port) {} // Receive data (UDP/IP RAW)

uint16 igmpsend(SOCKET s, const uint8 * buf, uint16 len) {}
