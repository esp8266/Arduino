/*! \file rprintf.c \brief printf routine and associated routines. */
//*****************************************************************************
//
// File Name	: 'rprintf.c'
// Title		: printf routine and associated routines
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 2000.12.26
// Revised		: 2003.5.1
// Version		: 1.0
// Target MCU	: Atmel AVR series and other targets
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/pgmspace.h>
//#include <string-avr.h>
//#include <stdlib.h>
#include <stdarg.h>
#include "global.h"
#include "rprintf.h"

#ifndef TRUE
	#define TRUE	-1
	#define FALSE	0
#endif

#define INF     32766	// maximum field size to print
#define READMEMBYTE(a,char_ptr)	((a)?(pgm_read_byte(char_ptr)):(*char_ptr))

#ifdef RPRINTF_COMPLEX
	static unsigned char buf[128];
#endif

// use this to store hex conversion in RAM
//static char HexChars[] = "0123456789ABCDEF";
// use this to store hex conversion in program memory
//static prog_char HexChars[] = "0123456789ABCDEF";
static char __attribute__ ((progmem)) HexChars[] = "0123456789ABCDEF";

// function pointer to single character output routine
static void (*rputchar)(unsigned char c);

// *** rprintf initialization ***
// you must call this function once and supply the character output
// routine before using other functions in this library
void rprintfInit(void (*putchar_func)(unsigned char c))
{
	rputchar = putchar_func;
}

// *** rprintfChar ***
// send a character/byte to the current output device
inline void rprintfChar(unsigned char c)
{
	// send character
	rputchar(c);
}

// *** rprintfStr ***
// prints a null-terminated string stored in RAM
void rprintfStr(char str[])
{
	// send a string stored in RAM
	// check to make sure we have a good pointer
	if (!str) return;

	// print the string until a null-terminator
	while (*str)
		rprintfChar(*str++);
}

// *** rprintfStrLen ***
// prints a section of a string stored in RAM
// begins printing at position indicated by <start>
// prints number of characters indicated by <len>
void rprintfStrLen(char str[], unsigned int start, unsigned int len)
{
	register int i=0;

	// check to make sure we have a good pointer
	if (!str) return;
	// spin through characters up to requested start
	// keep going as long as there's no null
	while((i++<start) && (*str++));
//	for(i=0; i<start; i++)
//	{
//		// keep steping through string as long as there's no null
//		if(*str) str++;
//	}

	// then print exactly len characters
	for(i=0; i<len; i++)
	{
		// print data out of the string as long as we haven't reached a null yet
		// at the null, start printing spaces
		if(*str)
			rprintfChar(*str++);
		else
			rprintfChar(' ');
	}

}

// *** rprintfProgStr ***
// prints a null-terminated string stored in program ROM
void rprintfProgStr(const prog_char str[])
{
	// print a string stored in program memory
	register char c;

	// check to make sure we have a good pointer
	if (!str) return;
	
	// print the string until the null-terminator
	while((c = pgm_read_byte(str++)))
		rprintfChar(c);
}

// *** rprintfCRLF ***
// prints carriage return and line feed
void rprintfCRLF(void)
{
	// print CR/LF
	rprintfChar('\r');
	rprintfChar('\n');
}

// *** rprintfu04 ***
// prints an unsigned 4-bit number in hex (1 digit)
void rprintfu04(unsigned char data)
{
	// print 4-bit hex value
//	char Character = data&0x0f;
//	if (Character>9)
//		Character+='A'-10;
//	else
//		Character+='0';
	rprintfChar(pgm_read_byte( HexChars+(data&0x0f) ));
}

// *** rprintfu08 ***
// prints an unsigned 8-bit number in hex (2 digits)
void rprintfu08(unsigned char data)
{
	// print 8-bit hex value
	rprintfu04(data>>4);
	rprintfu04(data);
}

