#include "wifi_drv.h"
#include "WiFi.h"
#include "wiring.h"

extern "C" {
  #include "utility/wl_definitions.h"
  #include "utility/wl_types.h"
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
	init();
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
}

int WiFiClass::begin(char* ssid)
{
   if (WiFiDrv::wifiSetNetwork(ssid, strlen(ssid)) != WL_FAILURE)
   {
	   delay(WL_DELAY_START_CONNECTION);
	   return  WiFiDrv::getConnectionStatus();
   }else
   {
	   return WL_CONNECT_FAILED;
   }
}

int WiFiClass::begin(char* ssid, uint8_t key_idx, const char *key)
{
    // set encryption key
   if (WiFiDrv::wifiSetKey(ssid, strlen(ssid), key_idx, key, strlen(key)) != WL_FAILURE)
   {
	   delay(WL_DELAY_START_CONNECTION);
	   return  WiFiDrv::getConnectionStatus();
   }else{
	   return WL_CONNECT_FAILED;
   }
}

int WiFiClass::begin(char* ssid, const char *passphrase)
{
    // set passphrase
    if (WiFiDrv::wifiSetPassphrase(ssid, strlen(ssid), passphrase, strlen(passphrase))!= WL_FAILURE)
    {
    	delay(WL_DELAY_START_CONNECTION);
    	return  WiFiDrv::getConnectionStatus();
    }else{
 	   return WL_CONNECT_FAILED;
    }
}

int WiFiClass::disconnect()
{
    return WiFiDrv::disconnect();
}

uint8_t* WiFiClass::macAddress(uint8_t* mac)
{
	mac = WiFiDrv::getMacAddress();
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
	bssid = WiFiDrv::getCurrentBSSID();
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

WiFiClass WiFi;
