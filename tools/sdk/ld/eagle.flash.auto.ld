/* Flash Split */
/* sketch @0x40200000 (~1019KB) (1044464B) = MAX */

MEMORY
{
  dport0_0_seg :                        org = 0x3FF00000, len = 0x10
  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  irom0_0_seg :                         org = 0x40201010, len = 0xfeff0
}

INCLUDE "local.eagle.app.v6.common.ld"
