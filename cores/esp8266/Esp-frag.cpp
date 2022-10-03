/*
 Esp.cpp - ESP8266-specific APIs
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "umm_malloc/umm_malloc.h"
#include "umm_malloc/umm_malloc_cfg.h"
#include "coredecls.h"
#include "Esp.h"

#if defined(UMM_INFO) || defined(UMM_INFO_EMPTY)
void EspClass::getHeapStats(uint32_t* hfree, uint32_t* hmax, uint8_t* hfrag)
{
    // L2 / Euclidean norm of free block sizes.
    // Having getFreeHeap()=sum(hole-size), fragmentation is given by
    // 100 * (1 - sqrt(sum(hole-size²)) / sum(hole-size))
    umm_info(NULL, false);

    uint32_t free_size = umm_free_heap_size_core(umm_get_current_heap());
    if (hfree)
        *hfree = free_size;
    if (hmax)
        *hmax = umm_max_block_size_core(umm_get_current_heap());
    if (hfrag) {
      if (free_size) {
        *hfrag = umm_fragmentation_metric_core(umm_get_current_heap());
      } else {
        *hfrag = 0;
      }
    }
}

void EspClass::getHeapStats(uint32_t* hfree, uint16_t* hmax, uint8_t* hfrag)
{
    uint32_t hmax32;
    getHeapStats(hfree, &hmax32, hfrag);
    if (hmax) {
      // With the MMU_EXTERNAL_HEAP option, hmax could overflow for heaps larger
      // then 64KB. return UINT16_MAX (saturation) for those cases.
      // Added deprecated attribute and message.
      *hmax = (hmax32 > UINT16_MAX) ? UINT16_MAX : hmax32;
    }
}

uint8_t EspClass::getHeapFragmentation()
{
  return (uint8_t)umm_fragmentation_metric();
}
#endif
