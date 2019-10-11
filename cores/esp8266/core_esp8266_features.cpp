
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

#ifdef __cplusplus
}
#endif
