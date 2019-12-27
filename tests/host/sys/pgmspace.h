/* PGMSPACE.H - Accessor utilities/types for accessing PROGMEM data */

#ifndef _PGMSPACE_H_
#define _PGMSPACE_H_

// These are no-ops in anything but the ESP8266, where they are defined in
// a custom sys/pgmspace.h header

#ifndef ICACHE_RODATA_ATTR
#define ICACHE_RODATA_ATTR
#endif

#ifndef PROGMEM
#define PROGMEM
#endif

#ifndef PGM_P
#define PGM_P const char *
#endif

#ifndef PGM_VOID_P
#define PGM_VOID_P const void *
#endif

#ifndef PSTR
#define PSTR
#endif

#ifdef __cplusplus
    #define pgm_read_byte(addr)             (*reinterpret_cast<const uint8_t*>(addr))
    #define pgm_read_word(addr)             (*reinterpret_cast<const uint16_t*>(addr))
    #define pgm_read_dword(addr)            (*reinterpret_cast<const uint32_t*>(addr))
    #define pgm_read_float(addr)            (*reinterpret_cast<const float>(addr))
    #define pgm_read_ptr(addr)              (*reinterpret_cast<const void* const *>(addr))
#else
    #define pgm_read_byte(addr)             (*(const uint8_t*)(addr))
    #define pgm_read_word(addr)             (*(const uint16_t*)(addr))
    #define pgm_read_dword(addr)            (*(const uint32_t*)(addr))
    #define pgm_read_float(addr)            (*(const float)(addr))
    #define pgm_read_ptr(addr)              (*(const void* const *)(addr))
#endif

#define pgm_read_byte_near(addr)        pgm_read_byte(addr)
#define pgm_read_word_near(addr)        pgm_read_word(addr)
#define pgm_read_dword_near(addr)       pgm_read_dword(addr)
#define pgm_read_float_near(addr)       pgm_read_float(addr)
#define pgm_read_ptr_near(addr)         pgm_read_ptr(addr)
#define pgm_read_byte_far(addr)         pgm_read_byte(addr)
#define pgm_read_word_far(addr)         pgm_read_word(addr)
#define pgm_read_dword_far(addr)        pgm_read_dword(addr)
#define pgm_read_float_far(addr)        pgm_read_float(addr)
#define pgm_read_ptr_far(addr)          pgm_read_ptr(addr)

// Wrapper inlines for _P functions
#include <stdio.h>
#include <string.h>
inline const char *strstr_P(const char *haystack, const char *needle) { return strstr(haystack, needle); }
inline char *strcpy_P(char *dest, const char *src) { return strcpy(dest, src); }
inline size_t strlen_P(const char *s) { return strlen(s); }
inline int vsnprintf_P(char *str, size_t size, const char *format, va_list ap) { return vsnprintf(str, size, format, ap); }

#define memcpy_P memcpy
#define memmove_P memmove
#define strncpy_P strncpy
#define strcmp_P strcmp
#define memccpy_P memccpy
#define snprintf_P snprintf
#define sprintf_P sprintf
#define strncmp_P strncmp

#endif
