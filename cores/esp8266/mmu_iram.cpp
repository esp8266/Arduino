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

#if (MMU_ICACHE_SIZE == 0x4000)
#define SOC_CACHE_SIZE 0 // 16KB
#pragma message("ICACHE size 16K")
#else
#define SOC_CACHE_SIZE 1 // 32KB
#endif

// IS this set as part of segment data at load time? appears to be
mmu_cre_status_t mmu_status = {SOC_CACHE_SIZE, -1, 0, 0, 0, 0, 0};

// Enable all the time so we can gather information on Cache_Read_Enable usage.
#if 1 //(MMU_ICACHE_SIZE == 0x4000)
/*
 * "Cache_Read_Enable" as in Instruction Read Cache enable, ICACHE.
 *
 * The Boot ROM "Cache_Read_Enable" API enables virtual execution of code in
 * flash memory via an instruction cache, ICACHE. The cache size can be set to
 * 16K or 32K, and the NONOS SDK 2.x will always set ICACHE to 32K during
 * initialization.
 *
 * When you select a 16K vs. a 32K ICACHE size, you get 48K contiguous IRAM to
 * work with. The NONOS SDK 2.x does not have an option to select 16K/32K. This
 * is where this Boot ROM wrapper for Cache_Read_Enable comes in.
 * Note, there is support for 16K/32K cache size in NONOS SDK 3.0; however, I
 * do not see an option to have it has part of your general IRAM. That SDK adds
 * it to the heap.
 *
 * With this wrapper function, we override the SDK's ICACHE size.
 * A build-time define MMU_ICACHE_SIZE selects 16K or 32K ICACHE size.
 *
 * mmu_status is used to help understand calling behavior. At some point, it
 * should be trimmed down to the essentials.
 *
 * During NONOS SDK init, it will call to enable. Then call later, to process a
 * spi_flash_get_id request, it will disable/enable around the Boot ROM SPI calls.
 *
 *
 *
 *                   Arguments for Cache_Read_Enable
 *
 * The first two arguments appear to specify which 1MB block of the flash to
 * access with the ICACHE.
 *
 *   The first argument, map, is partly understood. It has three values 0, 1,
 *   and 2+. The value 0 selects the even 1MB block, and 1 selects the odd 1MB
 *   block, in other words, bit20 of the flash address. No guesses for a value
 *   of 2 or greater.
 *
 *   The second argument, p, bit 21 of the flash address. Or, it may be bits 23,
 *   22, 21 of the flash address. A three-bit field is cleared in the register
 *   for this argument; however, I have not seen any examples of it being used
 *   that way.
 *
 * The third argument, v, holds our center of attention. A value of 0 selects
 * 16K, and a value of 1 selects a 32K ICACHE. This is the only parameter we
 * need to modify on Cache_Read_Enable calls.
 *
 *
 *
 *                   Clues and Information sources
 *
 * "Cache_Read_Enable" is underdocumented. Main sources of information were from
 * rboot, zboot, https://richard.burtons.org/2015/06/12/esp8266-cache_read_enable/,
 * and other places. And some additional expermentation.
 *
 * Searching through the NONOS SDK shows nothing on this API; however, some
 * clues on what the NONOS SDK might be doing with ICACHE related calls can be
 * found in the RTOS SDK.
 * eg. ESP8266_RTOS_SDK/blob/master/components/spi_flash/src/spi_flash_raw.c
 * also calls to it in the bootloader.
 *
 */

#ifndef ROM_Cache_Read_Enable
#define ROM_Cache_Read_Enable         0x40004678
#endif

typedef void (*fp_Cache_Read_Enable_t)(uint8_t map, uint8_t p, uint8_t v);
constexpr fp_Cache_Read_Enable_t real_Cache_Read_Enable =
         (fp_Cache_Read_Enable_t)ROM_Cache_Read_Enable;

void IRAM_ATTR Cache_Read_Enable(uint8_t map, uint8_t p, uint8_t v) {
  mmu_status.map = map;
  mmu_status.p = p;
  mmu_status.v = v;
  mmu_status.enable_count++;
  mmu_status.state = 1;
  if (0 == mmu_status.enable_count) {
    mmu_status.enable_count--;   // keep saturated value
  }
  DBG_MMU_PRINT_IRAM_BANK_REG("before");

  real_Cache_Read_Enable(map, p, SOC_CACHE_SIZE);

  DBG_MMU_PRINT_IRAM_BANK_REG("after");
  DBG_MM_PRINT_STATUS();
}

#ifndef ROM_Cache_Read_Disable
#define ROM_Cache_Read_Disable         0x400047f0
#endif

typedef void (*fp_Cache_Read_Disable_t)(void);
constexpr fp_Cache_Read_Disable_t real_Cache_Read_Disable =
         (fp_Cache_Read_Disable_t)ROM_Cache_Read_Disable;
/*
 *
 */
void IRAM_ATTR Cache_Read_Disable(void) {

  mmu_status.disable_count++;
  mmu_status.state = 0;
  if (0 == mmu_status.disable_count) {
    mmu_status.disable_count--;   // keep saturated value
  }
  DBG_MMU_PRINT_IRAM_BANK_REG("before");

  real_Cache_Read_Disable();

  DBG_MMU_PRINT_IRAM_BANK_REG("after");
  DBG_MM_PRINT_STATUS();
}

#endif

};
