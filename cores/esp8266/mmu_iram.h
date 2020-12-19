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
#include <esp8266_undocumented.h>

#ifdef __cplusplus
extern "C" {
#endif

//C This turns on range checking. Is this the value you want to trigger it?
#ifdef DEBUG_ESP_CORE
#define DEBUG_ESP_MMU
#endif

#if defined(CORE_MOCK)
#define ets_uart_printf(...) do {} while(false)
#endif

/*
 * DEV_DEBUG_PRINT:
 *   Debug printing macros for printing before before, during, and after
 *   NONOS SDK initializes. May or maynot be safe during NONOS SDK
 *   initialization. As in printing from functions called on by the SDK
 *   during the SDK initialization.
 *
 #define DEV_DEBUG_PRINT
 */

#if defined(DEV_DEBUG_PRINT) || defined(DEBUG_ESP_MMU)
#include <esp8266_peri.h>

#define DBG_MMU_FLUSH(a) while((USS(a) >> USTXC) & 0xff) {}

#if defined(DEV_DEBUG_PRINT)
extern void set_pll(void);
extern void dbg_set_pll(void);

#define DBG_MMU_PRINTF(fmt, ...) \
set_pll(); \
uart_buff_switch(0); \
ets_uart_printf(fmt, ##__VA_ARGS__); \
DBG_MMU_FLUSH(0)

#else // ! defined(DEV_DEBUG_PRINT)
#define DBG_MMU_PRINTF(fmt, ...) ets_uart_printf(fmt, ##__VA_ARGS__)
#endif

#else     // ! defined(DEV_DEBUG_PRINT) || defined(DEBUG_ESP_MMU)
#define DBG_MMU_FLUSH(...) do {} while(false)
#define DBG_MMU_PRINTF(...) do {} while(false)
#endif    // defined(DEV_DEBUG_PRINT) || defined(DEBUG_ESP_MMU)

static inline __attribute__((always_inline))
bool mmu_is_iram(const void *addr) {
  #define IRAM_START 0x40100000UL
#ifndef MMU_IRAM_SIZE
#if defined(__GNUC__) && !defined(CORE_MOCK)
  #warning "MMU_IRAM_SIZE was undefined, setting to 0x8000UL!"
#endif
  #define MMU_IRAM_SIZE 0x8000UL
#endif
  #define IRAM_END (IRAM_START + MMU_IRAM_SIZE)

  return (IRAM_START <= (uintptr_t)addr && IRAM_END > (uintptr_t)addr);
}

static inline __attribute__((always_inline))
bool mmu_is_dram(const void *addr) {
  #define DRAM_START 0x3FF80000UL
  #define DRAM_END 0x40000000UL

  return (DRAM_START <= (uintptr_t)addr && DRAM_END > (uintptr_t)addr);
}

static inline __attribute__((always_inline))
bool mmu_is_icache(const void *addr) {
  #define ICACHE_START 0x40200000UL
  #define ICACHE_END (ICACHE_START + 0x100000UL)

  return (ICACHE_START <= (uintptr_t)addr && ICACHE_END > (uintptr_t)addr);
}

#ifdef DEBUG_ESP_MMU
#define ASSERT_RANGE_TEST_WRITE(a) \
  if (mmu_is_iram(a) || mmu_is_dram(a)) { \
  } else { \
    DBG_MMU_PRINTF("\nexcvaddr: %p\n", a); \
    assert(("Outside of Range - Write" && false)); \
  }

#define ASSERT_RANGE_TEST_READ(a) \
  if (mmu_is_iram(a) || mmu_is_dram(a) || mmu_is_icache(a)) { \
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
static inline __attribute__((always_inline))
uint8_t mmu_get_uint8(const void *p8) {
  ASSERT_RANGE_TEST_READ(p8);
  return *(uint32_t *)((uintptr_t)p8 & ~0x3) >> (uintptr_t)p8 * 8;
}

static inline __attribute__((always_inline))
uint16_t mmu_get_uint16(const uint16_t *p16) {
  ASSERT_RANGE_TEST_READ(p16);
  return *(uint32_t *)((uintptr_t)p16 & ~0x3) >> (uintptr_t)p16 * 8;
}

static inline __attribute__((always_inline))
int16_t mmu_get_int16(const int16_t *p16) {
  ASSERT_RANGE_TEST_READ(p16);
  return *(uint32_t *)((uintptr_t)p16 & ~0x3) >> (uintptr_t)p16 * 8;
}

static inline __attribute__((always_inline))
uint8_t mmu_set_uint8(void *p8, const uint8_t val) {
  ASSERT_RANGE_TEST_WRITE(p8);
  uint32_t *p32 = (uint32_t *)((uintptr_t)p8 & ~0x3);
  uint32_t ival = *p32;
  uint32_t valmask, sval;

  __asm__ ("ssa8b\t%2\n\t"
           "src\t%0, %3, %3\n\t"
           "src\t%1, %4, %4"
           : "=&a"(valmask), "=a"(sval)
           : "r"(p8), "r"(~0xFF), "r"(val), "r"(ival));
  *p32 = (ival & valmask) | sval;
  return val;
}

static inline __attribute__((always_inline))
uint16_t mmu_set_uint16(uint16_t *p16, const uint16_t val) {
  ASSERT_RANGE_TEST_WRITE(p16);
  uint32_t *p32 = (uint32_t *)((uintptr_t)p16 & ~0x3);
  uint32_t ival = *p32;
  uint32_t valmask, sval;

  __asm__ ("ssa8b\t%2\n\t"
           "src\t%0, %3, %3\n\t"
           "src\t%1, %4, %4"
           : "=&a"(valmask), "=a"(sval)
           : "r"(p16), "r"(~0xFFFF), "r"(val), "r"(ival));
  *p32 = (ival & valmask) | sval;
  return val;
}

static inline __attribute__((always_inline))
int16_t mmu_set_int16(int16_t *p16, const int16_t val) {
  ASSERT_RANGE_TEST_WRITE(p16);
  uint32_t *p32 = (uint32_t *)((uintptr_t)p16 & ~0x3);
  uint32_t ival = *p32;
  uint32_t valmask, sval;

  __asm__ ("ssa8b\t%2\n\t"
           "src\t%0, %3, %3\n\t"
           "src\t%1, %4, %4"
           : "=&a"(valmask), "=a"(sval)
           : "r"(p16), "r"(~0xFFFF), "r"(val & 0xFFFF), "r"(ival));
  *p32 = (ival & valmask) | sval;
  return val;
}

#if (MMU_IRAM_SIZE > 32*1024) && !defined(MMU_SEC_HEAP)
extern void _text_end(void);
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

#ifdef __cplusplus
}
#endif

#endif