// *** rprintfu16 ***
// prints an unsigned 16-bit number in hex (4 digits)
void rprintfu16(unsigned short data)
{
	// print 16-bit hex value
	rprintfu08(data>>8);
	rprintfu08(data);
}

// *** rprintfu32 ***
// prints an unsigned 32-bit number in hex (8 digits)
void rprintfu32(unsigned long data)
{
	// print 32-bit hex value
	rprintfu16(data>>16);
	rprintfu16(data);
}

// *** rprintfNum ***
// special printf for numbers only
// see formatting information below
//	Print the number "n" in the given "base"
//	using exactly "numDigits"
//	print +/- if signed flag "isSigned" is TRUE
//	use the character specified in "padchar" to pad extra characters
//
//	Examples:
//	uartPrintfNum(10, 6,  TRUE, ' ',   1234);  -->  " +1234"
//	uartPrintfNum(10, 6, FALSE, '0',   1234);  -->  "001234"
//	uartPrintfNum(16, 6, FALSE, '.', 0x5AA5);  -->  "..5AA5"
void rprintfNum(char base, char numDigits, char isSigned, char padchar, long n)
{
	// define a global HexChars or use line below
	//static char HexChars[16] = "0123456789ABCDEF";
	char *p, buf[32];
	unsigned long x;
	unsigned char count;

	// prepare negative number
	if( isSigned && (n < 0) )
	{
		x = -n;
	}
	else
	{
	 	x = n;
	}

	// setup little string buffer
	count = (numDigits-1)-(isSigned?1:0);
  	p = buf + sizeof (buf);
  	*--p = '\0';
	
	// force calculation of first digit
	// (to prevent zero from not printing at all!!!)
	*--p = pgm_read_byte(HexChars + (x%base)); x /= base;
	// calculate remaining digits
	while(count--)
	{
		if(x != 0)
		{
			// calculate next digit
			*--p = pgm_read_byte(HexChars + (x%base)); x /= base;
		}
		else
		{
			// no more digits left, pad out to desired length
			*--p = padchar;
		}
	}

	// apply signed notation if requested
	if( isSigned )
	{
		if(n < 0)
		{
   			*--p = '-';
		}
		else if(n > 0)
		{
	   		*--p = '+';
		}
		else
		{
	   		*--p = ' ';
		}
	}

	// print the string right-justified
	count = numDigits;
	while(count--)
	{
		rprintfChar(*p++);
	}
}

#ifdef RPRINTF_FLOAT
// *** rprintfFloat ***
// floating-point print
void rprintfFloat(char numDigits, double x)
{
	unsigned char firstplace = FALSE;
	unsigned char negative;
	unsigned char i, digit;
	double place = 1.0;
	
	// save sign
	negative = (x<0);
	// convert to absolute value
	x = (x>0)?(x):(-x);
	
	// find starting digit place
	for(i=0; i<15; i++)
	{
		if((x/place) < 10.0)
			break;
		else
			place *= 10.0;
	}
	// print polarity character
	if(negative)
		rprintfChar('-');
	else
		rprintfChar('+');

	// print digits
	for(i=0; i<numDigits; i++)
	{
		digit = (x/place);

		if(digit | firstplace | (place == 1.0))
		{
			firstplace = TRUE;
			rprintfChar(digit+0x30);
		}
		else
			rprintfChar(' ');
		
		if(place == 1.0)
		{
			rprintfChar('.');
		}
		
		x -= (digit*place);
		place /= 10.0;
	}
}
#endif

