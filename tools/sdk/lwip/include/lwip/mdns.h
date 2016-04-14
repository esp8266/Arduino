/**
 * lwip MDNS resolver header file.
 *
 * Created on: Jul 29, 2010
 * Author: Daniel Toma


 * ported from uIP resolv.c Copyright (c) 2002-2003, Adam Dunkels.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LWIP_DNS_H__
#define __LWIP_DNS_H__

#include "lwip/opt.h"

#if LWIP_MDNS /* don't build if not configured for use in lwipopts.h */

/** DNS timer period */
#define DNS_TMR_INTERVAL          1000

/** mDNS Address offset flag*/
#define DNS_OFFSET_FLAG          0xC0     /* the offset flag in the DNS message */
#define DNS_DEFAULT_OFFSET       0x0C     /* the offset is set at the beginning of the DNS message */

#define DNS_IP_ADDR_LEN           4


/** DNS field TYPE used for "Resource Records" */
#define DNS_RRTYPE_A              1     /* a host address */
#define DNS_RRTYPE_NS             2     /* an authoritative name server */
#define DNS_RRTYPE_MD             3     /* a mail destination (Obsolete - use MX) */
#define DNS_RRTYPE_MF             4     /* a mail forwarder (Obsolete - use MX) */
#define DNS_RRTYPE_CNAME          5     /* the canonical name for an alias */
#define DNS_RRTYPE_SOA            6     /* marks the start of a zone of authority */
#define DNS_RRTYPE_MB             7     /* a mailbox domain name (EXPERIMENTAL) */
#define DNS_RRTYPE_MG             8     /* a mail group member (EXPERIMENTAL) */
#define DNS_RRTYPE_MR             9     /* a mail rename domain name (EXPERIMENTAL) */
#define DNS_RRTYPE_NULL           10    /* a null RR (EXPERIMENTAL) */
#define DNS_RRTYPE_WKS            11    /* a well known service description */
#define DNS_RRTYPE_PTR            12    /* a domain name pointer */
#define DNS_RRTYPE_HINFO          13    /* host information */
#define DNS_RRTYPE_MINFO          14    /* mailbox or mail list information */
#define DNS_RRTYPE_MX             15    /* mail exchange */
#define DNS_RRTYPE_TXT            16    /* text strings */
#define DNS_RRTYPE_SRV            33    /* Service record */
#define DNS_RRTYPE_OPT            41    /* EDNS0 OPT record */
#define DNS_RRTYPE_TSIG           250   /* Transaction Signature */
#define DNS_RRTYPE_ANY            255   /*Not a DNS type, but a DNS query type, meaning "all types"*/

/* DNS field CLASS used for "Resource Records" */
#define DNS_RRCLASS_IN            1    /* the Internet */
#define DNS_RRCLASS_CS            2     /* the CSNET class (Obsolete - used only for examples in some obsolete RFCs) */
#define DNS_RRCLASS_CH            3     /* the CHAOS class */
#define DNS_RRCLASS_HS            4     /* Hesiod [Dyer 87] */
#define DNS_RRCLASS_FLUSH         0x800 /* Flush bit */
#define DNS_RRCLASS_FLUSH_IN      0x8001/* Flush bit and Internet*/

/** Callback which is invoked when a hostname is found.
 * A function of this type must be implemented by the application using the DNS resolver.
 * @param name pointer to the name that was looked up.
 * @param ipaddr pointer to a struct ip_addr containing the IP address of the hostname,
 *        or NULL if the name could not be found (or on any other error).
 * @param callback_arg a user-specified callback argument passed to dns_gethostbyname
*/
#ifndef _MDNS_INFO
#define _MDNS_INFO
struct mdns_info {
	char *host_name;
	char *server_name;
	uint16 server_port;
	unsigned long ipAddr;
	char *txt_data[10];
};
#endif
//void 		   mdns_enable(void);
//void           mdns_disable(void);
//void           mdns_init(struct mdns_info *info);
//void           mdns_close(void);
//char* 		   mdns_get_hostname(void);
//void           mdns_set_hostname(char *name);
//void           mdns_set_servername(const char *name);
//char*          mdns_get_servername(void);
//void           mdns_server_unregister(void);
//void           mdns_server_register(void) ;
//void           mdns_tmr(void);
//void           Delay(unsigned long ulSeconds);

#endif /* LWIP_DNS */

#endif /* __LWIP_DNS_H__ */
