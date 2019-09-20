/* Flash Split for 16M chips */
/* sketch @0x40200000 (~1019KB) (1044464B) */
/* empty    @0x402FEFF0 (~1020KB) (1044496B) */
/* otacmds  @0x403FE000 (8KB) */
/* spiffs   @0x40400000 (~14312KB) (14655488B) */
/* eeprom   @0x411FB000 (4KB) */
/* rfcal    @0x411FC000 (4KB) */
/* wifi     @0x411FD000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0xfeff0
}

PROVIDE ( _FS_start = 0x40400000 );
PROVIDE ( _FS_end = 0x411FA000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x2000 );
PROVIDE ( _BOOTLOADER_DATA = 0x001FE000 );
PROVIDE ( _SKETCH_AREA_end = _FS_start );

INCLUDE "local.eagle.app.v6.common.ld"
