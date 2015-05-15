/* Flash Split for 1M chips */
/* irom0  876KB */
/* spiffs 64KB */
/* eeprom 20KB */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40210000, len = 0xDB000
}

PROVIDE ( _SPIFFS_start = 0x402EB000 );
PROVIDE ( _SPIFFS_end = 0x402FB000 );

INCLUDE "../ld/eagle.app.v6.common.ld"
