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

//C This turns on range checking. Is this the value you want to trigger it?
#ifdef DEBUG_ESP_CORE
#define DEBUG_ESP_MMU
#endif

/*
  The more I look at _xtos_c_wrapper_handler the more convinced I am that this
  USE_ISR_SAFE_EXC_WRAPPER is required.
*/
#define USE_ISR_SAFE_EXC_WRAPPER

#if defined(DEV_DEBUG_PRINT) || defined(DEV_DEBUG_MMU_IRAM) || defined(DEBUG_ESP_MMU)
#include <esp8266_peri.h>

#define DBG_MMU_FLUSH(a) while((USS(a) >> USTXC) & 0xff) {}

#if defined(DEV_DEBUG_PRINT) || defined(DEV_DEBUG_MMU_IRAM)
extern "C" void set_pll(void);
extern "C" void dbg_set_pll(void);

#define DBG_MMU_PRINTF(fmt, ...) \
set_pll(); \
uart_buff_switch(0); \
ets_uart_printf(fmt, ##__VA_ARGS__); \
DBG_MMU_FLUSH(0)

#else
// ! defined(DEBUG_ESP_MMU)
#define DBG_MMU_PRINTF(fmt, ...) ets_uart_printf(fmt, ##__VA_ARGS__)
#endif

#else     // ! defined(DEV_DEBUG_PRINT) || defined(DEV_DEBUG_MMU_IRAM) || defined(DEBUG_ESP_MMU)
#define DBG_MMU_FLUSH(...) do {} while(false)
#define DBG_MMU_PRINTF(...) do {} while(false)
#endif    // defined(DEV_DEBUG_PRINT) || defined(DEV_DEBUG_MMU_IRAM) || defined(DEBUG_ESP_MMU)

#ifdef DEV_DEBUG_MMU_IRAM
#define DBG_MMU_PRINT_STATUS() { \
    DBG_MMU_PRINTF("\nmmu_status = {" \
                   "v_cfg = %u, state = %d, enable/disable count = %u/%u, " \
                   "map = 0x%02X, p = 0x%02X, v = 0x%02X}\n", \
                   mmu_status.v_cfg, mmu_status.state, \
                   mmu_status.enable_count, mmu_status.disable_count, \
                   mmu_status.map, mmu_status.p, mmu_status.v); \
    DBG_MMU_FLUSH(0); \
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

#ifdef DEV_DEBUG_MMU_IRAM
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
#endif

#ifdef DEBUG_ESP_MMU

static inline bool is_iram(const void *addr) {
  #define IRAM_START 0x40100000UL
  #ifndef MMU_IRAM_SIZE
  #error "MMU_IRAM_SIZE was undefined!"
  #endif
  #define IRAM_END (IRAM_START + MMU_IRAM_SIZE)

  return (IRAM_START <= (uint32_t)addr && IRAM_END > (uint32_t)addr);
}

static inline bool is_dram(const void *addr) {
  #define DRAM_START 0x3FF80000UL
  #define DRAM_END 0x40000000UL

  return (DRAM_START <= (uint32_t)addr && DRAM_END > (uint32_t)addr);
}

static inline bool is_icache(const void *addr) {
  #define ICACHE_START 0x40200000UL
  #define ICACHE_END (ICACHE_START + 0x100000UL)

  return (ICACHE_START <= (uint32_t)addr && ICACHE_END > (uint32_t)addr);
}

#else
static inline bool is_iram(const void *addr) {
  (void)addr;
  return true;
}

static inline bool is_dram(const void *addr) {
  (void)addr;
  return true;
}

static inline bool is_icache(const void *addr) {
  (void)addr;
  return true;
}
#endif  // #ifdef DEBUG_ESP_MMU

#ifdef DEBUG_ESP_MMU
#define ASSERT_RANGE_TEST_WRITE(a) \
  if (is_iram(a) || is_dram(a)) { \
  } else { \
    DBG_MMU_PRINTF("\nexcvaddr: %p\n", a); \
    assert(("Outside of Range - Write" && false)); \
  }

#define ASSERT_RANGE_TEST_READ(a) \
  if (is_iram(a) || is_dram(a) || is_icache(a)) { \
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
static inline uint8_t mmu_get_uint8(const void *p8) {
  ASSERT_RANGE_TEST_READ(p8);
  uint32_t val = (*(uint32_t *)((uintptr_t)p8 & ~0x3));
  uint32_t pos = ((uintptr_t)p8 & 0x3) * 8;
  val >>= pos;
  return (uint8_t)val;
}

static inline uint16_t mmu_get_uint16(const unsigned short *p16) {
  ASSERT_RANGE_TEST_READ(p16);
  uint32_t val = (*(uint32_t *)((uintptr_t)p16 & ~0x3));
  uint32_t pos = ((uintptr_t)p16 & 0x3) * 8;
  val >>= pos;
  return (uint16_t)val;
}

static inline int16_t mmu_get_int16(const short *p16) {
  ASSERT_RANGE_TEST_READ(p16);
  uint32_t val = (*(uint32_t *)((uintptr_t)p16 & ~0x3));
  uint32_t pos = ((uintptr_t)p16 & 0x3) * 8;
  val >>= pos;
  return (int16_t)val;
}

static inline uint8_t mmu_set_uint8(void *p8, const uint8_t val) {
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

static inline uint16_t mmu_set_uint16(unsigned short *p16, const uint16_t val) {
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

static inline int16_t mmu_set_int16(short *p16, const int16_t val) {
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
