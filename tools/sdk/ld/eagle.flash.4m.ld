/* Flash Split for 4M chips */
/* sketch @0x40200000 (~1019KB) (1044464B) */
/* empty  @0x402FEFF0 (~3056KB) (3129360B) */
/* spiffs @0x405FB000 (~0KB) (0B) */
/* eeprom @0x405FB000 (4KB) */
/* rfcal  @0x405FC000 (4KB) */
/* wifi   @0x405FD000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  irom0_0_seg :                         org = 0x40201010, len = 0xfeff0
}

PROVIDE ( _FS_start = 0x405FB000 );
PROVIDE ( _FS_end = 0x405FB000 );
PROVIDE ( _FS_page = 0x0 );
PROVIDE ( _FS_block = 0x0 );
PROVIDE ( _EEPROM_start = 0x405fb000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x405FB000 );
PROVIDE ( _SPIFFS_end = 0x405FB000 );
PROVIDE ( _SPIFFS_page = 0x0 );
PROVIDE ( _SPIFFS_block = 0x0 );

INCLUDE "local.eagle.app.v6.common.ld"
