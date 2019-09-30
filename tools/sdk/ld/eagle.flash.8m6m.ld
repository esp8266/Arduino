/* Flash Split for 8M chips */
/* sketch @0x40200000 (~1019KB) (1044464B) */
/* empty    @0x402FEFF0 (~1020KB) (1044496B) */
/* otacmds  @0x403FE000 (8KB) */
/* spiffs   @0x40400000 (~6120KB) (6266880B) */
/* eeprom   @0x409FB000 (4KB) */
/* rfcal    @0x409FC000 (4KB) */
/* wifi     @0x409FD000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0xfeff0
}

PROVIDE ( _FS_start = 0x40400000 );
PROVIDE ( _FS_end = 0x409FA000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x2000 );
PROVIDE ( _BOOTLOADER_DATA = 0x403FE000 );
PROVIDE ( _SKETCH_AREA_end = _BOOTLOADER_DATA );
PROVIDE ( _EEPROM_start = 0x409fb000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x40400000 );
PROVIDE ( _SPIFFS_end = 0x409FA000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x2000 );

INCLUDE "local.eagle.app.v6.common.ld"
