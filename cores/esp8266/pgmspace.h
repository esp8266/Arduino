#ifndef __PGMSPACE_H_
#define __PGMSPACE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#ifdef __cplusplus
}
#endif

#define PROGMEM     ICACHE_RODATA_ATTR
#define PGM_P  		const char *
#define PGM_VOID_P  const void *
#define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))


#define _SFR_BYTE(n) (n)

typedef void prog_void;
typedef char prog_char;
typedef unsigned char prog_uchar;
typedef int8_t prog_int8_t;
typedef uint8_t prog_uint8_t;
typedef int16_t prog_int16_t;
typedef uint16_t prog_uint16_t;
typedef int32_t prog_int32_t;
typedef uint32_t prog_uint32_t;

#define SIZE_IRRELEVANT 0x7fffffff

void* memcpy_P(void* dest, const void* src, size_t count);

char* strncpy_P(char* dest, const char* src, size_t size);
#define strcpy_P(dest, src)          strncpy_P((dest), (src), SIZE_IRRELEVANT)

char* strncat_P(char* dest, const char* src, size_t size);
#define strcat_P(dest, src)          strncat_P((dest), (src), SIZE_IRRELEVANT)

int strncmp_P(const char* str1, const char* str2P, size_t size);
#define strcmp_P(str1, str2P)          strncmp_P((str1), (str2P), SIZE_IRRELEVANT)

int strncasecmp_P(const char* str1, const char* str2P, size_t size);
#define strcasecmp_P(str1, str2P)          strncasecmp_P((str1), (str2P), SIZE_IRRELEVANT)

size_t strnlen_P(const char *s, size_t size);
#define strlen_P(strP)          strnlen_P((strP), SIZE_IRRELEVANT)

int	printf_P(const char *formatP, ...) __attribute__ ((format (printf, 1, 2)));
int	snprintf_P(char *str, size_t strSize, const char *formatP, ...) __attribute__ ((format (printf, 3, 4)));
int	vsnprintf_P(char *str, size_t strSize, const char *formatP, va_list ap) __attribute__ ((format (printf, 3, 0)));

// flash memory must be read using 32 bit aligned addresses else a processor
// exception will be triggered
// order within the 32 bit values are
// --------------
// b3, b2, b1, b0
//     w1,     w0

#define pgm_read_byte(addr) 		                                           \
(__extension__({                                                               \
    PGM_P __local = (PGM_P)(addr);  /* isolate varible for macro expansion */         \
    ptrdiff_t __offset = ((uint32_t)__local & 0x00000003); /* byte aligned mask */            \
    const uint32_t* __addr32 = reinterpret_cast<const uint32_t*>(reinterpret_cast<const uint8_t*>(__local)-__offset);   \
    uint8_t __result = ((*__addr32) >> (__offset * 8));                        \
    __result;                                                                  \
}))

#define pgm_read_word(addr) 		                                           \
(__extension__({                                                               \
    PGM_P __local = (PGM_P)(addr); /* isolate varible for macro expansion */          \
    ptrdiff_t __offset = ((uint32_t)__local & 0x00000002);   /* word aligned mask */          \
    const uint32_t* __addr32 = reinterpret_cast<const uint32_t*>(reinterpret_cast<const uint8_t*>(__local) - __offset); \
    uint16_t __result = ((*__addr32) >> (__offset * 8));                       \
    __result;                                                                  \
}))	

#define pgm_read_dword(addr) 		(*reinterpret_cast<const uint32_t*>(addr))
#define pgm_read_float(addr) 		(*reinterpret_cast<const float*>(addr))

#define pgm_read_byte_near(addr) 	pgm_read_byte(addr)
#define pgm_read_word_near(addr) 	pgm_read_word(addr)
#define pgm_read_dword_near(addr) 	pgm_read_dword(addr)
#define pgm_read_float_near(addr)	pgm_read_float(addr)
#define pgm_read_byte_far(addr) 	pgm_read_byte(addr)
#define pgm_read_word_far(addr) 	pgm_read_word(addr)
#define pgm_read_dword_far(addr) 	pgm_read_dword(addr)
#define pgm_read_float_far(addr) 	pgm_read_float(addr)

#endif //__PGMSPACE_H_
