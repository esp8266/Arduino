/*
 Arduino emulation - common to all emulated code
 Copyright (c) 2018 david gauchard. All rights reserved.

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal with the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 - Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimers.

 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimers in the
   documentation and/or other materials provided with the distribution.

 - The names of its contributors may not be used to endorse or promote
   products derived from this Software without specific prior written
   permission.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS WITH THE SOFTWARE.
*/

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

#include <stdlib.h>
#define RANDOM_REG32 ((uint32_t)random())

// net tweak

// htontoh code in common/MockTools.cpp
#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS
#undef INADDR_NONE

//

#ifdef __cplusplus
extern "C" {
#endif
int ets_printf (const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));
#define os_printf_plus printf

extern const char* host_interface; // cmdline parameter

#define NO_GLOBAL_BINDING 0xffffffff
extern uint32_t global_ipv4_netfmt; // selected interface addresse to bind to

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

#define CORE_MOCK 1

#define ARDUINO 267
#define ESP8266 1
#define A0 0
#define LED_BUILTIN 0
#define F_CPU 80000000
#define LWIP_OPEN_SRC
#define TCP_MSS 536
#define LWIP_FEATURES 1

//

#endif // __cplusplus
