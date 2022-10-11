/* Flash Split for 2MB chips */
/* sketch @0x40201010 (~1019KB) (1044464B) */
/* empty  @0x40300000 (~960KB) (983040B) */
/* fs     @0x403F0000 (~44KB) (45056B) */
/* eeprom @0x403FB000 (4096B) */
/* rfcal  @0x403FC000 (4096B) */
/* wifi   @0x403FD000 (12288B) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  irom0_0_seg :                         org = 0x40201010, len = 0xfeff0
}

PROVIDE ( _FS_start = 0x403F0000 );
PROVIDE ( _FS_end = 0x403FB000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x1000 );
PROVIDE ( _EEPROM_start = 0x403FB000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x403F0000 );
PROVIDE ( _SPIFFS_end = 0x403FB000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x1000 );

INCLUDE "local.eagle.app.v6.common.ld"
