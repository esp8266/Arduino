/* Flash Split for 512K chips */
/* sketch @0x40200000 (~487KB) (499696B) */
/* empty  @0x40279FF0 (~4KB) (4112B) */
/* spiffs @0x4027B000 (~0KB) (0B) */
/* eeprom @0x4027B000 (4KB) */
/* rfcal  @0x4027C000 (4KB) */
/* wifi   @0x4027D000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = IRAM_SIZE
  irom0_0_seg :                         org = 0x40201010, len = 0x79ff0
}

PROVIDE ( _FS_start = 0x4027B000 );
PROVIDE ( _FS_end = 0x4027B000 );
PROVIDE ( _FS_page = 0x0 );
PROVIDE ( _FS_block = 0x0 );
PROVIDE ( _EEPROM_start = 0x4027b000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x4027B000 );
PROVIDE ( _SPIFFS_end = 0x4027B000 );
PROVIDE ( _SPIFFS_page = 0x0 );
PROVIDE ( _SPIFFS_block = 0x0 );

INCLUDE "local.eagle.app.v6.common.ld"
