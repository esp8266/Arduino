
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


#ifndef GLUE_STUB_H
#define GLUE_STUB_H

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "esp-missing.h"

#define UDEBUG	0	// 0 or 1
#define UDUMP	0	// 0 or 1 (show packets content)

#include "uprint.h"
#include "doprint.h"

// 0: use os_printf
// 1: buffered and line number, needs doprint_allow=1 after Serial.begin
#if UDEBUG
#undef	os_printf
#define	os_printf	uprint
#define UPRINTF 	doprint
#else
#define UPRINTF 	os_printf
#ifdef USE_OPTIMIZE_PRINTF // bug in osapi.h
extern int os_printf_plus(const char * format, ...) __attribute__ ((format (printf, 1, 2)));
#endif
#endif

#if UDEBUG
#define uprint(x...)	do { UPRINTF(x); } while (0)
#else
#define uprint(x...)	do { (void)0; } while (0)
#endif

#define uerror(x...)	do { UPRINTF(x); } while (0)
#define uassert(ass...)	do { if ((ass) == 0) { UPRINTF("assert fail: " #ass " @%s:%d\n", __FILE__, __LINE__); uhalt(); } } while (0)
#define uhalt()		do { (void)0; } while (0)
#define nl()		do { uprint("\n"); } while (0)


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
void		esp2glue_dhcps_start		(struct ip_info* info);
err_glue_t	esp2glue_dhcp_start		(int netif_idx);
void		esp2glue_netif_updated		(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw, glue_netif_flags_t flags, size_t hwlen, const uint8_t* hw /*, void* state*/);
err_glue_t	esp2glue_ethernet_input		(int netif_idx, void* glue_pbuf);
void		esp2glue_alloc_for_recv		(size_t len, void** glue_pbuf, void** glue_data);
void		esp2glue_pbuf_freed		(void* ref_saved);
void		esp2glue_netif_set_default	(int netif_idx);
void		esp2glue_netif_add		(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw, size_t hwlen, const uint8_t* hwaddr, uint16_t mtu);
void		esp2glue_netif_set_addr		(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw);
void		esp2glue_netif_set_updown	(int netif_idx, int up1_or_down0);

void		glue2esp_ifup			(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw);
err_glue_t	glue2esp_linkoutput		(int netif_idx, void* ref2save, void* data, size_t size);

#endif // GLUE_STUB_H
