#ifndef __PGMSPACE_H_
#define __PGMSPACE_H_

#include <stdint.h>
#include <stdio.h>

#define PROGMEM
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


int memcmp_P(const void* buf1, PGM_VOID_P buf2P, size_t size);
// memccpy_P is only valid when used with pointers to 8bit data, due to size aligned pointers
// and endianess of the values greater than 8bit, matching c may return invalid aligned pointers
void* memccpy_P(void* dest, PGM_VOID_P src, int c, size_t count);
void* memmem_P(const void* buf, size_t bufSize, PGM_VOID_P findP, size_t findPSize);
void* memcpy_P(void* dest, PGM_VOID_P src, size_t count);

#define strncpy_P(dest, src, size)   strncpy(dest, src, size)
#define strcpy_P(dest, src)          strncpy_P((dest), (src), SIZE_IRRELEVANT)

#define strncat_P(dest, src, size)   strncat(dest, src, size)
#define strcat_P(dest, src)          strncat_P((dest), (src), SIZE_IRRELEVANT)

#define strncmp_P(str1, str2P, size)   strncmp(str1, str2P, size)
#define strcmp_P(str1, str2P)          strncmp_P((str1), (str2P), SIZE_IRRELEVANT)

#define strncasecmp_P(str1, str2P, size)   strncasecmp(str1, str2P, size)
#define strcasecmp_P(str1, str2P)          strncasecmp_P((str1), (str2P), SIZE_IRRELEVANT)

#define strnlen_P(s, size)      strnlen(s, size)
#define strlen_P(strP)          strnlen_P((strP), SIZE_IRRELEVANT)

#define printf_P(formatP, ...) printf(formatP, __VA_ARGS__)
#define sprintf_P(str, formatP, ...) sprintf(str, formatP, __VA_ARGS__)
#define snprintf_P(str, strSize, formatP, ...) snprintf(str, strSize, formatP, __VA_ARGS__)
#define	vsnprintf_P(str, strSize, formatP, ap) vsnprintf(str, strSize, formatP, ap)

// flash memory must be read using 32 bit aligned addresses else a processor
// exception will be triggered
// order within the 32 bit values are
// --------------
// b3, b2, b1, b0
//     w1,     w0

#define pgm_read_byte(x) (*reinterpret_cast<const uint8_t*>(x))
#define pgm_read_word(x) (*reinterpret_cast<const uint16_t*>(x))
#define pgm_read_float(x) (*reinterpret_cast<const float*>(x))
#define pgm_read_dword(x) (*reinterpret_cast<const uint32_t*>(x))

#define pgm_read_byte_near(addr) 	pgm_read_byte(addr)
#define pgm_read_word_near(addr) 	pgm_read_word(addr)
#define pgm_read_dword_near(addr) 	pgm_read_dword(addr)
#define pgm_read_float_near(addr)	pgm_read_float(addr)
#define pgm_read_byte_far(addr) 	pgm_read_byte(addr)
#define pgm_read_word_far(addr) 	pgm_read_word(addr)
#define pgm_read_dword_far(addr) 	pgm_read_dword(addr)
#define pgm_read_float_far(addr) 	pgm_read_float(addr)

#endif //__PGMSPACE_H_
