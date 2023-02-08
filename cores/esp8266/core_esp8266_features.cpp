
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

#include <eagle_soc.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* precache()
 *  pre-loads flash data into the flash cache
 *  if f==0, preloads instructions starting at the address we were called from.
 *  otherwise preloads flash at the given address.
 *  All preloads are word aligned.
 */
void precache(void *f, uint32_t bytes) {
  // Size of a cache page in bytes. We only need to read one word per
  // page (ie 1 word in 8) for this to work.
  #define CACHE_PAGE_SIZE 32

  uint32_t a0;
  __asm__("mov.n %0, a0" : "=r"(a0));
  uint32_t lines = (bytes/CACHE_PAGE_SIZE)+2;
  volatile uint32_t *p = (uint32_t*)((f ? (uint32_t)f : a0) & ~0x03);
  uint32_t x;
  for (uint32_t i=0; i<lines; i++, p+=CACHE_PAGE_SIZE/sizeof(uint32_t)) x=*p;
  (void)x;
}

/** based on efuse data, we could determine what type of chip this is
 * - https://github.com/espressif/esptool/blob/f04d34bcab29ace798d2d3800ba87020cccbbfdd/esptool.py#L1060-L1070
 * - https://github.com/espressif/ESP8266_RTOS_SDK/blob/3c055779e9793e5f082afff63a011d6615e73639/components/esp8266/include/esp8266/efuse_register.h#L20-L21
 */
bool esp_is_8285() {
    const uint32_t data[] {
        READ_PERI_REG(0x3ff00050), // aka MAC0
        READ_PERI_REG(0x3ff00058), // aka CHIPID
    };

    return ((data[0] & (1 << 4)) > 0)
        || ((data[1] & (1 << 16)) > 0);
}

#ifdef __cplusplus
}
#endif
