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
#include <ctype.h>

#include "cmd_wl.h"

#include "wl_cm.h"
#include "console.h"
#include "wl_util.h"
#include "util.h"
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "debug.h"

extern void showTTCPstatus();

#define ENABLE_DEBUG_LEVEL 1
#define VERBOSE_DEBUG_LEVEL 2

#define CHECK_ENA_DEBUG(LEVEL, FLAG)  		\
	do{										\
		if (LEVEL >= ENABLE_DEBUG_LEVEL) enableDebug |= FLAG;		\
		else enableDebug &= ~FLAG;			\
		}while(0);

#define CHECK_VERB_DEBUG(LEVEL, FLAG)  		\
	do{										\
		if (LEVEL >= VERBOSE_DEBUG_LEVEL) verboseDebug |= FLAG;		\
		else verboseDebug &= ~FLAG;			\
		}while(0);

/**
 *
 */
cmd_state_t 
cmd_scan(int argc, char* argv[], void* ctx)
{
        /* Note that the scan results presented will
         * be from the last scan, not this one.
         */
        wl_scan();
        print_network_list();
        return CMD_DONE;
}


/**
 *
 */
cmd_state_t 
cmd_connect(int argc, char* argv[], void* ctx)
{
        struct wl_ssid_t ssid;
        char desired_ssid[WL_SSID_MAX_LENGTH];
        int len = 0;
        
        if (argc < 2) {
                printk("usage: connect <ssid>\n");
                return CMD_DONE;
        }
        
        len = join_argv(desired_ssid, sizeof desired_ssid, argc - 1, argv + 1);
        if (0 == len) {
                return CMD_DONE;
        }

        memcpy(ssid.ssid, desired_ssid, len);
        ssid.len = len;
        wl_cm_set_network(&ssid, NULL);
        return CMD_DONE;
}


#ifdef WITH_WPA

/**
 *
 */
cmd_state_t
cmd_delpass(int argc, char* argv[], void* ctx)
{
        const char *usage = "usage: dpass <ssid>\n";
        struct wl_network_t net;
        char desired_ssid[WL_SSID_MAX_LENGTH];
        int len = 0;

        if (argc != 2) {
                printk(usage);
                return CMD_DONE;
        }

        memset(&net, 0, sizeof net);
        memset(net.bssid.octet, 0xFF, sizeof net.bssid.octet);

        len = join_argv(desired_ssid, sizeof desired_ssid, argc - 1, argv + 1);
        if (0 == len) {
                return CMD_DONE;
        }
        memcpy(net.ssid.ssid, desired_ssid, len);
        net.ssid.len = len;
        net.enc_type = ENC_TYPE_AUTO;
        if (wl_clear_passphrase(&net) != WL_SUCCESS) {
                printk("%s : Failed to delete passphrase\n", __func__);
        }

        return CMD_DONE;
}


/**
 *
 */
cmd_state_t
cmd_setpass(int argc, char* argv[], void* ctx)
{
        const char *usage = "usage: wpass <ssid> <passphrase>\n";
        struct wl_network_t net;
        char desired_ssid[WL_SSID_MAX_LENGTH];
        int len = 0;

        if (argc < 3) {
                printk(usage);
                return CMD_DONE;
        }
        /* Not really kosher, an ssid may legally contain 0-bytes but
         * the console interface does not deal with that.
         */
        memset(&net, 0, sizeof net);
        memset(net.bssid.octet, 0xFF, sizeof net.bssid.octet);

        len = join_argv(desired_ssid, sizeof desired_ssid, argc - 2, argv + 1);
        if (0 == len) {
                return CMD_DONE;
        }

        memcpy(net.ssid.ssid, desired_ssid, len);
        net.ssid.len = len;
        net.enc_type = ENC_TYPE_AUTO;
        if (wl_set_passphrase(&net, 
                              argv[argc - 1], 
                              strlen(argv[argc - 1]), 
                              ENC_TYPE_AUTO,
                              AUTH_MODE_AUTO) 
            != WL_SUCCESS) {
                printk("%s : Failed to add passphrase\n", __func__);
        }

        return CMD_DONE;
}
#endif