#ifdef RPRINTF_SIMPLE
// *** rprintf1RamRom ***
//! called by rprintf() - does a simple printf (supports %d, %x, %c)
// Supports:
// %d - decimal
// %x - hex
// %c - character
int rprintf1RamRom(unsigned char stringInRom, const char *format, ...)
{
	// simple printf routine
	// define a global HexChars or use line below
	//static char HexChars[16] = "0123456789ABCDEF";
	char format_flag;
	unsigned int u_val, div_val, base;
	va_list ap;

	va_start(ap, format);
	for (;;)
	{
		while ((format_flag = READMEMBYTE(stringInRom,format++) ) != '%')
		{	// Until '%' or '\0'
			if (!format_flag)
			{
				va_end(ap);
				return(0);
			}
			rprintfChar(format_flag);
		}

		switch (format_flag = READMEMBYTE(stringInRom,format++) )
		{
			case 'c': format_flag = va_arg(ap,int);
			default:  rprintfChar(format_flag); continue;
			case 'd': base = 10; div_val = 10000; goto CONVERSION_LOOP;
			case 'x': base = 16; div_val = 0x10;

			CONVERSION_LOOP:
			u_val = va_arg(ap,int);
			if (format_flag == 'd')
			{
				if (((int)u_val) < 0)
				{
					u_val = - u_val;
					rprintfChar('-');
				}
				while (div_val > 1 && div_val > u_val) div_val /= 10;
			}
			do
			{
				rprintfChar(pgm_read_byte(HexChars+(u_val/div_val)));
				u_val %= div_val;
				div_val /= base;
			} while (div_val);
		}
	}
	va_end(ap);
}
#endif


#ifdef RPRINTF_COMPLEX
// *** rprintf2RamRom ***
//! called by rprintf() - does a more powerful printf (supports %d, %u, %o, %x, %c, %s)
// Supports:
// %d - decimal
// %u - unsigned decimal
// %o - octal
// %x - hex
// %c - character
// %s - strings
// and the width,precision,padding modifiers
// **this printf does not support floating point numbers
int rprintf2RamRom(unsigned char stringInRom, const char *sfmt, ...)
{
	register unsigned char *f, *bp;
	register long l;
	register unsigned long u;
	register int i;
	register int fmt;
	register unsigned char pad = ' ';
	int flush_left = 0, f_width = 0, prec = INF, hash = 0, do_long = 0;
	int sign = 0;

	va_list ap;
	va_start(ap, sfmt);

	f = (unsigned char *) sfmt;

	for (; READMEMBYTE(stringInRom,f); f++)
	{
		if (READMEMBYTE(stringInRom,f) != '%')
		{	// not a format character
			// then just output the char
			rprintfChar(READMEMBYTE(stringInRom,f));
		}
		else 
		{
			f++;						// if we have a "%" then skip it
			if (READMEMBYTE(stringInRom,f) == '-')
			{
				flush_left = 1;	// minus: flush left
				f++;
			}
            if (READMEMBYTE(stringInRom,f) == '0'
				 || READMEMBYTE(stringInRom,f) == '.')
				{
					// padding with 0 rather than blank
					pad = '0';
					f++;
            }
            if (READMEMBYTE(stringInRom,f) == '*')
				{	// field width
					f_width = va_arg(ap, int);
					f++;
            }
            else if (Isdigit(READMEMBYTE(stringInRom,f)))
				{
					f_width = atoiRamRom(stringInRom, (char *) f);
					while (Isdigit(READMEMBYTE(stringInRom,f)))
						f++;        // skip the digits
            }
            if (READMEMBYTE(stringInRom,f) == '.')
				{	// precision
					f++;
					if (READMEMBYTE(stringInRom,f) == '*')
					{
						prec = va_arg(ap, int);
						f++;
					}
					else if (Isdigit(READMEMBYTE(stringInRom,f)))
					{
						prec = atoiRamRom(stringInRom, (char *) f);
						while (Isdigit(READMEMBYTE(stringInRom,f)))
							f++;    // skip the digits
					}
				}
            if (READMEMBYTE(stringInRom,f) == '#')
				{	// alternate form
					hash = 1;
					f++;
            }
            if (READMEMBYTE(stringInRom,f) == 'l')
				{	// long format
					do_long = 1;
					f++;
            }

				fmt = READMEMBYTE(stringInRom,f);
				bp = buf;
				switch (fmt) {		// do the formatting
				case 'd':			// 'd' signed decimal
					if (do_long)
						l = va_arg(ap, long);
					else
						l = (long) (va_arg(ap, int));
					if (l < 0)
					{
						sign = 1;
						l = -l;
					}
					do	{
						*bp++ = l % 10 + '0';
					} while ((l /= 10) > 0);
					if (sign)
						*bp++ = '-';
					f_width = f_width - (bp - buf);
					if (!flush_left)
						while (f_width-- > 0)
							rprintfChar(pad);
					for (bp--; bp >= buf; bp--)
						rprintfChar(*bp);
					if (flush_left)
						while (f_width-- > 0)
							rprintfChar(' ');
					break;
            case 'o':			// 'o' octal number
            case 'x':			// 'x' hex number
            case 'u':			// 'u' unsigned decimal
					if (do_long)
						u = va_arg(ap, unsigned long);
					else
						u = (unsigned long) (va_arg(ap, unsigned));
					if (fmt == 'u')
					{	// unsigned decimal
						do {
							*bp++ = u % 10 + '0';
						} while ((u /= 10) > 0);
					}
					else if (fmt == 'o')
					{  // octal
						do {
							*bp++ = u % 8 + '0';
						} while ((u /= 8) > 0);
						if (hash)
							*bp++ = '0';
					}
					else if (fmt == 'x')
					{	// hex
						do {
							i = u % 16;
							if (i < 10)
								*bp++ = i + '0';
							else
								*bp++ = i - 10 + 'a';
						} while ((u /= 16) > 0);
						if (hash)
						{
							*bp++ = 'x';
							*bp++ = '0';
						}
					}
					i = f_width - (bp - buf);
					if (!flush_left)
						while (i-- > 0)
							rprintfChar(pad);
					for (bp--; bp >= buf; bp--)
						rprintfChar((int) (*bp));
					if (flush_left)
						while (i-- > 0)
							rprintfChar(' ');
					break;
            case 'c':			// 'c' character
					i = va_arg(ap, int);
					rprintfChar((int) (i));
					break;
            case 's':			// 's' string
					bp = va_arg(ap, unsigned char *);
					if (!bp)
						bp = (unsigned char *) "(nil)";
					f_width = f_width - strlen((char *) bp);
					if (!flush_left)
						while (f_width-- > 0)
							rprintfChar(pad);
					for (i = 0; *bp && i < prec; i++)
					{
						rprintfChar(*bp);
						bp++;
					}
					if (flush_left)
						while (f_width-- > 0)
							rprintfChar(' ');
					break;
            case '%':			// '%' character
					rprintfChar('%');
					break;
			}
			flush_left = 0, f_width = 0, prec = INF, hash = 0, do_long = 0;
			sign = 0;
			pad = ' ';
		}
	}

	va_end(ap);
	return 0;
}

