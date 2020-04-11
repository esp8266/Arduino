/* Flash Split for 2M chips */
/* sketch @0x40200000 (~1019KB) (1044464B) */
/* empty  @0x402FEFF0 (~516KB) (528400B) */
/* spiffs @0x40380000 (~488KB) (499712B) */
/* eeprom @0x403FB000 (4KB) */
/* rfcal  @0x403FC000 (4KB) */
/* wifi   @0x403FD000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0xfeff0
}

PROVIDE ( _FS_start = 0x40380000 );
PROVIDE ( _FS_end = 0x403FA000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x2000 );
PROVIDE ( _EEPROM_start = 0x403fb000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x40380000 );
PROVIDE ( _SPIFFS_end = 0x403FA000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x2000 );

INCLUDE "local.eagle.app.v6.common.ld"
