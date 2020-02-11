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
bool inline is_iram(uint32_t addr) {
  constexpr uint32_t iram_start = 0x40100000u;
  constexpr uint32_t iram_end = iram_start + MMU_IRAM_SIZE + MMU_SEC_HEAP_SIZE;
  return (iram_start <= addr && iram_end > addr);
}

bool inline is_icache(uint32_t addr) {
  constexpr uint32_t icache_start = 0x40200000u;
  constexpr uint32_t icache_end = icache_start + 0x100000;
  return (icache_start <= addr && icache_end > addr);
}

#else
bool inline is_iram(uint32_t addr) {
  (void)addr;
  return true;
}

bool inline is_icache(uint32_t addr) {
  (void)addr;
  return true;
}
#endif

#if DEV_DEBUG
#define DBG_MMU_PRINTF ets_uart_printf

#define DBG_MM_PRINT_STATUS() { \
    DBG_MMU_PRINTF("\nmmu_status = {" \
                   "v_cfg = %u, state = %d, enable/disable count = %u/%u, " \
                   "map = 0x%02X, p = 0x%02X, v = 0x%02X}\n", \
                   mmu_status.v_cfg, mmu_status.state, \
                   mmu_status.enable_count, mmu_status.disable_count, \
                   mmu_status.map, mmu_status.p, mmu_status.v); \
    ets_delay_us(12000); \
}

#define DBG_MMU_PRINT_IRAM_BANK_REG(a) { \
    uint32_t iram_bank_reg = ESP8266_DREG(0x24); \
    DBG_MMU_PRINTF("\niram_bank_reg %s%s0x%08X\n", (0 == a) ? "" : a, (0 == a) ? "" : " Cache_Read_Enable ", iram_bank_reg); \
}

#else
#define DBG_MMU_PRINTF(...) do {} while(false)
#define DBG_MM_PRINT_STATUS(...) do {} while(false)
#define DBG_MMU_PRINT_IRAM_BANK_REG(...) do {} while(false)
#endif


#ifdef __cplusplus
}
#endif
#endif
