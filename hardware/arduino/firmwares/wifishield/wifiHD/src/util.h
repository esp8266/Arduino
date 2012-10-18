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
#ifndef _UTIL_H
#define _UTIL_H
#include <stdint.h>
#include <stdlib.h>
#include <wl_api.h>
#include "lwip/ip.h"
#include <console.h>

const char* ip2str(struct ip_addr addr);

struct ip_addr str2ip(const char* str);

uint8_t ascii_to_key(char *outp, const char *inp);

void print_network(struct wl_network_t* wl_network);

void print_network_list(void);

int join_argv(char *dst, size_t dst_len, int argc, char* argv[]);

void printbuf(const char *prefix, const void *data, size_t len);

const char* ssid2str(struct wl_ssid_t *ssid);

const char* mac2str(uint8_t mac[6]);

char* enc_type2str(enum wl_enc_type enc_type);

int equal_ssid(const struct wl_ssid_t* ssid1, 
	       const struct wl_ssid_t* ssid2);

int equal_bssid(const struct wl_mac_addr_t* bssid1, 
		const struct wl_mac_addr_t* bssid2);

#define NET_SET_SSID(net, xssid, ssid_len) do { \
                DE_MEMCPY((net)->ssid.ssid, (xssid), (ssid_len));   \
                (net)->ssid.len = (ssid_len);                   \
        } while (0) 

#define NET_SET_BSSID(net, xbssid) do { \
                DE_MEMCPY(&(net)->bssid, &(xbssid), sizeof (xbssid));  \
        } while (0)

#endif /* _UTIL_H */
