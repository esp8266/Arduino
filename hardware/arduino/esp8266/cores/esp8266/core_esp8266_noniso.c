#include <stdlib.h>
#include "stdlib_noniso.h"


extern int ets_sprintf(char*, const char*, ...);

#define sprintf ets_sprintf

long atol_internal(const char* s)
{
  long result = 0;
  return result;
}

float atof_internal(const char* s)
{
  float result = 0;
  return result;
}

char * itoa (int val, char *s, int radix)
{
  // todo: radix
  sprintf(s, "%d", val);
  return s;
}

char * ltoa (long val, char *s, int radix)
{
  sprintf(s, "%ld", val);
  return s;
}
 
char * utoa (unsigned int val, char *s, int radix)
{
  sprintf(s, "%u", val);
  return s;
}
 
char * ultoa (unsigned long val, char *s, int radix)
{
  sprintf(s, "%lu", val);
  return s;
}

char * dtostre (double __val, char *__s, unsigned char __prec, unsigned char __flags)
{
  *__s = 0;
  return __s;
}
 
char * dtostrf (double __val, signed char __width, unsigned char __prec, char *__s)
{
   *__s = 0;
   return __s;
}
