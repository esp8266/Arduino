/* Flash Split for 512KB chips */
/* sketch @0x40201010 (~423KB) (434160B) */
/* fs     @0x4026B000 (~64KB) (65536B) */
/* eeprom @0x4027B000 (4096B) */
/* rfcal  @0x4027C000 (4096B) */
/* wifi   @0x4027D000 (12288B) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  irom0_0_seg :                         org = 0x40201010, len = 0x69ff0
}

PROVIDE ( _FS_start = 0x4026B000 );
PROVIDE ( _FS_end = 0x4027B000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x1000 );
PROVIDE ( _EEPROM_start = 0x4027B000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x4026B000 );
PROVIDE ( _SPIFFS_end = 0x4027B000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x1000 );

INCLUDE "local.eagle.app.v6.common.ld"
