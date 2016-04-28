#ifndef LWIP_SNTP_H
#define LWIP_SNTP_H

#include "lwip/opt.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The maximum number of SNTP servers that can be set */
#ifndef SNTP_MAX_SERVERS
#define SNTP_MAX_SERVERS           3
#endif

/** Set this to 1 to implement the callback function called by dhcp when
 * NTP servers are received. */
#ifndef SNTP_GET_SERVERS_FROM_DHCP
#define SNTP_GET_SERVERS_FROM_DHCP 0//LWIP_DHCP_GET_NTP_SRV
#endif

/* Set this to 1 to support DNS names (or IP address strings) to set sntp servers */
#ifndef SNTP_SERVER_DNS
#define SNTP_SERVER_DNS            1
#endif

/** One server address/name can be defined as default if SNTP_SERVER_DNS == 1:
 * #define SNTP_SERVER_ADDRESS "pool.ntp.org"
 */
uint32 sntp_get_current_timestamp();
char* sntp_get_real_time(long t);

void sntp_init(void);
void sntp_stop(void);

sint8 sntp_get_timezone(void);
bool sntp_set_timezone(sint8 timezone);
void sntp_setserver(u8_t idx, ip_addr_t *addr);
ip_addr_t sntp_getserver(u8_t idx);

#if SNTP_SERVER_DNS
void sntp_setservername(u8_t idx, char *server);
char *sntp_getservername(u8_t idx);
#endif /* SNTP_SERVER_DNS */

#if SNTP_GET_SERVERS_FROM_DHCP
void sntp_servermode_dhcp(int set_servers_from_dhcp);
#else /* SNTP_GET_SERVERS_FROM_DHCP */
#define sntp_servermode_dhcp(x)
#endif /* SNTP_GET_SERVERS_FROM_DHCP */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_SNTP_H */
