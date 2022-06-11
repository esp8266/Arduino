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

// This turns on range checking.
#ifdef DEBUG_ESP_CORE
#define DEBUG_ESP_MMU
#endif

#if defined(CORE_MOCK)
#define ets_uart_printf(...) do {} while(false)
#define XCHAL_INSTRAM0_VADDR		0x40000000
#define XCHAL_INSTRAM1_VADDR		0x40100000
#define XCHAL_INSTROM0_VADDR		0x40200000
#else
#include <sys/config.h> // For config/core-isa.h
/*
  Cautiously use XCHAL_..._VADDR values where possible.
  While XCHAL_..._VADDR values in core-isa.h may define the Xtensa processor
  CONFIG options, they are not always an indication of DRAM, IRAM, or ROM
  size or position in the address space.
*/
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

/*
 * This wrapper is for running code from IROM (flash) before the SDK starts.
 *
 * Wraps a `void fn(void)` call with calls to enable and disable iCACHE.
 * Allows a function that resides in IROM to run before the SDK starts.
 *
 * Do not use once the SDK has started.
 *
 * Because the SDK initialization code has not run, nearly all the SDK functions
 * are not safe to call.
 *
 * Note printing at this early stage is complicated. To gain more insight,
 * review DEV_DEBUG_PRINT build path in mmu_iram.cpp. To handle strings stored
 * in IROM, review printing method and comments in hwdt_app_entry.cpp.
 *
 */
void IRAM_ATTR mmu_wrap_irom_fn(void (*fn)(void));

static inline __attribute__((always_inline))
bool mmu_is_iram(const void *addr) {
  const uintptr_t iram_start = (uintptr_t)XCHAL_INSTRAM1_VADDR;
#ifndef MMU_IRAM_SIZE
#if defined(__GNUC__) && !defined(CORE_MOCK)
  #warning "MMU_IRAM_SIZE was undefined, setting to 0x8000UL!"
#endif
  #define MMU_IRAM_SIZE 0x8000ul
#endif
  const uintptr_t iram_end = iram_start + MMU_IRAM_SIZE;

  return (iram_start <= (uintptr_t)addr && iram_end > (uintptr_t)addr);
}

static inline __attribute__((always_inline))
bool mmu_is_dram(const void *addr) {
  const uintptr_t dram_start = 0x3FFE8000ul;
  // The start of the Boot ROM sits at the end of DRAM. 0x40000000ul;
  const uintptr_t dram_end = (uintptr_t)XCHAL_INSTRAM0_VADDR;

  return (dram_start <= (uintptr_t)addr && dram_end > (uintptr_t)addr);
}

