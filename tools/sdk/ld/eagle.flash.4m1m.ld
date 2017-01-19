/* Flash Split for 4M chips */
/* sketch 1019KB */
/* empty  2048KB */
/* spiffs 1004KB */
/* eeprom 20KB */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0xfeff0
}

_BOOTLOADER_SPIFFS_OFFSET = DEFINED(eboot_get_flash_block_size) ? 0x2000 : 0;

PROVIDE ( _SPIFFS_start = 0x40200000  + _BOOTLOADER_SPIFFS_OFFSET + _SPIFFS_start_offset );
PROVIDE ( _SPIFFS_end = 0x405FB000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x2000 );
PROVIDE ( _BOOTLOADER_DATA = DEFINED(eboot_get_flash_block_size) ? _SPIFFS_start - _BOOTLOADER_SPIFFS_OFFSET : 0 );
PROVIDE ( _SKETCH_AREA_end = DEFINED(eboot_get_flash_block_size) ? _BOOTLOADER_DATA : _SPIFFS_start );

INCLUDE "../ld/eagle.app.v6.common.ld"
