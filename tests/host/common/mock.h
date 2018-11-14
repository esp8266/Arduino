
// include host's STL before any other include file
// because core definition like max() is in the way

#ifdef __cplusplus
#include <vector>
#endif

// exotic typedefs used in the sdk

#include <stdint.h>
typedef uint8_t uint8;
typedef uint32_t uint32;

//

#include <Arduino.h>

//

// htontoh code in common/MockTools.cpp
#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS

//

#include <stdlib.h>
#define RANDOM_REG32 ((uint32_t)random())

//

#undef INADDR_NONE

//

#ifdef __cplusplus
extern "C" {
#endif
int ets_printf (const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));
#define os_printf_plus ets_printf

extern const char* host_interface; // cmdline parameter

#ifdef __cplusplus
}
#endif

//

#ifdef __cplusplus

#ifndef CCBUFSIZE
#define CCBUFSIZE 8192
#endif

// tcp
int    mockConnect   (uint32_t addr, int& sock, int port);
size_t mockFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize);
size_t mockPeekBytes (int sock, char* dst, size_t size, int timeout_ms, char* buf, size_t& bufsize);
size_t mockRead      (int sock, char* dst, size_t size, int timeout_ms, char* buf, size_t& bufsize);
size_t mockWrite     (int sock, const uint8_t* data, size_t size, int timeout_ms);
int serverAccept (int sock);

// udp
#define ADDRBUFSIZE	32	// ipv6:16 ipv4:4
int mockUDPSocket ();
bool mockUDPListen (int sock, uint32_t dstaddr, uint16_t port, uint32_t mcast = 0);
size_t mockUDPFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize, uint8_t& addrsize, uint8_t addr[16], uint16_t& port);
size_t mockUDPPeekBytes (int sock, char* dst, size_t usersize, int timeout_ms, char* ccinbuf, size_t& ccinbufsize);
size_t mockUDPRead (int sock, char* dst, size_t size, int timeout_ms, char* ccinbuf, size_t& ccinbufsize);
size_t mockUDPWrite (int sock, const uint8_t* data, size_t size, int timeout_ms, uint32_t ipv4, uint16_t port);

class UdpContext;
void register_udp (int sock, UdpContext* udp = nullptr);

class InterruptLock { };

//

#define ARDUINO 1
#define ESP8266 1
#define A0 0
#define LED_BUILTIN 0

//

#endif // __cplusplus
