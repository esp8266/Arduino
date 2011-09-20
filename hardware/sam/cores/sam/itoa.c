/*
 %atmel_license%
*/

#include "itoa.h"
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#if 0
/* reverse:  reverse string s in place */
static void reverse( char s[] )
{
  int i, j ;
  char c ;

  for ( i = 0, j = strlen(s)-1 ; i < j ; i++, j-- )
  {
    c = s[i] ;
    s[i] = s[j] ;
    s[j] = c ;
  }
}

/* itoa:  convert n to characters in s */
extern void itoa( int n, char s[] )
{
  int i, sign ;

  if ( (sign = n) < 0 )  /* record sign */
  {
    n = -n;          /* make n positive */
  }

  i = 0;
  do
  {       /* generate digits in reverse order */
    s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0) ;     /* delete it */

  if (sign < 0 )
  {
    s[i++] = '-';
  }

  s[i] = '\0';

  reverse( s ) ;
}

#else

extern char* itoa( int value, char *string, int radix )
{
  return ltoa( value, string, radix ) ;
}

extern char* ltoa( long value, char *string, int radix )
{
  char tmp[33];
  char *tp = tmp;
  long i;
  unsigned long v;
  int sign;
  char *sp;

  if ( string == NULL )
  {
    return 0 ;
  }

  if (radix > 36 || radix <= 1)
  {
    return 0 ;
  }

  sign = (radix == 10 && value < 0);
  if (sign)
  {
    v = -value;
  }
  else
  {
    v = (unsigned long)value;
  }

  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'a' - 10;
  }

  sp = string;

  if (sign)
    *sp++ = '-';
  while (tp > tmp)
    *sp++ = *--tp;
  *sp = 0;

  return string;
}

extern char* utoa( unsigned long value, char *string, int radix )
{
  return ultoa( value, string, radix ) ;
}

extern char* ultoa( unsigned long value, char *string, int radix )
{
  char tmp[33];
  char *tp = tmp;
  long i;
  unsigned long v = value;
  char *sp;

  if ( string == NULL )
  {
    return 0;
  }

  if (radix > 36 || radix <= 1)
  {
    return 0;
  }
 
  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'a' - 10;
  }

  sp = string;

 
  while (tp > tmp)
    *sp++ = *--tp;
  *sp = 0;

  return string;
}
#endif /* 0 */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
