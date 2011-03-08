#ifndef WiFi_Drv_h
#define WiFi_Drv_h

#include <inttypes.h>
#include "wifi_spi.h"

#define KEY_IDX_LEN     1
#define WL_DELAY_START_CONNECTION 5000

class WiFiDrv
{
private:
	// settings of requested network
	static char 	_networkSsid[WL_SSID_MAX_LENGTH];

	// settings of current selected network
	static char 	_ssid[WL_SSID_MAX_LENGTH];
	static uint8_t 	_bssid[WL_MAC_ADDR_LENGTH];
	static uint8_t 	_mac[WL_MAC_ADDR_LENGTH];
	static uint8_t  _localIp[WL_IPV4_LENGTH];
	static uint8_t  _subnetMask[WL_IPV4_LENGTH];
	static uint8_t  _gatewayIp[WL_IPV4_LENGTH];

    static void getNetworkData(uint8_t *ip, uint8_t *mask, uint8_t *gwip);

public:

    static void wifiDriverInit();

    static uint8_t wifiSetNetwork(char* ssid, uint8_t ssid_len);

    static uint8_t wifiSetPassphrase(char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len);

    static uint8_t wifiSetKey(char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t len);

    static uint8_t disconnect();
    
    static uint8_t getConnectionStatus();

    static uint8_t* getMacAddress();

    static void getIpAddress(uint8_t *ip);

    static void getSubnetMask(uint8_t *ip);

    static void getGatewayIP(uint8_t *ip);

    static char* getCurrentSSID();

    static uint8_t* getCurrentBSSID();

    static int32_t getCurrentRSSI();

    static uint8_t getCurrentEncryptionType();

    static uint8_t scanNetworks();

    static char* getSSIDNetoworks(uint8_t networkItem);

    static int32_t getRSSINetoworks(uint8_t networkItem);

    static uint8_t getEncTypeNetowrks(uint8_t networkItem);

};

extern WiFiDrv wiFiDrv;

#endif
