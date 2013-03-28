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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <cmd_wl.h>

#include <wl_cm.h>
#include <console.h>
#include <util.h>
#include <lwip_setup.h>

#include "lwip/netif.h"
#include "lwip/dns.h"
#include "debug.h"
#include "ard_spi.h"
#include "ard_tcp.h"
#include "ard_utils.h"

extern void showTTCPstatus();

#define  _DNS_CMD_

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

cmd_state_t 
cmd_debug_toggle(int argc, char* argv[], void* ctx)
{
        extern uint8_t tr_data_trace;
        if ( argc != 2 ) {
                printk("usage: dt <1|0>\n");
                return CMD_DONE;
        }
        if ( '0' == argv[1][0] ) {
                tr_data_trace = 0;
        }
        if ( '1' == argv[1][0] ) {
                tr_data_trace = 1;
        }
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
        /* Start connection manager */
        wl_cm_set_network(&ssid, NULL);
        wl_cm_start();
        return CMD_DONE;
}

#ifdef WFE_6_12
cmd_state_t 
cmd_ibss(int argc, char* argv[], void* ctx)
{
        struct wl_ssid_t ssid;
        char desired_ssid[WL_SSID_MAX_LENGTH];
        uint8_t channel;
        enum wl_auth_mode amode;
        int len = 0;
        wl_err_t ret;
        
        if ( 2 == argc && ! strncmp(argv[1], "none", 4) ) {
                printk("Disconnecting\n");
                wl_disconnect();
                wl_cm_stop();
                return CMD_DONE;
        }
        if (argc < 4) {
                printk("usage: ibss <ssid> <channel (1-14)> <wep_enable (1|0)>\n");
                printk("       ibss none\n");
                return CMD_DONE;
        }
        
        channel = atoi(argv[argc - 2]);
        if ( *argv[argc - 1] == '0' ) {
                amode = AUTH_MODE_OPEN_SYSTEM;
        } else {
                amode = AUTH_MODE_SHARED_KEY;
        }
        len = join_argv(desired_ssid, sizeof desired_ssid, argc - 3, argv + 1);
        if (0 == len) {
                return CMD_DONE;
        }
        if ( channel > 14 ) {
                printk("Invalid channel %d\n", (int)channel);
                return CMD_DONE;
        }
        printk("%s : Start with ssid \"%s\", channel %d\n", __func__,
               desired_ssid, channel);
        memcpy(ssid.ssid, desired_ssid, len);
        ssid.len = len;
        /* Stop the connection manager */
        wl_cm_stop();
        
        ret = wl_start_adhoc_net(ssid, channel, amode);
        switch (ret) {
        case WL_BUSY:
                printk("Driver is busy. Already connected?\n");
                break;
        case WL_RETRY:
                printk("Driver is busy. Retry operation\n");
                break;
        case WL_OOM:
                printk("Out of memory\n");
                break;
        case WL_INVALID_ARGS:
                printk("Invalid argument\n");
                break;
        case WL_SUCCESS:
                break;
        default:
                printk("Unknown error %d\n", ret);
                break;
        }
        return CMD_DONE;
}
#endif
/**
 *
 */
