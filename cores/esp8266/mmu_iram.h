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

#ifdef DEBUG_ESP_CORE
#endif
#define DEBUG_MMU

#ifndef assert
#define assert(...) do{}while(false)
#endif

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
  constexpr uint32_t iram_start = 0x40100000u;

#ifdef MMU_SEC_HEAP_SIZE
  constexpr uint32_t iram_end = iram_start + MMU_IRAM_SIZE + MMU_SEC_HEAP_SIZE;
#else
  constexpr uint32_t iram_end = iram_start + MMU_IRAM_SIZE;
#endif

  return (iram_start <= addr && iram_end > addr);
}

static inline bool is_icache(uint32_t addr) {
  constexpr uint32_t icache_start = 0x40200000u;
  constexpr uint32_t icache_end = icache_start + 0x100000;
  return (icache_start <= addr && icache_end > addr);
}

#else
static inline bool is_iram(uint32_t addr) {
  (void)addr;
  return true;
}

static inline bool is_icache(uint32_t addr) {
  (void)addr;
  return true;
}
#endif  // #ifdef DEBUG_MMU


/*
 * Some inlines to allow faster random access to non32bit access of iRAM or
 * iCACHE data elements. These remove the extra time and stack space that would
 * have occured by relying on exception processing.
 */
static inline uint8_t get_uint8_iram(const void *p8) {
  assert((is_iram((uint32_t)p8) || is_icache((uint32_t)p8)));
  uint32_t val = (*(uint32_t *)((uintptr_t)p8 & ~0x3));
  uint32_t pos = ((uintptr_t)p8 & 0x3) * 8;
  val >>= pos;
  return (uint8_t)val;
}

static inline uint16_t get_uint16_iram(const void *p16) {
  assert((is_iram((uint32_t)p16) || is_icache((uint32_t)p16)));
  uint32_t val = (*(uint32_t *)((uintptr_t)p16 & ~0x3));
  uint32_t pos = ((uintptr_t)p16 & 0x3) * 8;
  val >>= pos;
  return (uint16_t)val;
}

static inline int16_t get_int16_iram(const void *p16) {
  assert((is_iram((uint32_t)p16) || is_icache((uint32_t)p16)));
  uint32_t val = (*(uint32_t *)((uintptr_t)p16 & ~0x3));
  uint32_t pos = ((uintptr_t)p16 & 0x3) * 8;
  val >>= pos;
  return (int16_t)val;
}



static inline uint8_t set_uint8_iram(void *p8, const uint8_t val) {
  assert((is_iram((uint32_t)p8)));
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

static inline uint16_t set_uint16_iram(void *p16, const uint16_t val) {
  assert((is_iram((uint32_t)p16)));
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

static inline int16_t set_int16_iram(void *p16, const int16_t val) {
  assert((is_iram((uint32_t)p16)));
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



#if DEV_DEBUG
#define DBG_MMU_PRINTF ets_uart_printf

#define ETS_FLUSH(a) while((USS(a) >> USTXC) & 0xff){}

#define DBG_MM_PRINT_STATUS() { \
    DBG_MMU_PRINTF("\nmmu_status = {" \
                   "v_cfg = %u, state = %d, enable/disable count = %u/%u, " \
                   "map = 0x%02X, p = 0x%02X, v = 0x%02X}\n", \
                   mmu_status.v_cfg, mmu_status.state, \
                   mmu_status.enable_count, mmu_status.disable_count, \
                   mmu_status.map, mmu_status.p, mmu_status.v); \
    ETS_FLUSH(0); \
}

#define DBG_MMU_PRINT_IRAM_BANK_REG(a) { \
    uint32_t iram_bank_reg = ESP8266_DREG(0x24); \
    DBG_MMU_PRINTF("\niram_bank_reg %s%s0x%08X\n", (0 == a) ? "" : a, (0 == a) ? "" : " Cache_Read_Enable ", iram_bank_reg); \
}

#else
#define ETS_FLUSH(...) do {} while(false)
#define DBG_MMU_PRINTF(...) do {} while(false)
#define DBG_MM_PRINT_STATUS(...) do {} while(false)
#define DBG_MMU_PRINT_IRAM_BANK_REG(...) do {} while(false)
#endif    // #if DEV_DEBUG


#ifdef __cplusplus
}
#endif

#endif
