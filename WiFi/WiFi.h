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
#define EE_WIFI_KEY_LEN     EE_WIFI_DATA_ADDR + 1
#define EE_WIFI_PASSPH_LEN  EE_WIFI_DATA_ADDR + 2
#define EE_WIFI_SSID_DATA   EE_WIFI_DATA_ADDR + 3
#define EE_WIFI_KEY_DATA    EE_WIFI_DATA_ADDR + 4
#define EE_WIFI_PASSPH_DATA EE_WIFI_DATA_ADDR + 4
// Note: Or KEY or PASSPH can be defined. 
// The selection is made by the len not equal to zero

class WiFiClass
{
private:
    // this data are stored in EEPROM and loaded at begin
    // The next connect overwrite these values
    static  char ssid[WL_SSID_MAX_LENGTH];
    static  uint8_t ssid_len;
    static  char key[13];
    static  uint8_t key_len;
    static  char passph[63];
    static  uint8_t passph_len;
    static  wl_status_t status;

    void init();
public:
    //static wl_state_t _wl_state[MAX_SOCK_NUM];
    static int16_t _state[MAX_SOCK_NUM];
    static uint16_t _server_port[MAX_SOCK_NUM];
    WiFiClass();

    // Start Wifi connection with data stored in EEPROM
    void begin();

    // Start Wifi connection without WEP or WPA
    void begin(char* ssid, uint8_t ssid_len);

    // Start Wifi connection with WEP
    void begink(char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t key_len);

    // Start Wifi connection with WPA
    void beginp(char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len);

    //getSock get the first socket available
    static uint8_t getSocket();

    // verify the completion of the scan command
    wl_status_t get_status();

    // get the result of the last operation 
    uint8_t get_result(uint8_t dummy = 0);

    // Disconnect from the network
    uint8_t disconnect(void);

    //Get the interface MAC address.
    uint8_t getMacAddr(uint8_t* mac);

    //Get the DHCP infortion related to IP, netmas, gateway
    void getIpAddr(uint8_t *ip, uint8_t *mask, uint8_t *gwip); 

    // Get the list of currently known networks.
    wl_error_code_t wl_get_network_list(struct wl_network_t** network_list, uint8_t* network_cnt);

    // Return a list of all SSID available
    void getSSIDList(char** ssid_list, uint8_t* ssidListNum);

    // Return the current SSID associated with the network
    void getCurrSSID(char* ssid);

    // Return the current BSSID associated with the network
    void getCurrBSSID(uint8_t* bssid);

    // Return the current RSSI associated with the network
    void getCurrRSSI(int32_t* rssi);

    // Return the current Encryption Type associated with the network
    void getCurrEncType(uint8_t* enc_type);

    friend class Client;
    friend class Server;
};

extern WiFiClass WiFi;

#endif
