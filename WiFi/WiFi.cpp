#include "wifi_drv.h"
#include "WiFi.h"
#include "wiring.h"

#define _DEBUG_

extern "C" {
  #include "utility/wl_definitions.h"
  #include "utility/wl_types.h"
	#include "debug.h"
}

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
int16_t 	WiFiClass::_state[MAX_SOCK_NUM] = { 0, 0, 0, 0 };
uint16_t 	WiFiClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };

char    	WiFiClass::_ssid[] = { 0 };
char    	WiFiClass::_key[] = { 0 };
char    	WiFiClass::_passph[] = { 0 };
wl_status_t WiFiClass::_status;


WiFiClass::WiFiClass()
{
}

void WiFiClass::init()
{
    WiFiDrv::wifiDriverInit();
}

uint8_t WiFiClass::getSocket()
{
    for (uint8_t i = 0; i < MAX_SOCK_NUM; ++i)
    {
        if (WiFiClass::_server_port[i] == 0)
        {
             return i;
        }
    }
    return NO_SOCKET_AVAIL;
}

int WiFiClass::begin()
{
	init();
}

int WiFiClass::begin(char* ssid)
{
	uint8_t status = WL_IDLE_STATUS;
	init();

   if (WiFiDrv::wifiSetNetwork(ssid, strlen(ssid)) != WL_FAILURE)
   {

	   do
	   {
		   delay(WL_DELAY_START_CONNECTION);
		   status = WiFiDrv::getConnectionStatus();
	   }
	   while (( status == WL_IDLE_STATUS)||(status == WL_SCAN_COMPLETED));
   }else
   {
	   status = WL_CONNECT_FAILED;
   }
   return status;
}

int WiFiClass::begin(char* ssid, uint8_t key_idx, const char *key)
{
	uint8_t status = WL_IDLE_STATUS;
	init();
    // set encryption key
   if (WiFiDrv::wifiSetKey(ssid, strlen(ssid), key_idx, key, strlen(key)) != WL_FAILURE)
   {
	   do
	   {
		   delay(WL_DELAY_START_CONNECTION);
		   status = WiFiDrv::getConnectionStatus();
	   }
	   while (( status == WL_IDLE_STATUS)||(status == WL_SCAN_COMPLETED));
   }else{
	   status = WL_CONNECT_FAILED;
   }
   return status;
}

int WiFiClass::begin(char* ssid, const char *passphrase)
{
	uint8_t status = WL_IDLE_STATUS;
	init();
    // set passphrase
    if (WiFiDrv::wifiSetPassphrase(ssid, strlen(ssid), passphrase, strlen(passphrase))!= WL_FAILURE)
    {
 	   do
 	   {
 		   delay(WL_DELAY_START_CONNECTION);
 		   status = WiFiDrv::getConnectionStatus();
 	   }
 	   while (( status == WL_IDLE_STATUS)||(status == WL_SCAN_COMPLETED));
    }else{
    	status = WL_CONNECT_FAILED;
    }
    return status;
}

int WiFiClass::disconnect()
{
    return WiFiDrv::disconnect();
}

uint8_t* WiFiClass::macAddress(uint8_t* mac)
{
	uint8_t* _mac = WiFiDrv::getMacAddress();
	memcpy(mac, _mac, WL_MAC_ADDR_LENGTH);
    return mac;
}
   
IPAddress WiFiClass::localIp()
{
	IPAddress ret;
	WiFiDrv::getIpAddress(ret.raw_address());
	return ret;
}

IPAddress WiFiClass::subnetMask()
{
	IPAddress ret;
	WiFiDrv::getSubnetMask(ret.raw_address());
	return ret;
}

IPAddress WiFiClass::gatewayIP()
{
	IPAddress ret;
	WiFiDrv::getGatewayIP(ret.raw_address());
	return ret;
}

char* WiFiClass::SSID()
{
    return WiFiDrv::getCurrentSSID();
}

uint8_t* WiFiClass::BSSID(uint8_t* bssid)
{
	uint8_t* _bssid = WiFiDrv::getCurrentBSSID();
	memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
    return bssid;
}

int32_t WiFiClass::RSSI()
{
    return WiFiDrv::getCurrentRSSI();
}

uint8_t WiFiClass::encryptionType()
{
    return WiFiDrv::getCurrentEncryptionType();
}


uint8_t WiFiClass::scanNetworks()
{
	return WiFiDrv::scanNetworks();
}

char* WiFiClass::SSID(uint8_t networkItem)
{
	return WiFiDrv::getSSIDNetoworks(networkItem);
}

int32_t WiFiClass::RSSI(uint8_t networkItem)
{
	return WiFiDrv::getRSSINetoworks(networkItem);
}

uint8_t WiFiClass::encryptionType(uint8_t networkItem)
{
    return WiFiDrv::getEncTypeNetowrks(networkItem);
}

uint8_t WiFiClass::status()
{
    return WiFiDrv::getConnectionStatus();
}


WiFiClass WiFi;
