/* Flash Split for 1M chips */
/* sketch @0x40200000 (~991KB) (1015792B) */
/* empty    @0x402F7FF0 (~4KB) (4112B) */
/* otacmds  @0x402F9000 (8KB) */
/* spiffs   @0x402FB000 (~0KB) (0B) */
/* eeprom   @0x402FB000 (4KB) */
/* rfcal    @0x402FC000 (4KB) */
/* wifi     @0x402FD000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0xf7ff0
}

PROVIDE ( _FS_start = 0x402FB000 );
PROVIDE ( _FS_end = 0x402FB000 );
PROVIDE ( _FS_page = 0x0 );
PROVIDE ( _FS_block = 0x0 );
PROVIDE ( _BOOTLOADER_DATA = 0x000F9000 );
PROVIDE ( _SKETCH_AREA_end = _FS_start );

INCLUDE "local.eagle.app.v6.common.ld"
