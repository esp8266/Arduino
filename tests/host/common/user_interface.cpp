
#include <lwip/def.h>

extern "C"
{

#include <user_interface.h>

uint8 wifi_get_opmode(void)
{
	return STATION_MODE;
}

phy_mode_t wifi_get_phy_mode(void)
{
	return PHY_MODE_11N;
}

uint8 wifi_get_channel (void)
{
	return 1;
}

uint8 wifi_station_get_current_ap_id (void)
{
	return 0;
}

station_status_t wifi_station_get_connect_status (void)
{
	return STATION_GOT_IP;
}

uint8 wifi_station_get_auto_connect (void)
{
	return 1;
}

bool wifi_station_get_config (struct station_config *config)
{
	strcpy((char*)config->ssid, "emulated-ssid");
	strcpy((char*)config->password, "emulated-ssid-password");
	config->bssid_set = 0;
	for (int i = 0; i < 6; i++)
		config->bssid[i] = i;
	config->threshold.rssi = 1;
	config->threshold.authmode = AUTH_WPA_PSK;
	config->open_and_wep_mode_disable = true;
	return true;
}

void wifi_fpm_close(void)
{
}

sint8 wifi_fpm_do_sleep (uint32 sleep_time_in_us)
{
	usleep(sleep_time_in_us);
	return 1;
}

void wifi_fpm_do_wakeup (void)
{
}

void wifi_fpm_open (void)
{
}

void wifi_fpm_set_sleep_type (sleep_type_t type)
{
	(void)type;
}

bool wifi_get_ip_info (uint8 if_index, struct ip_info *info)
{
	//XXXTODO
	if (if_index != STATION_IF)
		fprintf(stderr, "we are not AP");
	
	//XXXTODO (give ip address of default route's interface?)
	info->ip.addr = lwip_htonl(0x7f000001);
	info->netmask.addr = lwip_htonl(0xff000000);
	info->gw.addr = lwip_htonl(0x7f000001);

	return true;
}

uint8 wifi_get_listen_interval (void)
{
	return 1;
}

bool wifi_get_macaddr(uint8 if_index, uint8 *macaddr)
{
	macaddr[0] = 0xde;
	macaddr[1] = 0xba;
	macaddr[2] = 0x7a;
	macaddr[3] = 0xb1;
	macaddr[4] = 0xe0;
	macaddr[5] = 0x42;
	return true;
}

uint8 wifi_get_opmode_default (void)
{
	return STATION_MODE;
}

sleep_level_t wifi_get_sleep_level (void)
{
	return MIN_SLEEP_T;
}

sleep_type_t wifi_get_sleep_type (void)
{
	return NONE_SLEEP_T;
}

bool wifi_set_channel (uint8 channel)
{
	(void)channel;
	return true;
}

wifi_event_handler_cb_t wifi_event_handler_cb_emu = nullptr;
void wifi_set_event_handler_cb (wifi_event_handler_cb_t cb)
{
	wifi_event_handler_cb_emu = cb;
	fprintf(stderr, MOCK "TODO: wifi_set_event_handler_cb set\n");
}

bool wifi_set_ip_info (uint8 if_index, struct ip_info *info)
{
	(void)if_index;
	(void)info;
	return false;
}

bool wifi_set_listen_interval (uint8 interval)
{
	(void)interval;
	return true;
}

bool wifi_set_opmode (uint8 opmode)
{
	return opmode == STATION_MODE || opmode == STATIONAP_MODE;
}

bool wifi_set_opmode_current (uint8 opmode)
{
	return opmode == STATION_MODE || opmode == STATIONAP_MODE;
}

bool wifi_set_phy_mode (phy_mode_t mode)
{
	return true;
}

bool wifi_set_sleep_level (sleep_level_t level)
{
	(void)level;
	return true;
}

bool wifi_set_sleep_type (sleep_type_t type)
{
	(void)type;
	return true;
}

bool wifi_station_connect (void)
{
	return true;
}

bool wifi_station_dhcpc_start (void)
{
	return true;
}

bool wifi_station_dhcpc_stop (void)
{
	return true;
}

bool wifi_station_disconnect (void)
{
	return true;
}

bool wifi_station_get_config_default (struct station_config *config)
{
	return wifi_station_get_config(config);
}

char wifi_station_get_hostname_str [128];
char* wifi_station_get_hostname (void)
{
	return strcpy(wifi_station_get_hostname_str, "esposix");
}

bool wifi_station_get_reconnect_policy ()
{
	return true;
}

sint8 wifi_station_get_rssi (void)
{
	return 5;
}

bool wifi_station_set_auto_connect (uint8 set)
{
	return set != 0;
}

bool wifi_station_set_config (struct station_config *config)
{
	(void)config;
	return true;
}

bool wifi_station_set_config_current (struct station_config *config)
{
	(void)config;
	return true;
}

bool wifi_station_set_hostname (char *name)
{
	(void)name;
	return true;
}

bool wifi_station_set_reconnect_policy (bool set)
{
	(void)set;
	return true;
}

void system_phy_set_max_tpw (uint8 max_tpw)
{
	(void)max_tpw;
}

///////////////////////////////////////
// not user_interface

void ets_isr_mask (int intr)
{
	(void)intr;
}

void ets_isr_unmask (int intr)
{
	(void)intr;
}

void esp_schedule (void)
{
}

void optimistic_yield (uint32_t ms)
{
	usleep(ms * 1000);
}

void dns_setserver (u8_t numdns, ip_addr_t *dnsserver)
{
	(void)numdns;
	(void)dnsserver;
}

ip_addr_t dns_getserver (u8_t numdns)
{
	ip_addr_t addr = { 0x7f000001 };
	return addr;
}


#include <smartconfig.h>
bool smartconfig_start (sc_callback_t cb, ...)
{
	//XXXFIXME ... -> ptr
	cb(SC_STATUS_LINK, NULL);
	return true;
}

bool smartconfig_stop (void)
{
	return true;
}




} // extern "C"
