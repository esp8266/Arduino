
/*

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 
3. The name of the author may not be used to endorse or promote products 
derived from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.

author: d. gauchard

*/


#include "doprint.h"

#include <stdarg.h>
#include <osapi.h>
#include <stdint.h>
#include <mem.h>
#include <ets_sys.h>

extern int doprint_allow;

#if 0

// do not bufferize

int doprint (const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = ets_vprintf(ets_putc, format, ap);
	va_end(ap);
	return ret;
}

#else

// bufferize

#define ROTBUFLEN_BIT	10 // 11=2048
#define ROTBUFLEN	(1 << (ROTBUFLEN_BIT))
#define ROTBUFLEN_MASK	((ROTBUFLEN) - 1)

static int nl = 0;

static int rotin = 0;
static int rotout = 0;
static int rotsmall = 0;
static char* rotbuf = NULL;
static int bufputc (int c)
{
	rotbuf[rotout] = c;
	rotout = (rotout + 1) & ROTBUFLEN_MASK;
	if (rotout == rotin)
	{
		rotin = (rotin + 1) & ROTBUFLEN_MASK;
		rotsmall++;
	}
	return c;
}

//#define PUTC ets_putc		// no line number
#define PUTC nl_putc		// show line number

static int nl_putc (int c);

static int doprint_direct (const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = ets_vprintf(PUTC, format, ap);
	va_end(ap);
	return ret;
}

static int nl_putc (int c)
{
	ets_putc(c);
	if (c == '\n')
		doprint_direct("%d:", nl++);
	return c;
}

extern uint32_t millis (void);

#if STRING_IN_FLASH
// http://marrin.org/2017/01/16/putting-data-in-esp8266-flash-memory/
// https://github.com/cmarrin/m8rscript/blob/master/esp/core/Esp.{h,cpp}
static inline uint8_t readRomByte(const uint8_t* addr)
{
    uint32_t bytes;
    bytes = *(uint32_t*)((uint32_t)addr & ~3);
    return ((uint8_t*)&bytes)[(uint32_t)addr & 3];
}
size_t ROMstrlen(const char* s)
{
    const char* p;
    for (p = s; readRomByte((uint8_t*)p) != '\0'; p++) ;
    return (size_t) (p - s);
}
char* ROMCopyString(char* dst, const char* src)
{
    uint8_t* s = (uint8_t*) src;
    char c;
    while ((c = (char) readRomByte(s++))) {
        *dst++ = c;
    }
    *dst = '\0';
    return dst;
}
#endif

int doprint_minus (const char* minus_format, ...)
{
	int ret;
	int (*myputc)(int);
	
	if (doprint_allow)
	{
		if (rotbuf)
		{
			doprint_direct("\n<buffered:");
			if (rotsmall)
				doprint_direct("(%dcharslost):", rotsmall);
			PUTC('\n');
			for (; rotin != rotout; ret++, rotin = (rotin + 1) & ROTBUFLEN_MASK)
				PUTC(rotbuf[rotin]);
			doprint_direct(":dereffub>\n");
			os_free(rotbuf);
			rotbuf = NULL;
		}
		
		myputc = PUTC;
	}
	else if (!rotbuf && !(rotbuf = (char*)os_malloc(ROTBUFLEN)))
		return 0;
	else
		myputc = bufputc;

#if STRING_IN_FLASH
	size_t fmtlen = ROMstrlen(minus_format);
	char* format = os_malloc(fmtlen + 1);
	ROMCopyString(format, minus_format);
#else
#define format minus_format
#endif

	va_list ap;
	va_start(ap, minus_format);
	ret += ets_vprintf(myputc, format, ap);
	va_end(ap);

#if STRING_IN_FLASH
	os_free(format);
#endif
	
	return ret;
}

#endif
