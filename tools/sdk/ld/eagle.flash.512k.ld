/* Flash Split for 512K chips */
/* sketch @0x40200000 (~487KB) (499696B) */
/* eeprom @0x4027B000 (=4KB) */
/* rfcal  @0x4027C000 (=4KB) */
/* wifi   @0x4027D000 (=12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0x79ff0
}


INCLUDE "eagle.app.v6.common.ld"
