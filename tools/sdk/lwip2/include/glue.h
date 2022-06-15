
/*

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 
3. The name of the author may not be used to endorse or promote products 
derived from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.

author: d. gauchard

*/

#ifndef GLUE_H
#define GLUE_H

#ifndef ARDUINO
#define ARDUINO 0
#endif

#ifndef OPENSDK
#define OPENSDK 0
#endif

#if !ARDUINO && !OPENSDK
#error Must defined ARDUINO or OPENSDK
#endif

#include "gluedebug.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LWIP14GLUE
#define __IPV4_ADDR_H__     // prevents inclusion of ipv4_addr.h meant for lwip2
#define ipv4_addr ip_addr   // structures are identical, ipv4_addr is unknown with lwIP-1.4
#include <lwip/ip_addr.h>   // formerly official struct ip_info, disappeared in lwIP-v2
#endif

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#ifdef __cplusplus
}
#endif

typedef enum
{
	GLUE_ERR_OK         = 0,
	GLUE_ERR_MEM        = -1,
	GLUE_ERR_BUF        = -2,
	GLUE_ERR_TIMEOUT    = -3,
	GLUE_ERR_RTE        = -4,
	GLUE_ERR_INPROGRESS = -5,
	GLUE_ERR_VAL        = -6,
	GLUE_ERR_WOULDBLOCK = -7,
	GLUE_ERR_USE        = -8,
	GLUE_ERR_ALREADY    = -9,
	GLUE_ERR_ISCONN     = -10,
	GLUE_ERR_CONN       = -11,
	GLUE_ERR_IF         = -12,
	GLUE_ERR_ABRT       = -13,
	GLUE_ERR_RST        = -14,
	GLUE_ERR_CLSD       = -15,
	GLUE_ERR_ARG        = -16
} err_glue_t;

typedef enum
{
	GLUE_NETIF_FLAG_BROADCAST	= 1,
	GLUE_NETIF_FLAG_UP		= 2,
	GLUE_NETIF_FLAG_ETHARP		= 4,
	GLUE_NETIF_FLAG_IGMP		= 8,
	GLUE_NETIF_FLAG_LINK_UP		= 16,
} glue_netif_flags_t;

void		esp2glue_lwip_init		(void);
void		esp2glue_espconn_init		(void);
void		esp2glue_dhcps_start		(struct ip_info* info);
err_glue_t	esp2glue_dhcp_start		(int netif_idx);
void		esp2glue_dhcp_stop		(int netif_idx);
void		esp2glue_netif_updated		(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw, glue_netif_flags_t flags, size_t hwlen, const uint8_t* hw /*, void* state*/);
err_glue_t	esp2glue_ethernet_input		(int netif_idx, void* glue_pbuf);
void		esp2glue_alloc_for_recv		(size_t len, void** glue_pbuf, void** glue_data);
void		esp2glue_pbuf_freed		(void* ref_saved);
void		esp2glue_netif_set_default	(int netif_idx);
void		esp2glue_netif_update		(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw, size_t hwlen, const uint8_t* hwaddr, uint16_t mtu);
void		esp2glue_netif_set_up1down0	(int netif_idx, int up1_or_down0);

void		glue2esp_ifupdown		(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw);
err_glue_t	glue2esp_linkoutput		(int netif_idx, void* ref2save, void* data, size_t size);

// fixed definitions from esp8266/arduino
// renamed with lwip_ to avoid name collision
// reference and credits: https://github.com/esp8266/Arduino/pull/6301
#ifndef __STRINGIFY
#define __STRINGIFY(a) #a
#endif
#define lwip_xt_rsil(level) (__extension__({uint32_t state; __asm__ __volatile__("rsil %0," __STRINGIFY(level) : "=a" (state) :: "memory"); state;}))
#define lwip_xt_wsr_ps(state)  __asm__ __volatile__("wsr %0,ps; isync" :: "a" (state) : "memory")

// quickfix: workaround for definition of __PRI32(x) in inttypes.h
// it has changed with recent version of xtensa-gcc
// __INT32 is missing
// gcc-4.x:    __PRI32(x) is __STRINGIFY(l##x)
// gcc-10.2.0: __PRI32(x) is __INT32 __STRINGIFY(x)
#include <inttypes.h>
#if !defined(__INT8)
#define __INT8
#endif
#if !defined(__INT16)
#define __INT16
#endif
#if !defined(__INT32)
#define __INT32 "l"
#endif

#endif // GLUE_H
