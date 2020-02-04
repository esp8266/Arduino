
/*
 core_esp8266_features.cpp

 Copyright (c) 2019 Mike Nix. All rights reserved.
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

#include <stdint.h>
#include <c_types.h>

/* precache()
 *  pre-loads flash data into the flash cache
 *  if f==0, preloads instructions starting at the address we were called from.
 *  otherwise preloads flash at the given address.
 *  All preloads are word aligned.
 */
#ifdef __cplusplus
extern "C" {
#endif

void precache(void *f, uint32_t bytes) {
  // Size of a cache page in bytes. We only need to read one word per
  // page (ie 1 word in 8) for this to work.
  #define CACHE_PAGE_SIZE 32

  register uint32_t a0 asm("a0");
  register uint32_t lines = (bytes/CACHE_PAGE_SIZE)+2;
  volatile uint32_t *p = (uint32_t*)((f ? (uint32_t)f : a0) & ~0x03);
  uint32_t x;
  for (uint32_t i=0; i<lines; i++, p+=CACHE_PAGE_SIZE/sizeof(uint32_t)) x=*p;
  (void)x;
}



int Cache_Read_Enable_status = -1;

#if (ICACHE_SIZE == 0x4000)
#define SOC_CACHE_SIZE 0 // 16KB
// #define SOC_CACHE_SIZE 1 // 32KB

#pragma message("ICACHE size 16K")


#ifndef ROM_Cache_Read_Enable
#define ROM_Cache_Read_Enable         0x40004678
#endif

typedef void (*fp_Cache_Read_Enable_t)(uint32_t map, uint32_t p, uint32_t v);
constexpr fp_Cache_Read_Enable_t real_Cache_Read_Enable = (fp_Cache_Read_Enable_t)ROM_Cache_Read_Enable;
/*
 * Override SDK's ICACHE size
 */
void IRAM_ATTR Cache_Read_Enable(uint32_t map, uint32_t p, uint32_t v) {
  (void)v;
  real_Cache_Read_Enable(map, p, SOC_CACHE_SIZE);
  Cache_Read_Enable_status = SOC_CACHE_SIZE;
}
#endif

#ifdef __cplusplus
}
#endif