#ifdef _DNS_CMD_
void foundHost(const char *name, struct ip_addr *ipaddr, void *callback_arg)
{
	printk("Found Host: name=%s ip=0x%x\n", name, ipaddr->addr);
}

/**
 *
 */
cmd_state_t
cmd_gethostbyname(int argc, char* argv[], void* ctx)
{
        const char *usage = "usage: getHost <hostname>\n";
        char hostname[DNS_MAX_NAME_LENGTH];
        struct ip_addr _addr;
        int len = 0;

        if (argc < 2) {
                printk(usage);
                return CMD_DONE;
        }

        len = join_argv(hostname, sizeof hostname, argc - 1, argv + 1);
        if (0 == len) {
                return CMD_DONE;
        }
        err_t err = dns_gethostbyname(hostname, &_addr, foundHost, NULL);
        if (err == ERR_OK)
        {
        	printk("Found Host: name=%s ip=0x%x\n", hostname, _addr.addr);
        }

        return CMD_DONE;
}
#endif


/**
 *
 */
cmd_state_t
cmd_status(int argc, char* argv[], void* ctx)
{
        struct wl_network_t* net;
        uint8_t mac[WL_MAC_ADDR_LENGTH];

        printk("wl_api version " WL_API_RELEASE_NAME "\n");
        /* print mac address */
        if (wl_get_mac_addr(mac) != WL_SUCCESS) {
                printk("failed to get mac address\n");
                return CMD_DONE;
        }
        printk("hw addr: %s\n", mac2str(mac));

        /* print network info */
        net = wl_get_current_network();
        printk("link status: "); 
        if (!net) { 
                printk("down\n");
                return CMD_DONE;
        }
        print_network(net);
        
        /* print ip address */
        if (netif_is_up(netif_default))
                printk("ip addr: %s\n", ip2str(netif_default->ip_addr));
        else
                printk("ip addr: none\n");

        showTTCPstatus();
        return CMD_DONE;
}

#ifdef ADD_CMDS
/**
 *
 */
cmd_state_t
cmd_power(int argc, char* argv[], void* ctx)
{
        const char *usage = "usage: powersave <on|off>\n";
        
        if (argc < 2) {
                printk(usage);
                return CMD_DONE;
        }
        
        if (!strcmp(argv[1], "on")) {
                if (wl_enable_ps() != WL_SUCCESS) {
                        printk("could not enable power save\n");
                        return CMD_DONE;
                }
                return CMD_DONE;
        }
        else if(!strcmp(argv[1], "off")) {
                if (wl_disable_ps() != WL_SUCCESS) {
                        printk("could not disable power save\n");
                        return CMD_DONE;
                }
                return CMD_DONE;
        }

        printk(usage);
        return CMD_DONE;
}
#endif

#ifdef ADD_CMDS
/**
 *
 */
cmd_state_t
cmd_psconf(int argc, char* argv[], void* ctx)
{
        const char *usage = 
                "usage: psconf <use_ps_poll>      (0/1       default 0)\n" \
                "              <traffic_timeout>  ([ms]      default 10)\n" \
                "              <ps_delay>         ([ms]      default 5000)\n"\
                "              <rx_all_dtim>      (0/1       default 1)\n"\
                "              <listen_interval>  ([beacons] default 20)\n";
        
        uint8_t use_ps_poll;
        uint32_t traffic_timeout;
        uint32_t ps_delay;
        uint8_t rx_all_dtim;
        uint16_t listen_interval;
        
        if (argc < 6) {
                printk(usage);
                return CMD_DONE;
        }
        
        use_ps_poll = atoi(argv[1]);
        traffic_timeout = atoi(argv[2]);
        ps_delay = atoi(argv[3]);
        rx_all_dtim = atoi(argv[4]);
        listen_interval = atoi(argv[5]);

        if (use_ps_poll > 1) {
                printk(usage);
                return CMD_DONE;
        }

        if (rx_all_dtim > 1) {
                printk(usage);
                return CMD_DONE;
        }

        if (wl_conf_ps(use_ps_poll, traffic_timeout, ps_delay,
                       rx_all_dtim, listen_interval) != WL_SUCCESS)
                printk("configuration failed\n");
        
        return CMD_DONE;
}
#endif

