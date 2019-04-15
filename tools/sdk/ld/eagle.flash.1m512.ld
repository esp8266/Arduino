/* Flash Split for 1M chips */
/* sketch @0x40200000 (~487KB) (499696B) */
/* empty  @0x40279FF0 (~4KB) (4112B) */
/* spiffs @0x4027B000 (~512KB) (524288B) */
/* eeprom @0x402FB000 (4KB) */
/* rfcal  @0x402FC000 (4KB) */
/* wifi   @0x402FD000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0x79ff0
}

PROVIDE ( _SPIFFS_start = 0x4027B000 );
PROVIDE ( _SPIFFS_end = 0x402FB000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x1000 );

INCLUDE "local.eagle.app.v6.common.ld"
