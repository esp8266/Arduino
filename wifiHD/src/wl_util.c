/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

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
#include "top_defs.h"
#include "wl_util.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

int equal_ssid(const struct wl_ssid_t* ssid1, 
	       const struct wl_ssid_t* ssid2) {
        if (ssid1->len == ssid2->len &&
            (memcmp(ssid1->ssid, ssid2->ssid, ssid1->len) == 0)) {
                return 1;
        }
        return 0;
}

int equal_bssid(const struct wl_mac_addr_t* bssid1, 
		const struct wl_mac_addr_t* bssid2) {
        if (memcmp(bssid1, bssid2, sizeof *bssid1) == 0) {
                return 1;
        }
        return 0;
}

const char* ssid2str(struct wl_ssid_t *ssid) {
        static char buf[WL_SSID_MAX_LENGTH + 1];

        memset(buf, 0, sizeof buf);
        memcpy(buf, ssid->ssid, ssid->len);

        return buf;
}


const char* mac2str(uint8_t* mac)
{
        static char buf[18] ALIGN;
        sniprintf(buf, sizeof(buf), "%02x-%02x-%02x-%02x-%02x-%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return buf;
}


char* enc_type2str(enum wl_enc_type enc_type)
{
        switch(enc_type) {
        case ENC_TYPE_WEP:
                return "WEP";
        case ENC_TYPE_CCMP:
                return "CCMP";
        case ENC_TYPE_TKIP:
                return "TKIP";
        default:
                return "";
        };
}
