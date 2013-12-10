/*
*
@file		socket.c
@brief	define function of socket API 
*
*/
#include <inttypes.h>
#include "utility/socket.h"

SOCKET socket(uint8_t protocol) {return 0;} // Opens a socket(TCP or UDP or IP_RAW mode)
void close(SOCKET s) {} // Close socket
uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port) {return 0;} // Establish TCP connection (Active connection)
void disconnect(SOCKET s) {} // disconnect the connection
uint8_t listen(SOCKET s) { return 0;}	// Establish TCP connection (Passive connection)
uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len) { return 0;} // Send data (TCP)
uint16_t recv(SOCKET s, uint8_t * buf, uint16_t len) {return 0;}	// Receive data (TCP)
uint16_t sendto(SOCKET s, const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port) {return 0;} // Send data (UDP/IP RAW)
uint16_t recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t  *port) {return 0;} // Receive data (UDP/IP RAW)

uint16_t igmpsend(SOCKET s, const uint8_t * buf, uint16_t len) {return 0;}
