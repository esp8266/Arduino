#include <Esp.h>
#include <Print.h>
#define String_F(a) String(F(a))

void printFlashInfo(Print& oStream) {
  oStream.println(String_F("Flash Size as reported by:"));
  oStream.println(String_F("  flashchip->chip_size:     0x0") + String(flashchip->chip_size, HEX)       + (", ") + String(flashchip->chip_size));
  oStream.println(String_F("  ESP.getFlashChipSize:     0x0") + String(ESP.getFlashChipSize(), HEX)     + (", ") + String(ESP.getFlashChipSize()));
  oStream.println(String_F("  ESP.getFlashChipRealSize: 0x0") + String(ESP.getFlashChipRealSize(), HEX) + (", ") + String(ESP.getFlashChipRealSize()));
}
