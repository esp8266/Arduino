#ifndef __SNTP_H__
#define __SNTP_H__

#include "os_type.h"

#include "lwip/init.h"
#include "lwip/ip_addr.h"
#if LWIP_VERSION_MAJOR == 1
#include "lwip/sntp.h"
#else
#include "lwip/apps/sntp.h"
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

#ifdef __cplusplus
}
#endif

#endif
