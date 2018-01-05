#ifndef __SNTP_H__
#define __SNTP_H__

#include "os_type.h"
#ifdef LWIP_OPEN_SRC
#include "lwip/ip_addr.h"
#else
#include "ip_addr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * get the seconds since Jan 01, 1970, 00:00 (GMT + 8)
 */
uint32 sntp_get_current_timestamp();
/**
 * get real time (GTM + 8 time zone)
 */
char* sntp_get_real_time(long t);
/**
 * SNTP get time_zone default GMT + 8
 */
sint8 sntp_get_timezone(void);
/**
 * SNTP set time_zone (default GMT + 8)
 */
bool sntp_set_timezone(sint8 timezone);
/**
 * Initialize this module.
 * Send out request instantly or after SNTP_STARTUP_DELAY(_FUNC).
 */
void sntp_init(void);
/**
 * Stop this module.
 */
void sntp_stop(void);
/**
 * Initialize one of the NTP servers by IP address
 *
 * @param numdns the index of the NTP server to set must be < SNTP_MAX_SERVERS
 * @param dnsserver IP address of the NTP server to set
 */
void sntp_setserver(unsigned char idx, ip_addr_t *addr);
/**
 * Obtain one of the currently configured by IP address (or DHCP) NTP servers
 *
 * @param numdns the index of the NTP server
 * @return IP address of the indexed NTP server or "ip_addr_any" if the NTP
 *         server has not been configured by address (or at all).
 */
ip_addr_t sntp_getserver(unsigned char idx);
/**
 * Initialize one of the NTP servers by name
 *
 * @param numdns the index of the NTP server to set must be < SNTP_MAX_SERVERS,now sdk support SNTP_MAX_SERVERS = 3
 * @param dnsserver DNS name of the NTP server to set, to be resolved at contact time
 */
void sntp_setservername(unsigned char idx, char *server);
/**
 * Obtain one of the currently configured by name NTP servers.
 *
 * @param numdns the index of the NTP server
 * @return IP address of the indexed NTP server or NULL if the NTP
 *         server has not been configured by name (or at all)
 */
char *sntp_getservername(unsigned char idx);

#define sntp_servermode_dhcp(x)


#ifdef __cplusplus
}
#endif

#endif
