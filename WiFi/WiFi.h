#ifndef WiFi_h
#define WiFi_h

#include <inttypes.h>

extern "C" {
  #include "utility/wl_definitions.h"
}
  
#include "IPAddress.h"
#include "Client.h"
#include "Server.h"

class WiFiClass
{
private:
    // this data are stored in EEPROM and loaded at begin
    // The next connect overwrite these values
    static  char		_ssid[WL_SSID_MAX_LENGTH];
    static  char 		_key[WL_WEP_KEY_MAX_LENGTH];
    static  char 		_passph[WL_WPA_KEY_MAX_LENGTH];
    static  wl_status_t _status;

    void init();
public:
    static int16_t 	_state[MAX_SOCK_NUM];
    static uint16_t _server_port[MAX_SOCK_NUM];

    WiFiClass();

    // Get thefirst socket available
    static uint8_t getSocket();

    // Start Wifi connection with latest settings
    int begin();

    // Start Wifi connection with no encryption
    int begin(char* ssid);

    // Start Wifi connection with WEP encryption
    int begin(char* ssid, uint8_t key_idx, const char* key);

    // Start Wifi connection with passphrase
    // the most secure supported mode will be automatically selected
    int begin(char* ssid, const char *passphrase);

    // Disconnect from the network
    int disconnect(void);

    //Get the interface MAC address.
    uint8_t* macAddress(uint8_t* mac);

    //Get the DHCP information related to IP
    IPAddress localIp();

    //Get the DHCP information related to subnetMask
    IPAddress subnetMask();

    //Get the DHCP information related to gateway IP
    IPAddress gatewayIP();

    // Return the current SSID associated with the network
    char* SSID();

    // Return the current BSSID associated with the network
    uint8_t* BSSID(uint8_t* bssid);

    // Return the current RSSI /Received Signal Strength in dBm) associated with the network
    int32_t RSSI();

    // Return the Encryption Type associated with the network
    uint8_t	encryptionType();

    // Start scan WiFi networks available and return the discovered number
    uint8_t scanNetworks();

    // Return SSID item associated with the network identified with networkItem
    char*	SSID(uint8_t networkItem);

    // Return the Encryption Type associated with the network identified with networkItem
    uint8_t	encryptionType(uint8_t networkItem);

    // Return the current RSSI /Received Signal Strength in dBm) associated with the network identified with networkItem
    int32_t RSSI(uint8_t networkItem);

    // Return Connection status
    uint8_t status();

    friend class Client;
    friend class Server;
};

extern WiFiClass WiFi;

#endif