static inline __attribute__((always_inline))
bool mmu_is_icache(const void *addr) {
  extern void _irom0_text_end(void);
  const uintptr_t icache_start = (uintptr_t)XCHAL_INSTROM0_VADDR;
  const uintptr_t icache_end = (uintptr_t)_irom0_text_end;

  return (icache_start <= (uintptr_t)addr && icache_end > (uintptr_t)addr);
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
 * have occurred by relying on exception processing.
 */
static inline __attribute__((always_inline))
uint8_t mmu_get_uint8(const void *p8) {
  ASSERT_RANGE_TEST_READ(p8);
  // https://gist.github.com/shafik/848ae25ee209f698763cffee272a58f8#how-do-we-type-pun-correctly
  // Comply with strict-aliasing rules. Using memcpy is a Standards suggested
  // method for type punning. The compiler optimizer will replace the memcpy
  // with an `l32i` instruction.  Using __builtin_memcpy to ensure we get the
  // effects of the compiler optimization and not some #define version of
  // memcpy.
  void *v32 = (void *)((uintptr_t)p8 & ~(uintptr_t)3u);
  uint32_t val;
  __builtin_memcpy(&val, v32, sizeof(uint32_t));
  // Use an empty ASM to reference the 32-bit value. This will block the
  // compiler from immediately optimizing to an 8-bit or 16-bit load instruction
  // against IRAM memory. (This approach was inspired by
  // https://github.com/esp8266/Arduino/pull/7780#discussion_r548303374)
  // This issue was seen when using a constant address with the GCC 10.3
  // compiler.
  // As a general practice, I think referencing by way of Extended ASM R/W
  // output register will stop the the compiler from reloading the value later
  // as 8-bit load from IRAM.
  asm volatile ("" :"+r"(val)); // inject 32-bit dependency
  uint32_t pos = ((uintptr_t)p8 & 3u) * 8u;
  val >>= pos;
  return (uint8_t)val;
}

static inline __attribute__((always_inline))
uint16_t mmu_get_uint16(const uint16_t *p16) {
  ASSERT_RANGE_TEST_READ(p16);
  void *v32 = (void *)((uintptr_t)p16 & ~(uintptr_t)0x3u);
  uint32_t val;
  __builtin_memcpy(&val, v32, sizeof(uint32_t));
  asm volatile ("" :"+r"(val));
  uint32_t pos = ((uintptr_t)p16 & 3u) * 8u;
  val >>= pos;
  return (uint16_t)val;
}

static inline __attribute__((always_inline))
int16_t mmu_get_int16(const int16_t *p16) {
  ASSERT_RANGE_TEST_READ(p16);
  void *v32 = (void *)((uintptr_t)p16 & ~(uintptr_t)3u);
  uint32_t val;
  __builtin_memcpy(&val, v32, sizeof(uint32_t));
  asm volatile ("" :"+r"(val));
  uint32_t pos = ((uintptr_t)p16 & 3u) * 8u;
  val >>= pos;
  return (int16_t)val;
}

static inline __attribute__((always_inline))
uint8_t mmu_set_uint8(void *p8, const uint8_t val) {
  ASSERT_RANGE_TEST_WRITE(p8);
  uint32_t pos = ((uintptr_t)p8 & 3u) * 8u;
  uint32_t sval = val << pos;
  uint32_t valmask =  0x0FFu << pos;

  void *v32 = (void *)((uintptr_t)p8 & ~(uintptr_t)3u);
  uint32_t ival;
  __builtin_memcpy(&ival, v32, sizeof(uint32_t));
  asm volatile ("" :"+r"(ival));

  ival &= (~valmask);
  ival |= sval;
  /*
    This 32-bit dependency injection does not appear to be needed with the
    current GCC 10.3; however, that could change in the future versions. Or, I
    may not have the right test for it to fail.
  */
  asm volatile ("" :"+r"(ival));
  __builtin_memcpy(v32, &ival, sizeof(uint32_t));
  return val;
}

static inline __attribute__((always_inline))
uint16_t mmu_set_uint16(uint16_t *p16, const uint16_t val) {
  ASSERT_RANGE_TEST_WRITE(p16);
  uint32_t pos = ((uintptr_t)p16 & 3u) * 8u;
  uint32_t sval = val << pos;
  uint32_t valmask =  0x0FFFFu << pos;

  void *v32 = (void *)((uintptr_t)p16 & ~(uintptr_t)3u);
  uint32_t ival;
  __builtin_memcpy(&ival, v32, sizeof(uint32_t));
  asm volatile ("" :"+r"(ival));

  ival &= (~valmask);
  ival |= sval;
  asm volatile ("" :"+r"(ival));
  __builtin_memcpy(v32, &ival, sizeof(uint32_t));
  return val;
}

static inline __attribute__((always_inline))
int16_t mmu_set_int16(int16_t *p16, const int16_t val) {
  ASSERT_RANGE_TEST_WRITE(p16);
  uint32_t sval = (uint16_t)val;
  uint32_t pos = ((uintptr_t)p16 & 3u) * 8u;
  sval <<= pos;
  uint32_t valmask =  0x0FFFFu << pos;

  void *v32 = (void *)((uintptr_t)p16 & ~(uintptr_t)3u);
  uint32_t ival;
  __builtin_memcpy(&ival, v32, sizeof(uint32_t));
  asm volatile ("" :"+r"(ival));

  ival &= (~valmask);
  ival |= sval;
  asm volatile ("" :"+r"(ival));
  __builtin_memcpy(v32, &ival, sizeof(uint32_t));
  return val;
}

#if (MMU_IRAM_SIZE > 32*1024) && !defined(MMU_SEC_HEAP)
#define MMU_SEC_HEAP mmu_sec_heap()
#define MMU_SEC_HEAP_SIZE mmu_sec_heap_size()

static inline __attribute__((always_inline))
void *mmu_sec_heap(void) {
  extern void _text_end(void);
  uintptr_t sec_heap = (uintptr_t)_text_end + (uintptr_t)32u;
  return (void *)(sec_heap &= ~(uintptr_t)7u);
}

static inline __attribute__((always_inline))
size_t mmu_sec_heap_size(void) {
  return (size_t)0xC000ul - ((uintptr_t)mmu_sec_heap() - (uintptr_t)XCHAL_INSTRAM1_VADDR);
}
#endif

#ifdef __cplusplus
}
#endif

#endif
