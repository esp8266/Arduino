/* Flash Split for 512K chips */
/* sketch 487KB */
/* eeprom @0x7B000 (4KB) */
/* rfcal @0x7C000 (4KB) */
/* sdk wifi settings @0x7D000 (12KB) */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iram1_0_seg :                         org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0x79ff0
}


INCLUDE "eagle.app.v6.common.ld"
