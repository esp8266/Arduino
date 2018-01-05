/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __MEM_H__
#define __MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Note: check_memleak_debug_enable is a weak function inside SDK.
 * please copy following codes to user_main.c.
#include "mem.h"

bool ICACHE_FLASH_ATTR check_memleak_debug_enable(void)
{
    return MEMLEAK_DEBUG_ENABLE;
}
*/

#ifndef MEMLEAK_DEBUG
#define MEMLEAK_DEBUG_ENABLE	0
#define os_free(s)        vPortFree(s, "", 0)
#define os_malloc(s)      pvPortMalloc(s, "", 0)
#define os_calloc(s)      pvPortCalloc(s, "", 0);
#define os_realloc(p, s)  pvPortRealloc(p, s, "", 0)
#define os_zalloc(s)      pvPortZalloc(s, "", 0)
#else
#define MEMLEAK_DEBUG_ENABLE	1

#define os_free(s) \
do{\
	static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
    vPortFree(s, mem_debug_file, __LINE__);\
}while(0)

#define os_malloc(s)	\
	({	\
		static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
		pvPortMalloc(s, mem_debug_file, __LINE__);	\
	})

#define os_calloc(s)	\
	({	\
		static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
		pvPortCalloc(s, mem_debug_file, __LINE__);	\
	})

#define os_realloc(p, s)	\
	({	\
		static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
		pvPortRealloc(p, s, mem_debug_file, __LINE__);	\
	})

#define os_zalloc(s)	\
	({	\
		static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
		pvPortZalloc(s, mem_debug_file, __LINE__);	\
	})

#endif


#ifdef __cplusplus
}
#endif

#endif

