/* Flash Split for 2M chips */
/* sketch @0x40200000 (~1019KB) (1044464B) */
/* otacmds  @0x402FE000 (8KB) */
/* spiffs   @0x40300000 (~1000KB) (1024000B) */
/* eeprom   @0x403FB000 (4KB) */
/* rfcal    @0x403FC000 (4KB) */
/* wifi     @0x403FD000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0xfeff0
}

PROVIDE ( _FS_start = 0x40300000 );
PROVIDE ( _FS_end = 0x403FA000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x2000 );
PROVIDE ( _BOOTLOADER_DATA = 0x000FE000 );
PROVIDE ( _SKETCH_AREA_end = _FS_start );

INCLUDE "local.eagle.app.v6.common.ld"
