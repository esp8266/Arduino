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
#include <top_defs.h>
#include <util.h>
#include <stdint.h>
#include <stdlib.h>
#include <wl_api.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <printf-stdarg.h>

const char* ip2str(struct ip_addr addr)
{
    static char buf[16];

#if BYTE_ORDER == LITTLE_ENDIAN
        sniprintf(buf, sizeof(buf), "%lu.%lu.%lu.%lu",
                  addr.addr & 0xff,
                  (addr.addr >> 8) & 0xff,
                  (addr.addr >> 16) & 0xff,
                  (addr.addr >> 24) & 0xff);
        return buf;
#else
        sniprintf(buf, sizeof(buf), "%lu.%lu.%lu.%lu",
                 (addr.addr >> 24) & 0xff,
                 (addr.addr >> 16) & 0xff,
                 (addr.addr >> 8) & 0xff,
                 (addr.addr) & 0xff);
        return buf;
#endif
}

struct ip_addr str2ip(const char* str)
{
        int a,b,c,d;
        uint32_t ip = 0;
        struct ip_addr addr;
        
        if (siscanf(str,"%d.%d.%d.%d",&a,&b,&c,&d) != 4)
                goto out;

        if (a < 0 || a > 255 || b < 0 || b > 255 ||
            c < 0 || c > 255 || d < 0 || d > 255) {
                goto out;
        }

#if BYTE_ORDER == LITTLE_ENDIAN
        ip = (d << 24) | (c << 16) | (b << 8) | a;
#else
        ip = (a << 24) | (b << 16) | (c << 8) | d;
#endif

 out:
        addr.addr = ip;
        return addr;
}

uint8_t ascii_to_key(char *outp, const char *inp) {
        char buf[3];
        int len;
        buf[2] = '\0';
        len = strlen(inp);
        if (len % 2) {
                printk("Invalid length\n");
        }
        len = 0;
        while (*inp) {
                if (! isxdigit(*inp) || ! isxdigit(*(inp+1)) ||
                    len > WL_MAX_PASS_LEN) {
                        return 0;
                }
                buf[0] = *inp++;
                buf[1] = *inp++;
                *outp++ = strtol(buf, NULL, 16);
                len++;
        }
        return len;
}



void printbuf(const char *prefix, const void *data, size_t len)
{
        const unsigned char *s = data;
        int i, j;

        for (i = 0; i < len; i += 16) 
        {
                printk("%s ", prefix);
                for(j = 0; j < 16; j++) {
                        if(i + j >= len)
                                printk("   ");
                        else
                                printk("%02X ", (uint16_t)s[i + j]);
                }
                printk(": ");
                for(j = 0; j < 16; j++) {
                        if(i + j >= len)
                                break;
                        if(s[i+j] >= 32 && s[i+j] < 127)
                                printk("%c", s[i + j]);
                        else
                                printk(".");
                }
                printk("\n");
        }
}


void print_network(struct wl_network_t* wl_network)
{
        printk("%s ", mac2str(wl_network->bssid.octet));
        printk("\"%s\"", ssid2str(&wl_network->ssid));
        printk(" RSSI %d dBm ", wl_network->rssi);
        switch(wl_network->net_type) {
        case WL_CONN_TYPE_ADHOC:
                printk(" Ad-Hoc ");
                break;
        default :
                break;
        }
        switch (wl_network->enc_type) {
        case ENC_TYPE_WEP :
                printk(" (WEP encryption)");
                break;
        case ENC_TYPE_TKIP :
                printk(" (TKIP encryption)");
                break;
        case ENC_TYPE_CCMP :
                printk(" (CCMP encryption)");
                break;
        case ENC_TYPE_NONE :
                break;
        }
        printk("\n");
        
}

void print_network_list(void)
{
        struct wl_network_list_t* wl_network_list;
        uint8_t i;
        
        wl_get_network_list(&wl_network_list);

        if (wl_network_list->cnt == 0)
                printk("no nets found\n");

        for (i = 0; i < wl_network_list->cnt; i++)
                print_network(wl_network_list->net[i]);
}

int join_argv(char *dst, size_t dst_len, int argc, char* argv[]) {
        char *p = dst;
        int i;
        int len = 0;

        /* Not really kosher, an ssid may legally contain 0-bytes but
         * the console interface does not deal with that.
         */
        for (i = 0; i < argc; i++) {
                len += strlen(argv[i]);
                if (len > dst_len) {
			printk("ssid too long (max %d)\n", (int) dst_len);
                        return 0;
                }
                p += sniprintf(p,
                               dst_len - (p - dst), 
                               "%s ", 
                               argv[i]);
        }
        if (p == dst) {
                return 0;
        }
        p--;
        *p = '\0'; /* Delete last space */

        return p - dst;
}

const char* ssid2str(struct wl_ssid_t *ssid) {
        static char buf[WL_SSID_MAX_LENGTH + 1];

        memset(buf, 0, sizeof buf);
        memcpy(buf, ssid->ssid, ssid->len);

        return buf;
}


const char* mac2str(uint8_t* mac)
{
        static char buf[18];
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


