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

#ifndef __MMU_IRAM_H
#define __MMU_IRAM_H

#include <stdint.h>
#include <c_types.h>
#include <assert.h>

/*
 * DEV_DEBUG_MMU_IRAM:
 *   Debug printing for developing mmu_iram. I don't think it would be usful for
 *   general debugging.
 *
 * DEV_DEBUG_PRINT:
 *   Debug printing macros for printing before before, during, and after
 *   NONOS SDK initializes. May or maynot be safe during NONOS SDK
 *   initialization. As in printing from functions called on by the SDK
 *   during the SDK initialization.
 *
 #define DEV_DEBUG_MMU_IRAM
 #define DEV_DEBUG_PRINT
 */

#ifdef DEBUG_ESP_CORE
#define DEBUG_MMU
#endif

/*
  The more I look at _xtos_c_wrapper_handler the more convinced I am that this
  USE_ISR_SAFE_EXC_WRAPPER is required.
*/
#define USE_ISR_SAFE_EXC_WRAPPER

#if defined( DEV_DEBUG_PRINT) || defined(DEV_DEBUG_MMU_IRAM) || defined(DEBUG_MMU)
#include <esp8266_peri.h>

#if defined( DEV_DEBUG_PRINT) || defined(DEV_DEBUG_MMU_IRAM)
extern "C" void set_pll(void);
extern "C" void dbg_set_pll(void);
#define SET_PLL() set_pll()

#else
#define SET_PLL() do {} while(false)
#endif

#define ETS_FLUSH(a) while((USS(a) >> USTXC) & 0xff) {}

#define ETS_PRINTF(fmt, ...) \
  SET_PLL(); \
  uart_buff_switch(0); \
  ets_uart_printf(fmt, ##__VA_ARGS__); \
  ETS_FLUSH(0)

#else     // ! DEV_DEBUG_PRINT
#define ETS_FLUSH(...) do {} while(false)
#define ETS_PRINTF(...) do {} while(false)
#endif    // DEV_DEBUG_PRINT

#define DBG_MMU_PRINTF ETS_PRINTF

#ifdef DEV_DEBUG_MMU_IRAM
#define DBG_MMU_PRINT_STATUS() { \
    DBG_MMU_PRINTF("\nmmu_status = {" \
                   "v_cfg = %u, state = %d, enable/disable count = %u/%u, " \
                   "map = 0x%02X, p = 0x%02X, v = 0x%02X}\n", \
                   mmu_status.v_cfg, mmu_status.state, \
                   mmu_status.enable_count, mmu_status.disable_count, \
                   mmu_status.map, mmu_status.p, mmu_status.v); \
    ETS_FLUSH(0); \
}

#define DBG_MMU_PRINT_IRAM_BANK_REG(a, b) { \
    uint32_t iram_bank_reg = ESP8266_DREG(0x24); \
    DBG_MMU_PRINTF("\niram_bank_reg %s%s 0x%08X\n", (0 == a) ? "" : a, (0 == a) ? "" : " Cache_Read_" b, iram_bank_reg); \
}

#else     // ! DEV_DEBUG_MMU_IRAM
#define DBG_MMU_PRINT_STATUS(...) do {} while(false)
#define DBG_MMU_PRINT_IRAM_BANK_REG(...) do {} while(false)
#endif    // #if DEV_DEBUG_MMU_IRAM


