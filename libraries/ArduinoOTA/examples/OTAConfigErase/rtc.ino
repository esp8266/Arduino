#include <flash_utils.h>
#include <eboot_command.h>

#ifdef ERASE_CONFIG_H
void printFlashEraseMap(Print& out) {
  out.printf_P(PSTR(
 "_________________________________________________________________________________________\r\n"
 "|  Address of sectors       |           |           |           |           |           |\r\n"
 "|  at the end of the Flash  | ...FB000  | ...FC000  | ...FD000  | ...FE000  | ...FF000  |\r\n"
 "|___________________________|___________|___________|___________|___________|___________|\r\n"
 "|__Bit_number_for_Mask______|_____4_____|_____3_____|_____2_____|_____1_____|_____0_____|\r\n"
 "|                           |           |  RF_CAL   |        SDK Parameter Area         |\r\n"
 "|  Overlay at RF init       |           | PHY INIT  |           |           |           |\r\n"
 "|  Persistant data          |           |           |           |  SSID/PW  |           |\r\n"
 "|  User storage             |   EEPROM  |           |           |           |           |\r\n"
 "|  Often shown downloaded   |           | BLANK.BIN |           | BLANK.BIN |           |\r\n"
 "|___________________________|___________|___________|___________|___________|___________|\r\n"
 "| 0 ERASE_CONFIG_NONE       |           |           |           |           |           |\r\n"
 "| 1 ERASE_CONFIG_EEPROM     |   BIT(4)  |           |           |           |           |\r\n"
 "| 2 ERASE_CONFIG_RF_CAL     |           |  BIT(3)   |           |           |           |\r\n"
 "| 3 ERASE_CONFIG_PERSISTANT |           |           |           |   BIT(1)  |           |\r\n"
 "| 4 ERASE_CONFIG_BLANK_BIN  |           |  BIT(3)   |           |   BIT(1)  |           |\r\n"
 "| 5 ERASE_CONFIG_SDK_DATA   |           |  BIT(3)   |   BIT(2)  |   BIT(1)  |   BIT(0)  |\r\n"
 "| 6 ERASE_CONFIG_ALL_DATA   |   BIT(4)  |  BIT(3)   |   BIT(2)  |   BIT(1)  |   BIT(0)  |\r\n"
 "|___________________________|___________|___________|___________|___________|___________|\r\n"
));
}

bool queueEraseConfig(int hotKey) {
  switch (hotKey) {
    case '0':
      requestEraseConfig(ERASE_CONFIG_NONE);
      break;
    case '1':
      requestEraseConfig(ERASE_CONFIG_EEPROM);
      break;
    case '2':
      requestEraseConfig(ERASE_CONFIG_RF_CAL);
      break;
    case '3':
      requestEraseConfig(ERASE_CONFIG_PERSISTANT);
      break;
    case '4':
      requestEraseConfig(ERASE_CONFIG_BLANK_BIN);
      break;
    case '5':
      requestEraseConfig(ERASE_CONFIG_SDK_DATA);
      break;
    case '6':
      requestEraseConfig(ERASE_CONFIG_ALL_DATA);
      break;
    default:
      return false;
  }
  return true;
}

void requestEraseConfig(uint32_t mask) {
  eraseConfigOption = (ERASE_CONFIG_MASK_t)mask; // Save in case they do an OTA instead of a restart.
  
  eboot_command volatile * ebcmd = (eboot_command volatile *)RTC_MEM;

  for (size_t i = 0; i < sizeof(eboot_command) / sizeof(uint32_t) ; i++)
    RTC_MEM[i] = 0U;

  // Finish fake post OTA flash copy complete state
  ebcmd->args[4] =  mask;
  ebcmd->args[5] = ~mask;
  ebcmd->args[6] =  mask;
  ebcmd->args[7] = ~mask;
  ebcmd->action = ACTION_COPY_RAW;
}

#endif
