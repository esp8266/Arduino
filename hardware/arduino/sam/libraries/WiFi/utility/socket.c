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