#ifdef __cplusplus
extern "C" {
#endif

typedef struct MMU_CRE_STATUS {
  uint32_t v_cfg;
  int32_t state;      // -1 - not initialized, 0 - disabled, 1 - enabled
  uint32_t enable_count;
  uint32_t disable_count;
  uint32_t map;
  uint32_t p;
  uint32_t v;
}  mmu_cre_status_t;

extern mmu_cre_status_t mmu_status;


#ifdef DEBUG_MMU

static inline bool is_iram(uint32_t addr) {
// constexpr uint32_t _start = 0x40100000UL;
#define IRAM_START 0x40100000UL
#ifndef MMU_IRAM_SIZE
#define MMU_IRAM_SIZE 0x8000UL
#endif
#define IRAM_END (IRAM_START + MMU_IRAM_SIZE)

  return (IRAM_START <= addr && IRAM_END > addr);
}

static inline bool is_dram(uint32_t addr) {
  // constexpr uint32_t _start = 0x3FF80000UL;
  // constexpr uint32_t _end = 0x40000000UL;
  return (0x3FF80000UL <= addr && 0x40000000UL > addr);
}

static inline bool is_icache(uint32_t addr) {
  // constexpr uint32_t _start = 0x40200000UL;
  // constexpr uint32_t _end = _start + 0x100000UL;
  #define ICACHE_START 0x40200000UL
  #define ICACHE_END (ICACHE_START + 0x100000UL)
  return (ICACHE_START <= addr && ICACHE_END > addr);
}

#else
static inline bool is_iram(uint32_t addr) {
  (void)addr;
  return true;
}

static inline bool is_dram(uint32_t addr) {
  (void)addr;
  return true;
}

static inline bool is_icache(uint32_t addr) {
  (void)addr;
  return true;
}
#endif  // #ifdef DEBUG_MMU

#ifdef DEBUG_MMU
#define ASSERT_RANGE_TEST_WRITE(a) \
  if (is_iram((uint32_t)a) || is_dram((uint32_t)a)) { \
  } else { \
    DBG_MMU_PRINTF("\nexcvaddr: %p\n", a); \
    assert(("Outside of Range - Write" && false)); \
  }

#define ASSERT_RANGE_TEST_READ(a) \
  if (is_iram((uint32_t)a) || is_dram((uint32_t)a) || is_icache((uint32_t)a)) { \
  } else { \
    DBG_MMU_PRINTF("\nexcvaddr: %p\n", a); \
    assert(("Outside of Range - Read" && false)); \
  }

#else
#define ASSERT_RANGE_TEST_WRITE(a) do {} while(false)
#define ASSERT_RANGE_TEST_READ(a) do {} while(false)
#endif

/*
 * Some inlines to allow faster random access to non32bit access of iRAM or
 * iCACHE data elements. These remove the extra time and stack space that would
 * have occured by relying on exception processing.
 */
static inline uint8_t get_uint8_iram(const void *p8) {
  ASSERT_RANGE_TEST_READ(p8);
  uint32_t val = (*(uint32_t *)((uintptr_t)p8 & ~0x3));
  uint32_t pos = ((uintptr_t)p8 & 0x3) * 8;
  val >>= pos;
  return (uint8_t)val;
}

static inline uint16_t get_uint16_iram(const unsigned short *p16) {
  ASSERT_RANGE_TEST_READ(p16);
  uint32_t val = (*(uint32_t *)((uintptr_t)p16 & ~0x3));
  uint32_t pos = ((uintptr_t)p16 & 0x3) * 8;
  val >>= pos;
  return (uint16_t)val;
}

static inline int16_t get_int16_iram(const short *p16) {
  ASSERT_RANGE_TEST_READ(p16);
  uint32_t val = (*(uint32_t *)((uintptr_t)p16 & ~0x3));
  uint32_t pos = ((uintptr_t)p16 & 0x3) * 8;
  val >>= pos;
  return (int16_t)val;
}



static inline uint8_t set_uint8_iram(void *p8, const uint8_t val) {
  ASSERT_RANGE_TEST_WRITE(p8);
  uint32_t pos = ((uintptr_t)p8 & 0x3) * 8;
  uint32_t sval = val << pos;
  uint32_t valmask =  0x0FF << pos;

  uint32_t *p32 = (uint32_t *)((uintptr_t)p8 & ~0x3);
  uint32_t ival = *p32;
  ival &= (~valmask);
  ival |= sval;
  *p32 = ival;
  return val;
}

static inline uint16_t set_uint16_iram(unsigned short *p16, const uint16_t val) {
  ASSERT_RANGE_TEST_WRITE(p16);
  uint32_t pos = ((uintptr_t)p16 & 0x3) * 8;
  uint32_t sval = val << pos;
  uint32_t valmask =  0x0FFFF << pos;

  uint32_t *p32 = (uint32_t *)((uintptr_t)p16 & ~0x3);
  uint32_t ival = *p32;
  ival &= (~valmask);
  ival |= sval;
  *p32 = ival;
  return val;
}

static inline int16_t set_int16_iram(short *p16, const int16_t val) {
  ASSERT_RANGE_TEST_WRITE(p16);
  uint32_t sval = (uint16_t)val;
  uint32_t pos = ((uintptr_t)p16 & 0x3) * 8;
  sval <<= pos;
  uint32_t valmask =  0x0FFFF << pos;

  uint32_t *p32 = (uint32_t *)((uintptr_t)p16 & ~0x3);
  uint32_t ival = *p32;
  ival &= (~valmask);
  ival |= sval;
  *p32 = ival;
  return val;
}

#ifdef __cplusplus
}
#endif

#if (MMU_IRAM_SIZE > 32*1024) && !defined(MMU_SEC_HEAP)
extern "C" void _text_end(void);
#define MMU_SEC_HEAP mmu_sec_heap()
#define MMU_SEC_HEAP_SIZE mmu_sec_heap_size()

static inline __attribute__((always_inline))
void *mmu_sec_heap(void) {
  uint32_t sec_heap = (uint32_t)_text_end + 32;
  return (void *)(sec_heap &= ~7);
}

static inline __attribute__((always_inline))
size_t mmu_sec_heap_size(void) {
  return (size_t)0xC000UL - ((size_t)mmu_sec_heap() - 0x40100000UL);
}
#endif

#endif
