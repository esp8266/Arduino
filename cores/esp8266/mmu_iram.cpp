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

#define ICACHE_SIZE_32 1
#define ICACHE_SIZE_16 0

extern "C" {

#if (MMU_ICACHE_SIZE == 0x4000)
#define SOC_CACHE_SIZE ICACHE_SIZE_16
#pragma message("ICACHE size 16K")
#else
#define SOC_CACHE_SIZE ICACHE_SIZE_32
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

//C This was used to probe at different stages of boot the state of the PLL
//C register. I think we can get rid of this one.
extern "C" uint8_t rom_i2c_readReg(uint8_t block, uint8_t host_id, uint8_t reg_add);
extern "C" void rom_i2c_writeReg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
extern "C" void IRAM_ATTR dbg_set_pll(void)
{
  char r103_4_1 = rom_i2c_readReg(103,4,1);
  char r103_4_2 = rom_i2c_readReg(103,4,2);
  mmu_set_pll();
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
#else   // #ifdef DEV_DEBUG_PRINT
extern void Cache_Read_Disable(void);
#endif  // #ifdef DEV_DEBUG_PRINT

#else   // #if (MMU_ICACHE_SIZE == 0x4000)
extern void Cache_Read_Enable(uint8_t map, uint8_t p, uint8_t v);
#endif  // #if (MMU_ICACHE_SIZE == 0x4000)

/*
 * Early adjustment for CPU crystal frequency will allow early debug printing to
 * be readable before the SDK initialization is complete.
 * This should not be left enabled all the time in Cashe_Read..., I am concerned
 * that there may be unknown interference with the NONOS SDK startup.
 * It does low-level calls that could clash with the SDKs startup.
 *
 * Inspired by:
 * https://github.com/pvvx/esp8266web/blob/2e25559bc489487747205db2ef171d48326b32d4/app/sdklib/system/app_main.c#L581-L591
 */
extern "C" uint8_t rom_i2c_readReg(uint8_t block, uint8_t host_id, uint8_t reg_add);
extern "C" void rom_i2c_writeReg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);

extern "C" void IRAM_ATTR mmu_set_pll(void)
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

/*
 * This wrapper is for running code early from IROM (flash) before the SDK
 * starts. Since the NONOS SDK will do a full and proper flash device init for
 * speed and mode, we only do a minimum to make ICACHE functional, keeping IRAM
 * use to a minimum. After the SDK has started, this function is not needed and
 * must not be called.
 */
void IRAM_ATTR mmu_wrap_irom_fn(void (*fn)(void)) {
  // Cache Read must be disabled. This is always the case on entry when called
  // from the right context.
  // Cache_Read_Disable();

  // The SPI_CS_SETUP parameter has been observed set by RTOS SDK and NONOS SDK
  // as part of flash init/configuration. It may be necessary for some flash
  // chips to perform correctly with ICACHE hardware access. Turning on and
  // leaving it on should be okay.
  //
  // One SPI bus clock cycle time is inserted between #CS active and 1st SPI bus
  // clock cycle. The number of clock cycles is in SPI_CNTRL2 SPI_SETUP_TIME,
  // defaults to 1.
  SPI0U |= SPIUCSSETUP; // SPI_CS_SETUP or BIT5

  // phy_get_bb_evm is the key function, called from fix_cache_bug in the NONOS
  // SDK. This addition resolves the PUYA Flash issue with exception 0, when
  // early Cache_Read_Enable is used.
  extern uint32_t phy_get_bb_evm(void); // undocumented
  phy_get_bb_evm();

  // For early Cache_Read_Enable, only do ICACHE_SIZE_16. With this option,
  // Cache_Read_Disable will fully restore the original register states. With
  // ICACHE_SIZE_32, one bit is missed when disabling. Leave the full access
  // calls for the NONOS SDK.
  // This only works with image slice 0, which is all we do presently.
  Cache_Read_Enable(0, 0, ICACHE_SIZE_16);
  fn();
  Cache_Read_Disable();
}

};
