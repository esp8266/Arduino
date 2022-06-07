/* Flash Split for 4MB chips */
/* sketch @0x40201010 (~1019KB) (1044464B) */
/* empty  @0x40300000 (~3052KB) (3125248B) */
/* eeprom @0x405FB000 (4096B) */
/* rfcal  @0x405FC000 (4096B) */
/* wifi   @0x405FD000 (12288B) */

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
PROVIDE ( _EEPROM_start = 0x405FB000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x405FB000 );
PROVIDE ( _SPIFFS_end = 0x405FB000 );
PROVIDE ( _SPIFFS_page = 0x0 );
PROVIDE ( _SPIFFS_block = 0x0 );

INCLUDE "local.eagle.app.v6.common.ld"
