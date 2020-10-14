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

#define CORE_MOCK 1
#define MOCK "(mock) " // TODO: provide common logging API instead of adding this string everywhere?

//

#define ARDUINO 267
#define ESP8266 1
#define A0 0
#define LED_BUILTIN 0
#define LWIP_OPEN_SRC
#define TCP_MSS 536
#define LWIP_FEATURES 1

//

#define D0 0
#define D1 1
#define D2 3
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define D8 8

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
// TODO: #include <stdlib_noniso.h> ?
char* itoa (int val, char *s, int radix);
char* ltoa (long val, char *s, int radix);


size_t strlcat(char *dst, const char *src, size_t size);
size_t strlcpy(char *dst, const char *src, size_t size);

#ifdef __cplusplus
}
#endif

// exotic typedefs used in the sdk

#include <stdint.h>
typedef uint8_t uint8;
typedef uint32_t uint32;

//

#include <c_types.h>
#include <core_esp8266_features.h>

uint32_t esp_get_cycle_count();

#include <Arduino.h>

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
#define ets_vsnprintf vsnprintf

int mockverbose (const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));

extern const char* host_interface; // cmdline parameter
extern bool serial_timestamp;
extern int mock_port_shifter;
extern bool blocking_uart;
extern uint32_t global_source_address; // 0 = INADDR_ANY by default

#define NO_GLOBAL_BINDING 0xffffffff
extern uint32_t global_ipv4_netfmt; // selected interface addresse to bind to

#ifdef __cplusplus
}
#endif

//

#ifdef __cplusplus

#ifndef CCBUFSIZE
#define CCBUFSIZE 65536
#endif

// uart
#ifdef __cplusplus
extern "C" {
#endif
void uart_new_data(const int uart_nr, uint8_t data);
#ifdef __cplusplus
}
#endif

// tcp
int    mockSockSetup  (int sock);
int    mockConnect    (uint32_t addr, int& sock, int port);
ssize_t mockFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize);
ssize_t mockPeekBytes (int sock, char* dst, size_t size, int timeout_ms, char* buf, size_t& bufsize);
ssize_t mockRead      (int sock, char* dst, size_t size, int timeout_ms, char* buf, size_t& bufsize);
ssize_t mockWrite     (int sock, const uint8_t* data, size_t size, int timeout_ms);
int serverAccept (int sock);

// udp
void check_incoming_udp ();
int mockUDPSocket ();
bool mockUDPListen (int sock, uint32_t dstaddr, uint16_t port, uint32_t mcast = 0);
size_t mockUDPFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize, uint8_t& addrsize, uint8_t addr[16], uint16_t& port);
size_t mockUDPPeekBytes (int sock, char* dst, size_t usersize, int timeout_ms, char* ccinbuf, size_t& ccinbufsize);
size_t mockUDPRead (int sock, char* dst, size_t size, int timeout_ms, char* ccinbuf, size_t& ccinbufsize);
size_t mockUDPWrite (int sock, const uint8_t* data, size_t size, int timeout_ms, uint32_t ipv4, uint16_t port);
void mockUDPSwallow (size_t copied, char* ccinbuf, size_t& ccinbufsize);

class UdpContext;
void register_udp (int sock, UdpContext* udp = nullptr);

//

void mock_start_spiffs (const String& fname, size_t size_kb, size_t block_kb = 8, size_t page_b = 512);
void mock_stop_spiffs ();
void mock_start_littlefs (const String& fname, size_t size_kb, size_t block_kb = 8, size_t page_b = 512);
void mock_stop_littlefs ();

//

#include <common/esp8266_peri.h>

//

#endif // __cplusplus
