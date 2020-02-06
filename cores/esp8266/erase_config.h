#ifndef ERASE_CONFIG_H
#define ERASE_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif

typedef enum ERASE_CONFIG {
// Note, The Arduino ESP8266 flash memory map differs from Espressif's.
// Espressive has separate sectors for PHY init data and RF_CAL. In Arduino
// ESP8266 RF_CAL and PHY INIT share the same sector. Between two RF init user
// calls the PHY init data is overlayed. Thus avoiding the waste of a 4K sector
// to store only 128 bytes.
//
// Mapping of Sectors at end of Flash, adapted for the Arduino ESP8266.
// The IDE configured flash size defines where the actual last sector used by
// the SDK is located.
//_______________________________________________________________________________________
//_Bit_number_for_Mask______|_____4_____|_____3_____|_____2_____|_____1_____|_____0_____|
//                          |           |  RF_CAL   |        SDK Parameter Area         |
// Overlay at RF init       |           | PHY INIT  |           |           |           |
// Persistant data          |           |           |           |  SSID/PW  |           |
// User storage             |   EEPROM  |           |           |           |           |
// Often shown downloaded   |           | BLANK.BIN |           | BLANK.BIN |           |
//__________________________|___________|___________|___________|___________|___________|
ERASE_CONFIG_NONE = 0,    //|           |           |           |           |           |
ERASE_CONFIG_EEPROM       = (   BIT(4)                                                  ),
ERASE_CONFIG_RF_CAL       = (              BIT(3)                                       ),
ERASE_CONFIG_PERSISTANT   = (                                       BIT(1)              ),
ERASE_CONFIG_BLANK_BIN    = (              BIT(3)   |               BIT(1)              ),
ERASE_CONFIG_SDK_DATA     = (              BIT(3)   |   BIT(2)  |   BIT(1)  |   BIT(0)  ),
ERASE_CONFIG_ALL_DATA     = (   BIT(4)  |  BIT(3)   |   BIT(2)  |   BIT(1)  |   BIT(0)  )
//__________________________|___________|___________|___________|___________|___________|
} ERASE_CONFIG_MASK_t; // Use one of these with eraseConfig

bool check_and_erase_config(void);
bool erase_config(const uint32_t flash_erase_mask);
void enable_erase_config_at_link_time(void);

#ifdef __cplusplus
};
#endif

#endif
