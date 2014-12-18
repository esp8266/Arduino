/* 
  core_esp8266_noniso.c - nonstandard (but usefull) conversion functions

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

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
