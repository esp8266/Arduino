
#ifndef __GLUE_DEBUG_H
#define __GLUE_DEBUG_H

// this file is commonly included by both sides of the glue
/////////////////////////////////////////////////////////////////////////////
// user-definable

// this is needed separately from lwipopts.h
// because it is shared by both sides of glue

#define UNDEBUG		1	// 0 or 1 (1: uassert removed)
#define UDEBUG		0	// 0 or 1 (glue debug)
#define UDUMP		0	// 0 or 1 (glue / dump packet)
#define UDEBUGINDEX	0	// 0 or 1 (show debug line number)
#define UDEBUGSTORE	0	// 0 or 1 (store debug into buffer until doprint_allow=1=serial-available)

#define ULWIPDEBUG	0	// 0 or 1 (trigger lwip debug)
#define ULWIPASSERT	0	// 0 or 1 (trigger lwip self-check, 0 saves flash)

#if ARDUINO
#define STRING_IN_FLASH 1	// *print("fmt is stored in flash")
#else
#define STRING_IN_FLASH 0	// *print("fmt is stored in flash")
#endif

#define ROTBUFLEN_BIT	11	// (UDEBUGSTORE=1) doprint()'s buffer: 11=2048B

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
extern "C"
#endif
void (*phy_capture) (int netif_idx, const char* data, size_t len, int out, int success);

/////////////////////////////////////////////////////////////////////////////
#if ARDUINO
#include <sys/pgmspace.h>
#endif

#if UDEBUG && UDEBUGSTORE
#warning use 'doprint_allow=1' right after Serial is enabled
extern int doprint_allow;
#endif

// print definitions:
// uprint(): always used by glue, defined as doprint() in debug mode or nothing()
// os_printf(): can be redefined as doprint()
// doprint(): always print, can be os_printf_plus() or defined in doprint.c (buffered)
//	in buffered mode: doprint_allow=1 is needed after Serial.begin

#if STRING_IN_FLASH && !defined(USE_OPTIMIZE_PRINTF)
#define USE_OPTIMIZE_PRINTF	// at least used in arduino/esp8266
#endif
// os_printf_plus() missing in osapi.h (fixed in arduino's sdk-2.1):
//extern int os_printf_plus (const char * format, ...) __attribute__ ((format (printf, 1, 2)));

#include <osapi.h> // os_printf* definitions + ICACHE_RODATA_ATTR

#if UDEBUG && (UDEBUGINDEX || UDEBUGSTORE)
// doprint() is used

#undef	os_printf
#define	os_printf(x...)		do { doprint(x); } while (0)

#if STRING_IN_FLASH

#define doprint(fmt, ...) \
	do { \
		static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = fmt; \
		doprint_minus(flash_str, ##__VA_ARGS__); \
	} while(0)

#else // !STRING_IN_FLASH

#define doprint(fmt, ...) doprint_minus(fmt, ##__VA_ARGS__)

#endif // !STRING_IN_FLASH

int doprint_minus (const char* format, ...) __attribute__ ((format (printf, 1, 2))); // format in flash

#else // !( UDEBUG && (UDEBUGINDEX || UDEBUGSTORE) )

#define doprint(x...)		do { os_printf(x); } while (0)

#endif // !( UDEBUG && (UDEBUGINDEX || UDEBUGSTORE) )

#if UDEBUG
#define uprint(x...)		do { doprint(x); } while (0)
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

#define uerror(x...)		do { doprint(x); } while (0)
#define uhalt() 		do { *((int*)0) = 0; /* this triggers gdb */ } while (0)
#define nl()			do { uprint("\n"); } while (0)

#endif
