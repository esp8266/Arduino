/*
    Arduino emulation - espressif sdk host implementation
    Copyright (c) 2018 david gauchard. All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal with the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimers.

    - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimers in the
    documentation and/or other materials provided with the distribution.

    - The names of its contributors may not be used to endorse or promote
    products derived from this Software without specific prior written
    permission.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
    THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS WITH THE SOFTWARE.
*/

#include <lwip/def.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


extern "C"
{

#include <user_interface.h>
#include <lwip/netif.h>

    uint8 wifi_get_opmode(void)
    {
        return STATION_MODE;
    }

    phy_mode_t wifi_get_phy_mode(void)
    {
        return PHY_MODE_11N;
    }

    uint8 wifi_get_channel(void)
    {
        return 1;
    }

    uint8 wifi_station_get_current_ap_id(void)
    {
        return 0;
    }

    station_status_t wifi_station_get_connect_status(void)
    {
        return STATION_GOT_IP;
    }

    uint8 wifi_station_get_auto_connect(void)
    {
        return 1;
    }

    bool wifi_station_get_config(struct station_config *config)
    {
        strcpy((char*)config->ssid, "emulated-ssid");
        strcpy((char*)config->password, "emulated-ssid-password");
        config->bssid_set = 0;
        for (int i = 0; i < 6; i++)
            config->bssid[i] = i;
        config->threshold.rssi = 1;
        config->threshold.authmode = AUTH_WPA_PSK;
#ifdef NONOSDK3V0
        config->open_and_wep_mode_disable = true;
#endif
        return true;
    }

    void wifi_fpm_close(void)
    {
    }

    sint8 wifi_fpm_do_sleep(uint32 sleep_time_in_us)
    {
        usleep(sleep_time_in_us);
        return 1;
    }

    void wifi_fpm_do_wakeup(void)
    {
    }

    void wifi_fpm_open(void)
    {
    }

    void wifi_fpm_set_sleep_type(sleep_type_t type)
    {
        (void)type;
    }

    uint32_t global_ipv4_netfmt = 0; // global binding

    netif netif0;
    uint32_t global_source_address = INADDR_ANY;

    bool wifi_get_ip_info(uint8 if_index, struct ip_info *info)
    {
        // emulate wifi_get_ip_info()
        // ignore if_index
        // use global option -i (host_interface) to select bound interface/address

        struct ifaddrs * ifAddrStruct = NULL, * ifa = NULL;
        uint32_t ipv4 = lwip_htonl(0x7f000001);
        uint32_t mask = lwip_htonl(0xff000000);
        global_source_address = INADDR_ANY; // =0

        if (getifaddrs(&ifAddrStruct) != 0)
        {
            perror("getifaddrs");
            exit(EXIT_FAILURE);
        }

        if (host_interface)
            mockverbose("host: looking for interface '%s':\n", host_interface);
        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
        {
            mockverbose("host: interface: %s", ifa->ifa_name);
            if (ifa->ifa_addr
                    && ifa->ifa_addr->sa_family == AF_INET // ip_info is IPv4 only
               )
            {
                auto test_ipv4 = lwip_ntohl(*(uint32_t*) & ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr);
                mockverbose(" IPV4 (0x%08lx)", test_ipv4);
                if ((test_ipv4 & 0xff000000) == 0x7f000000)
                    // 127./8
                    mockverbose(" (local, ignored)");
                else
                {
                    if (!host_interface || (host_interface && strcmp(ifa->ifa_name, host_interface) == 0))
                    {
                        // use the first non-local interface, or, if specified, the one selected by user on cmdline
                        ipv4 = *(uint32_t*) & ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
                        mask = *(uint32_t*) & ((struct sockaddr_in*)ifa->ifa_netmask)->sin_addr;
                        mockverbose(" (selected)\n");
                        if (host_interface)
                            global_source_address = ntohl(ipv4);
                        break;
                    }
                }
            }
            mockverbose("\n");
        }

        if (ifAddrStruct != NULL)
            freeifaddrs(ifAddrStruct);

        (void)if_index;
        //if (if_index != STATION_IF)
        //	fprintf(stderr, "we are not AP");

        if (global_ipv4_netfmt == NO_GLOBAL_BINDING)
            global_ipv4_netfmt = ipv4;

        if (info)
        {
            info->ip.addr = ipv4;
            info->netmask.addr = mask;
            info->gw.addr = ipv4;

            netif0.ip_addr.addr = ipv4;
            netif0.netmask.addr = mask;
            netif0.gw.addr = ipv4;
            netif0.flags = NETIF_FLAG_IGMP | NETIF_FLAG_UP | NETIF_FLAG_LINK_UP;
            netif0.next = nullptr;
        }

        return true;
    }

    uint8 wifi_get_listen_interval(void)
    {
        return 1;
    }

    bool wifi_get_macaddr(uint8 if_index, uint8 *macaddr)
    {
        (void)if_index;
        macaddr[0] = 0xde;
        macaddr[1] = 0xba;
        macaddr[2] = 0x7a;
        macaddr[3] = 0xb1;
        macaddr[4] = 0xe0;
        macaddr[5] = 0x42;
        return true;
    }

    uint8 wifi_get_opmode_default(void)
    {
        return STATION_MODE;
    }

#ifdef NONOSDK3V0

    sleep_level_t wifi_get_sleep_level(void)
    {
        return MIN_SLEEP_T;
    }

#endif // nonos-sdk-pre-3

    sleep_type_t wifi_get_sleep_type(void)
    {
        return NONE_SLEEP_T;
    }

    bool wifi_set_channel(uint8 channel)
    {
        (void)channel;
        return true;
    }

    wifi_event_handler_cb_t wifi_event_handler_cb_emu = nullptr;
    void wifi_set_event_handler_cb(wifi_event_handler_cb_t cb)
    {
        wifi_event_handler_cb_emu = cb;
        mockverbose("TODO: wifi_set_event_handler_cb set\n");
    }

    bool wifi_set_ip_info(uint8 if_index, struct ip_info *info)
    {
        (void)if_index;
        (void)info;
        return false;
    }

    bool wifi_set_listen_interval(uint8 interval)
    {
        (void)interval;
        return true;
    }

    bool wifi_set_opmode(uint8 opmode)
    {
        return opmode == STATION_MODE || opmode == STATIONAP_MODE;
    }

    bool wifi_set_opmode_current(uint8 opmode)
    {
        return opmode == STATION_MODE || opmode == STATIONAP_MODE;
    }

    bool wifi_set_phy_mode(phy_mode_t mode)
    {
        (void)mode;
        return true;
    }

#ifdef NONOSDK3V0

    bool wifi_set_sleep_level(sleep_level_t level)
    {
        (void)level;
        return true;
    }

#endif

    bool wifi_set_sleep_type(sleep_type_t type)
    {
        (void)type;
        return true;
    }

    bool wifi_station_connect(void)
    {
        return true;
    }

    bool wifi_station_dhcpc_start(void)
    {
        return true;
    }

    bool wifi_station_dhcpc_stop(void)
    {
        return true;
    }

    bool wifi_station_disconnect(void)
    {
        return true;
    }

    bool wifi_station_get_config_default(struct station_config *config)
    {
        return wifi_station_get_config(config);
    }

    char wifi_station_get_hostname_str [128];
    const char* wifi_station_get_hostname(void)
    {
        return strcpy(wifi_station_get_hostname_str, "esposix");
    }

    bool wifi_station_get_reconnect_policy()
    {
        return true;
    }

    sint8 wifi_station_get_rssi(void)
    {
        return 5;
    }

    bool wifi_station_set_auto_connect(uint8 set)
    {
        return set != 0;
    }

    bool wifi_station_set_config(struct station_config *config)
    {
        (void)config;
        return true;
    }

    bool wifi_station_set_config_current(struct station_config *config)
    {
        (void)config;
        return true;
    }

    bool wifi_station_set_hostname(const char *name)
    {
        (void)name;
        return true;
    }

    bool wifi_station_set_reconnect_policy(bool set)
    {
        (void)set;
        return true;
    }

    void system_phy_set_max_tpw(uint8 max_tpw)
    {
        (void)max_tpw;
    }

    bool wifi_softap_dhcps_start(void)
    {
        return true;
    }

    enum dhcp_status wifi_softap_dhcps_status(void)
    {
        return DHCP_STARTED;
    }

    bool wifi_softap_dhcps_stop(void)
    {
        return true;
    }

    bool wifi_softap_get_config(struct softap_config *config)
    {
        strcpy((char*)config->ssid, "apssid");
        strcpy((char*)config->password, "appasswd");
        config->ssid_len = strlen("appasswd");
        config->channel = 1;
        config->authmode = AUTH_WPA2_PSK;
        config->ssid_hidden = 0;
        config->max_connection = 4;
        config->beacon_interval = 100;
        return true;
    }

    bool wifi_softap_get_config_default(struct softap_config *config)
    {
        return wifi_softap_get_config(config);
    }

    uint8 wifi_softap_get_station_num(void)
    {
        return 2;
    }

    bool wifi_softap_set_config(struct softap_config *config)
    {
        (void)config;
        return true;
    }

    bool wifi_softap_set_config_current(struct softap_config *config)
    {
        (void)config;
        return true;
    }

    bool wifi_softap_set_dhcps_lease(struct dhcps_lease *please)
    {
        (void)please;
        return true;
    }

    bool wifi_softap_set_dhcps_lease_time(uint32 minute)
    {
        (void)minute;
        return true;
    }

    bool wifi_softap_set_dhcps_offer_option(uint8 level, void* optarg)
    {
        (void)level;
        (void)optarg;
        return true;
    }

    bool wifi_station_scan(struct scan_config *config, scan_done_cb_t cb)
    {
        (void)config;
        cb(nullptr, FAIL);
        return false;
    }

    uint32_t core_version = 1;

    ///////////////////////////////////////
    // not user_interface

    void ets_isr_mask(int intr)
    {
        (void)intr;
    }

    void ets_isr_unmask(int intr)
    {
        (void)intr;
    }

    void esp_schedule(void)
    {
    }

    void dns_setserver(u8_t numdns, ip_addr_t *dnsserver)
    {
        (void)numdns;
        (void)dnsserver;
    }

    ip_addr_t dns_getserver(u8_t numdns)
    {
        (void)numdns;
        ip_addr_t addr = { 0x7f000001 };
        return addr;
    }


#include <smartconfig.h>
    bool smartconfig_start(sc_callback_t cb, ...)
    {
        //XXXFIXME ... -> ptr
        cb(SC_STATUS_LINK, NULL);
        return true;
    }

    bool smartconfig_stop(void)
    {
        return true;
    }

    sleep_type_t wifi_fpm_get_sleep_type(void)
    {
        return NONE_SLEEP_T;
    }

} // extern "C"
