#ifndef WiFi_h
#define WiFi_h

#include <inttypes.h>

extern "C" {
  #include "utility/wl_types.h"
  #include "utility/wifi_spi.h"
  #include "utility/debug.h"	// only for test, not released
}
  
#include "Client.h"
#include "Server.h"

// location in EEPROM to store key information
#define EE_WIFI_DATA_ADDR   0x10
#define EE_WIFI_SSID_LEN    EE_WIFI_DATA_ADDR
#define EE_WIFI_ENC_TYPE	EE_WIFI_SSID_LEN + 1
#define EE_WIFI_KEY_LEN     EE_WIFI_ENC_TYPE + 1
#define EE_WIFI_SSID_DATA   EE_WIFI_KEY_LEN  + 1
#define EE_WIFI_KEY_DATA    EE_WIFI_SSID_DATA + WL_SSID_MAX_LENGTH
#define EE_WIFI_END_DATA    EE_WIFI_KEY_DATA  + WL_KEY_MAX_LENGTH

// Wifi Encryption selection type
#define WIFI_ENC_NONE	0
#define WIFI_ENC_WEP	1
#define WIFI_ENC_WPA	2


class WiFiClass
{
private:
    // this data are stored in EEPROM and loaded at begin
    // The next connect overwrite these values
    static  char _ssid[WL_SSID_MAX_LENGTH];
    static  uint8_t _ssid_len;
    static  char _key[WL_WEP_KEY_MAX_LENGTH];
    static  uint8_t _key_len;
    static  char _passph[WL_WPA_KEY_MAX_LENGTH];
    static  uint8_t _passph_len;
    static  wl_status_t _status;

    void init();
public:
    static int16_t _state[MAX_SOCK_NUM];
    static uint16_t _server_port[MAX_SOCK_NUM];

    WiFiClass();

    // Start Wifi connection
    void begin();

    // Set SSID name
    void setSSID(char* ssid);

    // Set the encryption type: WPA, WEP or NONE
    void setEncryption(uint8_t encType);

    // Set the key used for the encryption
    void setPassword(const char *key, const uint8_t keyLen, uint8_t keyIndex);

    //getSock get the first socket available
    static uint8_t getSocket();

    // verify the completion of the scan command
    wl_status_t getScanCmdStatus();

    // get the result of the last operation 
    uint8_t getResult(uint8_t dummy = 0);

    // Disconnect from the network
    uint8_t disconnect(void);

    //Get the interface MAC address.
    uint8_t* getMacAddr();

    //Get the DHCP information related to IP
    uint8_t* getIpAddr();

    //Get the DHCP information related to IP
    uint8_t* getNetMask();

    //Get the DHCP information related to IP
    uint8_t* getGatewayIp();

    // Return SSID item available
    char*	getSSIDListItem(uint8_t ssidListItem);

    // Return the number of SSID discovered
    uint8_t getSSIDListNum();

    // Return the current SSID associated with the network
    char* getSSID();

    // Return the current BSSID associated with the network
    uint8_t* getBSSID();

    // Return the current RSSI associated with the network
    int32_t getRSSI();

    // Return the current Encryption Type associated with the network
    uint8_t	getCurrEncType();

    friend class Client;
    friend class Server;
};

extern WiFiClass WiFi;

#endif
