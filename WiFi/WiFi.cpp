#include "WProgram.h"
#include "WiFi.h"
#include <EEPROM.h>
#include "HardwareSerial.h"
#include "wifi_drv.h"


// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
int16_t WiFiClass::_state[MAX_SOCK_NUM] = { 0, 0, 0, 0 };
uint16_t WiFiClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };

char    WiFiClass::ssid[WL_SSID_MAX_LENGTH] = { 0 };
uint8_t WiFiClass::ssid_len;
char    WiFiClass::key[13] = { 0 };
uint8_t WiFiClass::key_len;
char    WiFiClass::passph[63] = { 0 };
uint8_t WiFiClass::passph_len;
wl_status_t WiFiClass::status;

void readEEdata(int addr, uint8_t* data, uint8_t len)
{
    for (int i = 0; i < len; ++i)
    {
        data[i]= EEPROM.read(addr);
    }
}

WiFiClass::WiFiClass()
{
}

void WiFiClass::init()
{
    Serial.begin(9600);
    Serial.println("WiFi initializing...");
    WiFiDrv::wifi_drv_init();
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

void WiFiClass::begin()
{
    init();
    Serial.println("WiFi Starting...");
    ssid_len = EEPROM.read(EE_WIFI_SSID_LEN);
    if ((ssid_len == 0)||(ssid_len > WL_SSID_MAX_LENGTH))
    {
        Serial.println("No SSID in EEPROM");

        status = WL_NO_SSID_AVAIL;
        return;
    }
    readEEdata(EE_WIFI_SSID_DATA, (uint8_t*)&ssid[0], ssid_len);

    Serial.print("SSID: ");
    Serial.print(ssid_len, 10);
    Serial.print(" - ");
    Serial.print(ssid[0]); 
    Serial.println("");
    key_len = EEPROM.read(EE_WIFI_KEY_LEN);    
    if (key_len == 0)
    {
        Serial.println("No PASSPHRASE in EEPROM");

        passph_len = EEPROM.read(EE_WIFI_PASSPH_LEN);
        if (passph_len == 0)
        {
            begin(ssid, ssid_len);
        }else{
            readEEdata(EE_WIFI_PASSPH_DATA, (uint8_t*)&passph[0], passph_len);
            beginp(ssid,ssid_len,&passph[0],passph_len);
        }
    }else{
        Serial.println("No KEY in EEPROM");

        readEEdata(EE_WIFI_KEY_DATA, (uint8_t*)&key[0], key_len);
        begink(ssid,ssid_len, 0, &key[0],key_len);
    }
}

void WiFiClass::begin(char* ssid, uint8_t ssid_len)
{
   wl_error_code_t result = (wl_error_code_t)WiFiDrv::wifi_set_net(ssid, ssid_len);
   if (result == WL_SUCCESS)
   {
       status = WL_CONNECTED; 
       Serial.println("WiFi Connected!");
   }else{
       status = WL_CONNECT_FAILED;
       Serial.println("WiFi Connection failed!");
   }
}

void WiFiClass::begink(char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t key_len)
{
    // set encryption key
    wl_error_code_t result = (wl_error_code_t)WiFiDrv::wifi_set_key(ssid, ssid_len, key_idx, key, key_len);
    if (result == WL_SUCCESS)
    {
        //begin(ssid, ssid_len);
    }else{ 
        // Error setting passphrase
    }

}

void WiFiClass::beginp(char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len)
{
    // set passphrase
    wl_error_code_t result = (wl_error_code_t)WiFiDrv::wifi_set_passpharse(ssid, ssid_len, passphrase, len);
    if (result == WL_SUCCESS)
    {
        //begin(ssid, ssid_len);
    }else{ 
        // Error setting passphrase
    }
}

wl_status_t WiFiClass::get_status()
{
    return status;
}

uint8_t WiFiClass::get_result(uint8_t dummy)
{
    uint8_t result = WiFiDrv::wifi_get_result(dummy);
    return result;
}

   
void WiFiClass::getIpAddr(uint8_t *ip, uint8_t *mask, uint8_t *gwip)
{
    WiFiDrv::getIpAddr(ip,mask,gwip);
}


uint8_t WiFiClass::getMacAddr(uint8_t* mac)
{
    return WiFiDrv::wl_get_mac_addr(mac);
}

void WiFiClass::getSSIDList(char** ssid_list, uint8_t* ssidListNum)
{
    WiFiDrv::getSSIDList(ssid_list, ssidListNum);
}

void WiFiClass::getCurrSSID(char* ssid)
{
    WiFiDrv::getCurrSSID(ssid);
}

void WiFiClass::getCurrBSSID(uint8_t* bssid)
{
    WiFiDrv::getCurrBSSID(bssid);
}

void WiFiClass::getCurrRSSI(int32_t* rssi)
{
    WiFiDrv::getCurrRSSI(rssi);
}

void WiFiClass::getCurrEncType(uint8_t* enc_type)
{
    WiFiDrv::getCurrEncType(enc_type);
}

uint8_t WiFiClass::disconnect()
{
    return WiFiDrv::disconnect();
}


WiFiClass WiFi;
