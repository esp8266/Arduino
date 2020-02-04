/*
 *   Copyright 2020 M Hightower
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include "Arduino.h"
#include "mmu_iram.h"

extern "C" {

int Cache_Read_Enable_status = -1;

mmu_cre_status_t mmu_status __attribute__((section(".noinit")));

#if (MMU_ICACHE_SIZE == 0x4000)
#define SOC_CACHE_SIZE 0 // 16KB
// #define SOC_CACHE_SIZE 1 // 32KB

#pragma message("ICACHE size 16K")


#ifndef ROM_Cache_Read_Enable
#define ROM_Cache_Read_Enable         0x40004678
#endif

typedef void (*fp_Cache_Read_Enable_t)(uint8_t map, uint8_t p, uint8_t v);
constexpr fp_Cache_Read_Enable_t real_Cache_Read_Enable = (fp_Cache_Read_Enable_t)ROM_Cache_Read_Enable;
/*
 * Override SDK's ICACHE size
 */
void IRAM_ATTR Cache_Read_Enable(uint8_t map, uint8_t p, uint8_t v) {
  mmu_status.map = map;
  mmu_status.p = p;
  mmu_status.v = v;
  real_Cache_Read_Enable(map, p, SOC_CACHE_SIZE);
  mmu_status.v_cfg = SOC_CACHE_SIZE;
  Cache_Read_Enable_status = SOC_CACHE_SIZE;
}
#endif

};
