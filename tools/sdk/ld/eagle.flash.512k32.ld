/* Flash Split for 512K chips */
/* sketch @0x40200000 (~447KB) (458736B) */
/* empty    @0x4026FFF0 (~4KB) (4112B) */
/* otacmds  @0x40271000 (8KB) */
/* spiffs   @0x40273000 (~32KB) (32768B) */
/* eeprom   @0x4027B000 (4KB) */
/* rfcal    @0x4027C000 (4KB) */
/* wifi     @0x4027D000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0x6fff0
}

PROVIDE ( _FS_start = 0x40273000 );
PROVIDE ( _FS_end = 0x4027B000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x1000 );
PROVIDE ( _BOOTLOADER_DATA = 0x40271000 );
PROVIDE ( _SKETCH_AREA_end = _BOOTLOADER_DATA );
PROVIDE ( _EEPROM_start = 0x4027b000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x40273000 );
PROVIDE ( _SPIFFS_end = 0x4027B000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x1000 );

INCLUDE "local.eagle.app.v6.common.ld"
