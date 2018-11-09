
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



} // extern "C"