cmd_state_t 
cmd_set_ip(int argc, char* argv[], void* ctx)
{
	struct ctx_server *hs = ctx;
    struct net_cfg *ncfg = &(hs->net_cfg);
        struct ip_addr lwip_addr;
        struct netif *nif = ncfg->netif;

        if (argc == 2 && 
            (strncmp(argv[1], "none", 4) == 0)) {
                ncfg->dhcp_enabled = DYNAMIC_IP_CONFIG;
                
                return CMD_DONE;
        }
        else if (argc != 4 ) {
                printk("usage: ipconfig <ip> <netmask> <gateway-ip>\n");
                printk("  or : ipconfig none (to enable DHCP)\n");
                return CMD_DONE;
        }

        /* IP address */
        lwip_addr = str2ip(argv[1]);
        INFO_SPI("nif:%p lwip_addr=0x%x\n", nif, lwip_addr.addr);
        netif_set_ipaddr(nif, &lwip_addr);
        /* Netmask */
        lwip_addr = str2ip(argv[2]);
        netif_set_netmask(nif, &lwip_addr);
        /* Default Gateway address */
        lwip_addr = str2ip(argv[3]);
        netif_set_gw(nif, &lwip_addr);
        /* Disable DHCP */
        ncfg->dhcp_enabled = STATIC_IP_CONFIG;

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

/**
 *
 */
cmd_state_t
cmd_setDnsServer(int argc, char* argv[], void* ctx)
{
        const char *usage = "usage: setdns [1-2] aaa.bbb.ccc.ddd\n";
        struct ip_addr dnsIp;
        int dnsIdx = 0;

        if (argc < 3) {
                printk(usage);
                return CMD_DONE;
        }

        /* DNS IDX */
        dnsIdx = atoi(argv[1])-1;
        /* IP address */
        dnsIp = str2ip(argv[2]);

        printk("Set DNS server %d to %s\n", dnsIdx, ip2str(dnsIp));
        dns_setserver(dnsIdx, &dnsIp);
        struct ip_addr addr1 = dns_getserver(0);
        struct ip_addr addr2 = dns_getserver(1);

        printk("==> DNS1: %s\n", ip2str(addr1), addr1);
        printk("==> DNS2: %s\n", ip2str(addr2), addr2);

        return CMD_DONE;
}

/**
 *
 */
cmd_state_t
cmd_startSrv(int argc, char* argv[], void* ctx)
{
        const char *usage = "usage: startSrv <port> <sock> <tcp(0)/udp(1)>\n";

        int port = 0;
        int sock = 0;
		int protMode = 0;

        if (argc < 4) {
                printk(usage);
                return CMD_DONE;
        }

        /* TCP port */
        port = atoi(argv[1]);
        /* socket index */
        sock = atoi(argv[2]);
		/* Protocol Mode */
		protMode = atoi(argv[3]);

        printk("Start %s server on port %d sock %d\n", ProtMode2Str(protMode), port, sock);
        if (start_server_tcp(port, sock, protMode) == -1)
        {
        	WARN("Start %s server on port %d sock %d FAILED\n", ProtMode2Str(protMode), port, sock);
        }
        return CMD_DONE;
}

/**
 *
 */
cmd_state_t
cmd_startCli(int argc, char* argv[], void* ctx)
{
        const char *usage = "usage: startCli <ipaddr> <port> <sock> <tcp(0)/udp(1)>\n";
		struct ip_addr addr = {0};
        int port = 0;
        int sock = 0;
		int protMode = 0;

        if (argc < 5) {
                printk(usage);
                return CMD_DONE;
        }

        /* IP address */
        addr = str2ip(argv[1]);
        /* TCP port */
        port = atoi(argv[2]);
        /* socket index */
        sock = atoi(argv[3]);
		/* Protocol Mode */
		protMode = atoi(argv[4]);

        printk("Start client on addr 0x%x, port %d sock %d mode %d\n", addr, port, sock, protMode);
        if (start_client_tcp(addr.addr, port, sock, protMode) == -1)
        {
        	WARN("Start client on port %d sock %d prot %d mode %d FAILED\n", port, sock, protMode);
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
        struct net_cfg *ncfg = ctx;
        struct wl_network_t* net;
        uint8_t mac[WL_MAC_ADDR_LENGTH];

        printk("wl_api version " WL_API_RELEASE_NAME "\n");
        /* print mac address */
        if (wl_get_mac_addr(mac) != WL_SUCCESS) {
                printk("failed to get mac address\n");
        }else{
        printk("hw addr: %s\n", mac2str(mac));
        }

        /* print network info */
        net = wl_get_current_network();
        printk("link status: "); 
        if (!net) { 
                printk("down\n");

        }else{
        print_network(net);
        }
        
        /* print ip address */
        if (netif_is_up(netif_default))
		{
                printk("ip addr: %s - ", ip2str(netif_default->ip_addr));
				printk("netmask: %s - ", ip2str(netif_default->netmask));
				printk("gateway: %s\n", ip2str(netif_default->gw));
		}					
        else
                printk("ip interface is down\n");
        printk("dhcp : ");
        if (ncfg->dhcp_enabled == DYNAMIC_IP_CONFIG) {
                printk("enabled\n");
        }
        else {
                printk("disabled\n");
        }
        struct ip_addr addr1 = dns_getserver(0);
        struct ip_addr addr2 = dns_getserver(1);

        printk("DNS: %s - ", ip2str(addr1));
		printk("%s\n", ip2str(addr2));

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
        len = strlen(argv[2]);
        /* Pass phrase? */
        if ( 5 == len || 13 == len ) {
                strncpy(key, argv[2], len);
        }
        /* Otherwise it's a hex string */
        else {
                len = ascii_to_key(key, argv[2]);
                if (0 == len || idx > 3 || idx < 0 || (idx == 0 && *argv[1] != '0')) {
                        printk(usage);
                        return CMD_DONE;
                }
                if (len != 5 && len != 13) {
                        printk(" WEP key must be 10 (WEP-40) or 26 (WEP-104) digits\n");
                        return CMD_DONE;
                }
        }
        wl_add_wep_key(idx, len, key, &bssid);
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
                INIT_DEBUG_VARIABLES()
                return CMD_DONE;
        }else if (argc == 2 && strcmp(argv[1], "print") == 0) {
        	PRINT_DEBUG_VARIABLES()
            return CMD_DONE;
        }else if (argc == 2 && strcmp(argv[1], "on") == 0) {
            printk("Debug ON\n");
            TURNON_DEBUG_VARIABLES();
            return CMD_DONE;
        }
        if (argc < 3) {
                printk(usage);
                return CMD_DONE;
        }
        level = atoi(argv[2]);
        if (argc == 3 && strcmp(argv[1], "init") == 0) {
        	CHECK_DEBUG_LEVEL(level, INFO_INIT_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "spi") == 0) {
        	CHECK_DEBUG_LEVEL(level, INFO_SPI_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "tcp") == 0) {
        	CHECK_DEBUG_LEVEL(level, INFO_TCP_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "cm") == 0) {
        	CHECK_DEBUG_LEVEL(level, INFO_CM_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "util") == 0) {
        	CHECK_DEBUG_LEVEL(level, INFO_UTIL_FLAG);
        }else if (argc == 3 && strcmp(argv[1], "warn") == 0) {
        	CHECK_DEBUG_LEVEL(level, INFO_WARN_FLAG);
        }
        return CMD_DONE;
}

extern void dumpPbuf(uint8_t sock);

/**
 *
 */
cmd_state_t
cmd_dumpBuf(int argc, char* argv[], void* ctx)
{
    const char *usage = "usage: dumpPbuf [sock]\n\t"\
    		"sock: socket Number\n";

    if (argc == 2 && strcmp(argv[1], "all") == 0) {
    	printk("Dump All Buffers\n");
    	int i = 0;
    	for (; i<MAX_SOCK_NUM; ++i)
    	{
			printk("Socket: %d\n", i);
    		dumpPbuf(i);
    	}
    }else if (argc == 2) {
    	uint8_t sock = atoi(argv[1]);
        printk("Socket: %d\n", sock);
        dumpPbuf(sock);
    }else {
		printk(usage);
	}		
	return CMD_DONE;
}



/**
 *
 */
cmd_state_t
cmd_sendUdpData(int argc, char* argv[], void* ctx)
{
	const char pattern[]={'M', 'I', 'M', 'L', 'F', 'D'};
	const char* pattern2[]={"Prova", "1234567890","FineTest"};
    const char *usage = "usage: sendUdp [sock]\n\t"\
    		"sock: socket Number\n";

    if (argc < 2)
    	printk(usage);

    if (argc >= 2) {

		uint8_t sock = atoi(argv[1]);
		printk("Socket: %d\n", sock);

		if (argc >= 3) {
			uint8_t patternType = atoi(argv[2]);
			printk("PatternType: %d\n", patternType);
			if (patternType == 1)
			{
				insertBuf(sock, (uint8_t*)pattern2[0], strlen(pattern2[0]));
				insertBuf(sock, (uint8_t*)pattern2[1], strlen(pattern2[1]));
				insertBuf(sock, (uint8_t*)pattern2[2], strlen(pattern2[2]));
			}
			if (patternType == 2)
			{
				mergeBuf(sock, NULL, NULL);
			}
		}else{
			if (sock < MAX_SOCK_NUM)
			{
				sendUdpData(getTTCP(sock, TTCP_MODE_TRANSMIT), (uint8_t*)pattern, sizeof(pattern)/sizeof(char));
			}
		}

    }
	return CMD_DONE;
}