unsigned char Isdigit(char c)
{
	if((c >= 0x30) && (c <= 0x39))
		return TRUE;
	else
		return FALSE;
}

int atoiRamRom(unsigned char stringInRom, char *str)
{
	int num = 0;;

	while(Isdigit(READMEMBYTE(stringInRom,str)))
	{
		num *= 10;
		num += ((READMEMBYTE(stringInRom,str++)) - 0x30);
	}
	return num;
}

#endif

//******************************************************************************
// code below this line is commented out and can be ignored
//******************************************************************************
/*
char* sprintf(const char *sfmt, ...)
{
	register unsigned char *f, *bp, *str;
	register long l;
	register unsigned long u;
	register int i;
	register int fmt;
	register unsigned char pad = ' ';
	int     flush_left = 0, f_width = 0, prec = INF, hash = 0, do_long = 0;
	int     sign = 0;

	va_list ap;
	va_start(ap, sfmt);

	str = bufstring;
	f = (unsigned char *) sfmt;

	for (; *f; f++)
	{
		if (*f != '%')
		{								// not a format character
			*str++ = (*f);			// then just output the char
		}
		else 
		{
			f++;						// if we have a "%" then skip it
			if (*f == '-')
			{
				flush_left = 1;	// minus: flush left
				f++;
			}
            if (*f == '0' || *f == '.')
				{
					// padding with 0 rather than blank
					pad = '0';
					f++;
            }
            if (*f == '*')
				{	// field width
					f_width = va_arg(ap, int);
					f++;
            }
            else if (Isdigit(*f))
				{
					f_width = atoi((char *) f);
					while (Isdigit(*f))
						f++;        // skip the digits
            }
            if (*f == '.')
				{	// precision
					f++;
					if (*f == '*')
					{
						prec = va_arg(ap, int);
						f++;
					}
					else if (Isdigit(*f))
					{
						prec = atoi((char *) f);
						while (Isdigit(*f))
							f++;    // skip the digits
					}
				}
            if (*f == '#')
				{	// alternate form
					hash = 1;
					f++;
            }
            if (*f == 'l')
				{	// long format
					do_long = 1;
					f++;
            }

				fmt = *f;
				bp = buf;
				switch (fmt) {		// do the formatting
				case 'd':			// 'd' signed decimal
					if (do_long)
						l = va_arg(ap, long);
					else
						l = (long) (va_arg(ap, int));
					if (l < 0)
					{
						sign = 1;
						l = -l;
					}
					do	{
						*bp++ = l % 10 + '0';
					} while ((l /= 10) > 0);
					if (sign)
						*bp++ = '-';
					f_width = f_width - (bp - buf);
					if (!flush_left)
						while (f_width-- > 0)
							*str++ = (pad);
					for (bp--; bp >= buf; bp--)
						*str++ = (*bp);
					if (flush_left)
						while (f_width-- > 0)
							*str++ = (' ');
					break;
            case 'o':			// 'o' octal number
            case 'x':			// 'x' hex number
            case 'u':			// 'u' unsigned decimal
					if (do_long)
						u = va_arg(ap, unsigned long);
					else
						u = (unsigned long) (va_arg(ap, unsigned));
					if (fmt == 'u')
					{	// unsigned decimal
						do {
							*bp++ = u % 10 + '0';
						} while ((u /= 10) > 0);
					}
					else if (fmt == 'o')
					{  // octal
						do {
							*bp++ = u % 8 + '0';
						} while ((u /= 8) > 0);
						if (hash)
							*bp++ = '0';
					}
					else if (fmt == 'x')
					{	// hex
						do {
							i = u % 16;
							if (i < 10)
								*bp++ = i + '0';
							else
								*bp++ = i - 10 + 'a';
						} while ((u /= 16) > 0);
						if (hash)
						{
							*bp++ = 'x';
							*bp++ = '0';
						}
					}
					i = f_width - (bp - buf);
					if (!flush_left)
						while (i-- > 0)
							*str++ = (pad);
					for (bp--; bp >= buf; bp--)
						*str++ = ((int) (*bp));
					if (flush_left)
						while (i-- > 0)
							*str++ = (' ');
					break;
            case 'c':			// 'c' character
					i = va_arg(ap, int);
					*str++ = ((int) (i));
					break;
            case 's':			// 's' string
					bp = va_arg(ap, unsigned char *);
					if (!bp)
						bp = (unsigned char *) "(nil)";
					f_width = f_width - strlen((char *) bp);
					if (!flush_left)
						while (f_width-- > 0)
							*str++ = (pad);
					for (i = 0; *bp && i < prec; i++)
					{
						*str++ = (*bp);
						bp++;
					}
					if (flush_left)
						while (f_width-- > 0)
							*str++ = (' ');
					break;
            case '%':			// '%' character
					*str++ = ('%');
					break;
			}
			flush_left = 0, f_width = 0, prec = INF, hash = 0, do_long = 0;
			sign = 0;
			pad = ' ';
		}
	}

	va_end(ap);
	// terminate string with null
	*str++ = '\0';
	return bufstring;
}

*/
