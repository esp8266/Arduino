#ifndef WiFi_h
#define WiFi_h

#include <inttypes.h>

extern "C" {
	#include "utility/wl_definitions.h"
	#include "utility/wl_types.h"
}

#include "IPAddress.h"
#include "WiFiClient.h"
#include "WiFiServer.h"

class WiFiClass
{
private:
    // this data are stored in EEPROM and loaded at begin
    // The next connect overwrite these values
    static  char		_ssid[WL_SSID_MAX_LENGTH];
    static  char 		_key[WL_WEP_KEY_MAX_LENGTH];
    static  char 		_passph[WL_WPA_KEY_MAX_LENGTH];
    static  wl_status_t _status;

    static void init();
public:
    static int16_t 	_state[MAX_SOCK_NUM];
    static uint16_t _server_port[MAX_SOCK_NUM];

    WiFiClass();

    // Get the first socket available
    static uint8_t getSocket();

    /* Start Wifi connection for OPEN networks
     *
     * param ssid: Pointer to the SSID string.
     */
    int begin(char* ssid);

    /* Start Wifi connection with WEP encryption.
     * Configure a key into the device. The key type (WEP-40, WEP-104)
     * is determined by the size of the key (5 bytes for WEP-40, 13 bytes for WEP-104).
     *
     * param ssid: Pointer to the SSID string.
     * param key_idx: The key index to set. Valid values are 0-3.
     * param key: Key input buffer.
     */
    int begin(char* ssid, uint8_t key_idx, const char* key);

    /* Start Wifi connection with passphrase
     * the most secure supported mode will be automatically selected
     *
     * param passphrase: Passphrase. Valid characters in a passphrase
     *        must be between ASCII 32-126 (decimal).
     */
    int begin(char* ssid, const char *passphrase);

    // Disconnect from the network
    int disconnect(void);

    //Get the interface MAC address.
    uint8_t* macAddress(uint8_t* mac);

    //Get the DHCP information related to IP
    IPAddress localIP();

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

    /* Return Connection status.
     *
     * return: one of the value defined in wl_status_t
     */
    uint8_t status();

    friend class WiFiClient;
    friend class WiFiServer;
};

extern WiFiClass WiFi;

#endif
