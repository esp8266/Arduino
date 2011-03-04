#ifndef WiFi_h
#define WiFi_h

#include <inttypes.h>

extern "C" {
  #include "utility/wl_types.h"
  #include "utility/wifi_spi.h"
  #include "utility/debug.h"	// only for test, not released
}
  
#include "IPAddress.h"
#include "Client.h"
#include "Server.h"

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

    // Start Wifi connection with latest settings
    int begin();

    // Start Wifi connection with no encryption
    int begin(char* ssid, uint8_t ssid_len);

    // Start Wifi connection with WEP encryption
    int begin(char* ssid, uint8_t ssid_len, uint8_t key_idx, const char* key, const uint8_t key_len);

    // Start Wifi connection with passphrase
    // the most secure supported mode will be automatically selected
    int begin(char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len);

    // Disconnect from the network
    int disconnect(void);

    //Get the interface MAC address.
    uint8_t* macAddress();

    //Get the DHCP information related to IP
    IPAddress localIp();

    //Get the DHCP information related to subnetMask
    IPAddress subnetMask();

    //Get the DHCP information related to gateway IP
    IPAddress gatewayIP();

    // Return the current SSID associated with the network
    char* SSID();

    // Return the current BSSID associated with the network
    uint8_t* BSSID();

    // Start scan SSIDs available and return the number of SSID discovered
    uint8_t scanSSID();

    // Return SSID item available
    char*	SSIDListItem(uint8_t ssidListItem);

    // Return the current Encryption Type associated with the network
    uint8_t	encType(uint8_t ssidListItem);

    // Return the current RSSI /Received Signal Strength in dBm) associated with the network
    int32_t RSSI(uint8_t ssidListItem);

    friend class Client;
    friend class Server;
};

extern WiFiClass WiFi;

#endif
