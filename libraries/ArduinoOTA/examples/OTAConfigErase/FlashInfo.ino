#include <Esp.h>
#include <Print.h>
#define String_F(a) String(F(a))

void printFlashInfo(Print& oStream) {
  oStream.println(String_F("Flash Info/Size as reported by:"));
  oStream.println(String_F("  ESP.getFlashChipId:           0x0") + String(ESP.getFlashChipId(), HEX)           + (", ") + String(ESP.getFlashChipId()));
  oStream.println(String_F("  ESP.getFlashChipSizeByChipId: 0x0") + String(ESP.getFlashChipSizeByChipId(), HEX) + (", ") + String(ESP.getFlashChipSizeByChipId()));
  oStream.println(String_F("  ESP.getFlashChipRealSize:     0x0") + String(ESP.getFlashChipRealSize(), HEX)     + (", ") + String(ESP.getFlashChipRealSize()));
  oStream.println(String_F("  ESP.getFlashChipSize:         0x0") + String(ESP.getFlashChipSize(), HEX)         + (", ") + String(ESP.getFlashChipSize()));
  oStream.println(String_F("  flashchip->deviceId:          0x0") + String(flashchip->deviceId, HEX)            + (", ") + String(flashchip->deviceId));
  oStream.println(String_F("  flashchip->chip_size:         0x0") + String(flashchip->chip_size, HEX)           + (", ") + String(flashchip->chip_size));
  oStream.println(String_F("  flashchip->block_size:        0x0") + String(flashchip->block_size, HEX)          + (", ") + String(flashchip->block_size));
  oStream.println(String_F("  flashchip->sector_size:       0x0") + String(flashchip->sector_size, HEX)         + (", ") + String(flashchip->sector_size));
  oStream.println(String_F("  flashchip->page_size:         0x0") + String(flashchip->page_size, HEX)           + (", ") + String(flashchip->page_size));
  oStream.println(String_F("  flashchip->status_mask:       0x0") + String(flashchip->status_mask, HEX)         + (", ") + String(flashchip->status_mask));
}
