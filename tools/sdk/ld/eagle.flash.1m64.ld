/* Flash Split for 1M chips */
/* sketch @0x40200000 (~935KB) (958448B) */
/* empty  @0x402E9FF0 (~4KB) (4112B) */
/* spiffs @0x402EB000 (~64KB) (65536B) */
/* eeprom @0x402FB000 (4KB) */
/* rfcal  @0x402FC000 (4KB) */
/* wifi   @0x402FD000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0xC000 /* Actual len: MMU_IRAM_SIZE */
  irom0_0_seg :                         org = 0x40201010, len = 0xe9ff0
}

PROVIDE ( _FS_start = 0x402EB000 );
PROVIDE ( _FS_end = 0x402FB000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x1000 );
PROVIDE ( _EEPROM_start = 0x402fb000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x402EB000 );
PROVIDE ( _SPIFFS_end = 0x402FB000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x1000 );

INCLUDE "local.eagle.app.v6.common.ld"
