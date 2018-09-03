/* Flash Split for 1M chips */
/* sketch 999KB */
/* eeprom 4KB rfcal 16KB */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0xf9ff0
}


INCLUDE "eagle.app.v6.common.ld"
