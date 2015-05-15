/* Flash Split for 256K chips */
/* irom0  108KB */
/* spiffs 64KB */
/* eeprom 20KB */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40210000, len = 0x1B000
}

PROVIDE ( _SPIFFS_start = 0x4022B000 );
PROVIDE ( _SPIFFS_end = 0x4023B000 );

INCLUDE "../ld/eagle.app.v6.common.ld"
