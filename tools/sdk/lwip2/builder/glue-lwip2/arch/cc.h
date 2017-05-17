
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

// version for esp8266 sdk-2.0.0(656edbf)

#ifndef LWIP2_ARCH_CC_H
#define LWIP2_ARCH_CC_H

#include "stdint.h"

#ifdef LWIP_BUILD

// define LWIP_BUILD only when building LWIP
// otherwise include files below would conflict
// with standard headers like atoi()
#include "ets_sys.h"
#include "osapi.h"
#include "esp-missing.h"

void sntp_set_system_time (uint32_t t);

#endif // defined(LWIP_BUILD)

#include "mem.h" // useful for os_malloc used in esp-arduino's mDNS

typedef int sys_prot_t;	// not really used
#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev) os_intr_lock()
#define SYS_ARCH_UNPROTECT(lev) os_intr_unlock()

///////////////////////////////
//// DEBUG
#if 0 // debug 1:on or 0

//#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)
#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)
//#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON)

#define LWIP_DEBUG 1

//int doprint (const char* format, ...) __attribute__ ((format (printf, 1, 2)));
extern int os_printf_plus(const char * format, ...) __attribute__ ((format (printf, 1, 2)));
#define LWIP_PLATFORM_DIAG(x) do { os_printf x;} while(0)

#else
#define LWIP_NOASSERT 1
#endif // debug

///////////////////////////////
//// MISSING 

#define sys_now millis		// arduino wire millis() definition returns 32 bits like sys_now() does
#define LWIP_RAND r_rand	// old lwip uses this useful undocumented function

// ip_addr / ip_info: do not exist in lwip2 (only in lwip1.4)
struct ip_addr {
  uint32_t addr;
};
struct ip_info {
    struct ip_addr ip;
    struct ip_addr netmask;
    struct ip_addr gw;
};

///////////////////////////////
//// PROVIDED TO USER

typedef struct ip4_addr ip4_addr_t;
extern int ntp_servers_number;
extern ip4_addr_t* ntp_servers;

///////////////////////////////
//// STUBS

// these symbols must be renamed in the new implementation
// because they are known/used in blobs

#define dhcp_cleanup dhcp_cleanup_LWIP2
#define dhcp_release dhcp_release_LWIP2
#define dhcp_start dhcp_start_LWIP2
#define dhcp_stop dhcp_stop_LWIP2
#define dhcps_start dhcps_start_LWIP2
#define dhcps_stop dhcps_stop_LWIP2
#define espconn_init espconn_init_LWIP2
#define etharp_output etharp_output_LWIP2
#define ethbroadcast ethbroadcast_LWIP2
#define ethernet_input ethernet_input_LWIP2
#define lwip_init lwip_init_LWIP2
#define netif_add netif_add_LWIP2
#define netif_default netif_default_LWIP2
#define netif_remove netif_remove_LWIP2
#define netif_set_addr netif_set_addr_LWIP2
#define netif_set_default netif_set_default_LWIP2
#define netif_set_down netif_set_down_LWIP2
#define netif_set_up netif_set_up_LWIP2
#define pbuf_alloc pbuf_alloc_LWIP2
#define pbuf_free pbuf_free_LWIP2
#define pbuf_ref pbuf_ref_LWIP2
//#define sys_check_timeouts sys_check_timeouts_LWIP2		// void(void)

#if !defined(LWIP_DEBUG) || !SYS_DEBUG
#define sys_timeout sys_timeout_LWIP2
#endif

#define sys_untimeout sys_untimeout_LWIP2

///////////////////////////////
#endif // LWIP2_ARCH_CC_H