#define MAX_KEY_LEN 64

/**
 *
 */
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
                    len > MAX_KEY_LEN) {
                        return 0;
                }
                buf[0] = *inp++;
                buf[1] = *inp++;
                *outp++ = strtol(buf, NULL, 16);
                len++;
        }
        return len;
}


/**
 *
 */
cmd_state_t
cmd_setkey(int argc, char* argv[], void* ctx)
{
        int idx, len;
        char key[13];
        struct wl_mac_addr_t bssid;
        const char *usage = "usage: setkey <key_idx (0-3)> <key in hex>\n\t "\
                "or: setkey none\n";

        memset(&bssid.octet, 0xff, sizeof bssid.octet);
        if (argc == 2 && strcmp(argv[1], "none") == 0) {
                printk("Deleting WEP keys\n");
                wl_set_auth_mode(AUTH_MODE_OPEN_SYSTEM);
                wl_delete_wep_key(0, &bssid);
                wl_delete_wep_key(1, &bssid);
                wl_delete_wep_key(2, &bssid);
                wl_delete_wep_key(3, &bssid);
                return CMD_DONE;
        }
        if (argc < 3) {
                printk(usage);
                return CMD_DONE;
        }
        idx = atoi(argv[1]);
        len = ascii_to_key(key, argv[2]);
        if (0 == len || idx > 3 || idx < 0 || (idx == 0 && *argv[1] != '0')) {
                printk(usage);
                return CMD_DONE;
        }
        if (len != 5 && len != 13) {
                printk(" WEP key must be 10 (WEP-40) or 26 (WEP-104) digits\n");
                return CMD_DONE;
        }
        wl_add_wep_key(idx, len, key, &bssid);
        wl_set_auth_mode(AUTH_MODE_SHARED_KEY);
        wl_set_default_wep_key(idx);

        return CMD_DONE;
}

cmd_state_t
cmd_debug(int argc, char* argv[], void* ctx)
{
        int level;
        const char *usage = "usage: debug <section> <level>\n\t"\
        		"section: init, cm, spi, tcp , util, warn\n\t"
        		"level  : 0 (off), 1 (on), 2 (verbose)\n\t"
                "or: debug print/on/off\n";

        if (argc == 2 && strcmp(argv[1], "off") == 0) {
                printk("Debug OFF\n");
                enableDebug = 0;
                verboseDebug = 0;
                return CMD_DONE;
        }else if (argc == 2 && strcmp(argv[1], "print") == 0) {
            printk("Debug enabled: 0x%x\n", enableDebug);
            printk("Verbose enabled: 0x%x\n", verboseDebug);
            return CMD_DONE;
        }else if (argc == 2 && strcmp(argv[1], "on") == 0) {
            printk("Debug ON\n");
            enableDebug = 0xff;
            return CMD_DONE;
        }
        if (argc < 3) {
                printk(usage);
                return CMD_DONE;
        }
        level = atoi(argv[2]);
        if (argc == 3 && strcmp(argv[1], "init") == 0) {
        	CHECK_ENA_DEBUG(level, INFO_INIT_FLAG);
        	CHECK_VERB_DEBUG(level, INFO_INIT_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "spi") == 0) {
        	CHECK_ENA_DEBUG(level, INFO_SPI_FLAG);
        	CHECK_VERB_DEBUG(level, INFO_SPI_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "tcp") == 0) {
        	CHECK_ENA_DEBUG(level, INFO_TCP_FLAG);
        	CHECK_VERB_DEBUG(level, INFO_TCP_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "cm") == 0) {
        	CHECK_ENA_DEBUG(level, INFO_CM_FLAG);
        	CHECK_VERB_DEBUG(level, INFO_CM_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "util") == 0) {
        	CHECK_ENA_DEBUG(level, INFO_UTIL_FLAG);
        	CHECK_VERB_DEBUG(level, INFO_UTIL_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "warn") == 0) {
        	CHECK_ENA_DEBUG(level, INFO_WARN_FLAG);
        	CHECK_VERB_DEBUG(level, INFO_WARN_FLAG);
        }
        return CMD_DONE;
}
