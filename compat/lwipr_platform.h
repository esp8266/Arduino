/*
 * lwipr_platform.h
 *
 *  Created on: Feb 8, 2016
 *      Author: slavey
 *
 */

#ifndef AXTLS_8266_COMPAT_LWIPR_PLATFORM_H_
#define AXTLS_8266_COMPAT_LWIPR_PLATFORM_H_

/* Add here all platform specific things */


// Some calls require the watchdog to be reset
#ifndef WATCHDOG_FEED
	#define WATCHDOG_FEED()
#endif


/* SSL_DEBUG is for more information */
#ifndef SSL_DEBUG
	#define AXL_DEBUG(...)
#endif


#endif /* AXTLS_8266_COMPAT_LWIPR_PLATFORM_H_ */
