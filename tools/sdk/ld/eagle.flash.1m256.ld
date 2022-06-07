/* Flash Split for 1MB chips */
/* sketch @0x40201010 (~743KB) (761840B) */
/* fs     @0x402BB000 (~256KB) (262144B) */
/* eeprom @0x402FB000 (4096B) */
/* rfcal  @0x402FC000 (4096B) */
/* wifi   @0x402FD000 (12288B) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  irom0_0_seg :                         org = 0x40201010, len = 0xb9ff0
}

PROVIDE ( _FS_start = 0x402BB000 );
PROVIDE ( _FS_end = 0x402FB000 );
PROVIDE ( _FS_page = 0x100 );
PROVIDE ( _FS_block = 0x1000 );
PROVIDE ( _EEPROM_start = 0x402FB000 );
/* The following symbols are DEPRECATED and will be REMOVED in a future release */
PROVIDE ( _SPIFFS_start = 0x402BB000 );
PROVIDE ( _SPIFFS_end = 0x402FB000 );
PROVIDE ( _SPIFFS_page = 0x100 );
PROVIDE ( _SPIFFS_block = 0x1000 );

INCLUDE "local.eagle.app.v6.common.ld"
