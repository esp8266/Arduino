
#ifndef __GLUE_DEBUG_H
#define __GLUE_DEBUG_H

// this file is commonly included by both sides of the glue
/////////////////////////////////////////////////////////////////////////////
// user-definable

// this is needed separately from lwipopts.h
// because it is shared by both sides of glue

#define UNDEBUG		1	// 0 or 1 (1: uassert removed = saves flash)
#define UDEBUG		0	// 0 or 1 (glue debug)
#define UDUMP		0	// 0 or 1 (glue: dump packet)

#define ULWIPDEBUG	0	// 0 or 1 (trigger lwip debug)
#define ULWIPASSERT	0	// 0 or 1 (trigger lwip self-check, 0 saves flash)

#if ARDUINO
#define STRING_IN_FLASH 1	// *print("fmt is stored in flash")
#else
#define STRING_IN_FLASH 0	// *print("fmt is stored in flash")
#endif

#if ULWIPDEBUG
//#define LWIP_DBG_TYPES_ON	(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)
#define LWIP_DBG_TYPES_ON	(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)
//#define LWIP_DBG_TYPES_ON	(LWIP_DBG_ON)
#endif

/////////////////////////////////////////////////////////////////////////////
// packet capture callback from esp side
#include <stdlib.h>

#define HAS_PHY_CAPTURE 1
#ifdef __cplusplus
extern "C" {
#endif
extern void (*phy_capture) (int netif_idx, const char* data, size_t len, int out, int success);
#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////
#if ARDUINO
#include <sys/pgmspace.h>
#endif

// print definitions:
// uprint(): always used by glue, defined as doprint() in debug mode or nothing()
// os_printf(): can be redefined as doprint()
// doprint(): always print, can be os_printf_plus() or defined in doprint.c (buffered)
//	in buffered mode: doprint_allow=1 is needed after Serial.begin

#if STRING_IN_FLASH && !defined(USE_OPTIMIZE_PRINTF)
#define USE_OPTIMIZE_PRINTF	// at least used in arduino/esp8266
#endif

#include <osapi.h> // os_printf* definitions + ICACHE_RODATA_ATTR

#if defined(ARDUINO)
// os_printf() does not understand ("%hhx",0x12345678) => "78") and prints 'h' instead
// now hacking/using ::printf() from updated and patched esp-quick-toolchain-by-Earle
#include <stdio.h>
#undef os_printf
#undef os_printf_plus
#define os_printf printf
#define os_printf_plus printf
#endif

#if UDEBUG
#define uprint(x...)		do { os_printf(x); } while (0)
#else
#define uprint(x...)		do { (void)0; } while (0)
#endif

#if ARDUINO
#define udoassert(assertion...)	\
do { if ((assertion) == 0) { \
		static const char assrt[] ICACHE_RODATA_ATTR STORE_ATTR = #assertion " wrong@"; \
		os_printf_plus(assrt); \
		static const char assrt_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__; \
		os_printf_plus(assrt_file); \
		static const char assrt_line[] ICACHE_RODATA_ATTR STORE_ATTR = ":%d\n"; \
		os_printf_plus(assrt_line, __LINE__); \
		uhalt(); \
} } while (0)
#else
#define udoassert(assertion...)   do { if ((assertion) == 0) { os_printf("assert fail: " #assertion " @%s:%d\n", __FILE__, __LINE__); uhalt(); } } while (0)
#endif

#if UNDEBUG
#define uassert(assertion...)	do { (void)0; } while (0)
#else // !defined(UNDEBUG)
#define uassert(assertion...)	udoassert(assertion)
#endif // !defined(UNDEBUG)

#define ualwaysassert(assertion...)	udoassert(assertion)

#define uerror(x...)		do { os_printf(x); } while (0)
#define uhalt() 		do { *((int*)0) = 0; /* this triggers gdb */ } while (0)
#define nl()			do { uprint("\n"); } while (0)

#endif
