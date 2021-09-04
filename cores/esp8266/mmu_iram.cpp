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
#include <user_interface.h>

extern "C" {

#if (MMU_ICACHE_SIZE == 0x4000)
#define SOC_CACHE_SIZE 0 // 16KB
#pragma message("ICACHE size 16K")
#else
#define SOC_CACHE_SIZE 1 // 32KB
#endif

#if (MMU_ICACHE_SIZE == 0x4000)
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
 * and other places. And some additional experimentation.
 *
 * Searching through the NONOS SDK shows nothing on this API; however, some
 * clues on what the NONOS SDK might be doing with ICACHE related calls can be
 * found in the RTOS SDK.
 * eg. ESP8266_RTOS_SDK/blob/master/components/spi_flash/src/spi_flash_raw.c
 * also calls to it in the bootloader.
 *
 */

#ifndef ROM_Cache_Read_Enable
#define ROM_Cache_Read_Enable         0x40004678U
#endif

typedef void (*fp_Cache_Read_Enable_t)(uint8_t map, uint8_t p, uint8_t v);
#define real_Cache_Read_Enable (reinterpret_cast<fp_Cache_Read_Enable_t>(ROM_Cache_Read_Enable))

void IRAM_ATTR Cache_Read_Enable(uint8_t map, uint8_t p, uint8_t v) {
  (void)v;
  real_Cache_Read_Enable(map, p, SOC_CACHE_SIZE);
}

#ifdef DEV_DEBUG_PRINT

#if 0
#ifndef ROM_Cache_Read_Disable
#define ROM_Cache_Read_Disable         0x400047f0
#endif

typedef void (*fp_Cache_Read_Disable_t)(void);
#define real_Cache_Read_Disable (reinterpret_cast<fp_Cache_Read_Disable_t>(ROM_Cache_Read_Disable))
/*
 *
 */
void IRAM_ATTR Cache_Read_Disable(void) {
  real_Cache_Read_Disable();
}
#endif

/*
 * Early adjustment for CPU crystal frequency, so debug printing will work.
 * This should not be left enabled all the time in Cashe_Read..., I am concerned
 * that there may be unknown interference with the NONOS SDK startup.
 *
 * Inspired by:
 * https://github.com/pvvx/esp8266web/blob/2e25559bc489487747205db2ef171d48326b32d4/app/sdklib/system/app_main.c#L581-L591
 */
extern "C" uint8_t rom_i2c_readReg(uint8_t block, uint8_t host_id, uint8_t reg_add);
extern "C" void rom_i2c_writeReg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);

extern "C" void IRAM_ATTR set_pll(void)
{
#if !defined(F_CRYSTAL)
#define F_CRYSTAL 26000000
#endif
  if (F_CRYSTAL != 40000000) {
    // At Boot ROM(-BIOS) start, it assumes a 40MHz crystal.
    // If it is not, we assume a 26MHz crystal.
    // There is no support for 24MHz crustal at this time.
    if(rom_i2c_readReg(103,4,1) != 136) { // 8: 40MHz, 136: 26MHz
      // Assume 26MHz crystal
      // soc_param0: 0: 40MHz, 1: 26MHz, 2: 24MHz
      // set 80MHz PLL CPU
      rom_i2c_writeReg(103,4,1,136);
      rom_i2c_writeReg(103,4,2,145);
    }
  }
}

//C This was used to probe at different stages of boot the state of the PLL
//C register. I think we can get rid of this one.
extern "C" void IRAM_ATTR dbg_set_pll(void)
{
  char r103_4_1 = rom_i2c_readReg(103,4,1);
  char r103_4_2 = rom_i2c_readReg(103,4,2);
  set_pll();
  ets_uart_printf("\nrom_i2c_readReg(103,4,1) == %u\n", r103_4_1);
  ets_uart_printf(  "rom_i2c_readReg(103,4,2) == %u\n", r103_4_2);
}

/*
  This helps keep the UART enabled at user_init() so we can get a few more
  messages printed.
*/
extern struct rst_info resetInfo;
extern "C" void __pinMode( uint8_t pin, uint8_t mode );

inline bool is_gpio_persistent(void) {
    return REASON_EXCEPTION_RST <= resetInfo.reason &&
           REASON_SOFT_RESTART  >= resetInfo.reason;
}

extern "C" void pinMode( uint8_t pin, uint8_t mode ) {
    static bool in_initPins = true;
    if (in_initPins && (1 == pin)) {
        if (!is_gpio_persistent()) {
            /* Restore pin to TX after Power-on and EXT_RST */
            __pinMode(pin, FUNCTION_0);
        }
        in_initPins = false;
        return;
    }

    __pinMode( pin, mode );
}
#endif  // #ifdef DEV_DEBUG_PRINT

#endif  // #if (MMU_ICACHE_SIZE == 0x4000)

};
