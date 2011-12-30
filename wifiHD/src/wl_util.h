/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*! \file wl_util.h **************************************************************
 *
 * \brief Optional WiFi support/convenience functions
 *
 * This file provides a support interface for wl_api.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  EVK1104 with SPB104 card
 * - AppNote:
 *
 * \author               H&D Wireless: \n
 *
 *****************************************************************************
 */

#include "top_defs.h"
#include <stdint.h>
#include <string.h>
#include "wl_api.h"

#ifndef WL_UTIL_H
#define WL_UTIL_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Important note!
 *
 * These functions are used internally by the wl_api library.  Since
 * they are useful for the application using the wl_api library they
 * are included as source code. To avoid duplicating code these
 * functions are declared as weak symbols so that the ones compiled
 * into the library will be replaced by the ones compiled with the
 * application (if any). However, this means that these functions
 * _must not_ be rewritten as the library depends on the current
 * implementation. If other functionality is desired then these
 * functions should be renamed.
 */

/*! Return a buffer with the SSID as a printable string.
 * The returned buffer is static and should not be freed and 
 * this function is not
 * thread safe.
 */
const char* ssid2str(struct wl_ssid_t *ssid) WEAK_DECL;

const char* mac2str(uint8_t mac[6]) WEAK_DECL;

char* enc_type2str(enum wl_enc_type enc_type) WEAK_DECL;

int equal_ssid(const struct wl_ssid_t* ssid1, 
	       const struct wl_ssid_t* ssid2) WEAK_DECL;

int equal_bssid(const struct wl_mac_addr_t* bssid1, 
		const struct wl_mac_addr_t* bssid2) WEAK_DECL;


#define NET_SET_SSID(net, xssid, ssid_len) do { \
                memcpy((net)->ssid.ssid, (xssid), (ssid_len));   \
                (net)->ssid.len = (ssid_len);                   \
        } while (0) 

#define NET_SET_BSSID(net, xbssid) do { \
                memcpy(&(net)->bssid, &(xbssid), sizeof (xbssid));  \
        } while (0)


#endif /* WL_UTIL_H */
