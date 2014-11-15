#include <stdlib.h>
#include "stdlib_noniso.h"

char *   itoa (int val, char *s, int radix)
{
  *s = 0;
  return s;
}

char *   ltoa (long val, char *s, int radix)
{
  *s = 0;
  return s;
}
 
char *  utoa (unsigned int val, char *s, int radix)
{
  *s = 0;
  return s;
}
 
char *  ultoa (unsigned long val, char *s, int radix)
{
  *s = 0;
  return s;
}

char *  dtostre (double __val, char *__s, unsigned char __prec, unsigned char __flags)
{
  *__s = 0;
  return __s;
}
 
char *  dtostrf (double __val, signed char __width, unsigned char __prec, char *__s)
{
   *__s = 0;
   return __s;
}
