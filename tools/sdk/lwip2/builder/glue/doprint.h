
#ifndef DOPRINT_H
#define DOPRINT_H

// os_printf/ets_printf works only with Serial.setDebugOutput(true)
// ets_putc always work (after Serial.begin())
// doprint uses ets_putc after doprint_allow gets true and bufferizes before that

#include <osapi.h> // ICACHE_RODATA_ATTR STORE_ATTR

extern int doprint_allow;

#define STRING_IN_FLASH 1

#if STRING_IN_FLASH
#define doprint(fmt, ...) \
	do { \
		static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = fmt; \
		doprint_minus(flash_str, ##__VA_ARGS__); \
	} while(0)
#else
#define doprint(fmt, ...) doprint_minus(fmt, ##__VA_ARGS__)
#endif

int doprint_minus (const char* format, ...) __attribute__ ((format (printf, 1, 2))); // format in flash

#endif // DOPRINT_H
