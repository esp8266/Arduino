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
#include "ets_sys.h"

#define sprintf ets_sprintf
#define strcpy ets_strcpy

long atol_internal(const char* s)
{
  int result = 0;
  int i;
  const char* b = s;
  int sign = 1;
  for (i = 0; *b; ++i, ++b)
  {
    if (i == 0 && *b == '-')
      sign = -1;
    int x = *b - '0';
    if (x < 0 || x > 9)
      break;
    result = result * 10 + x;
  }
  return sign * result;
}

float atof_internal(const char* s)
{
  float result = 0;
  return result;
}


void reverse(char* begin, char* end)
{
  char *is = begin;
  char *ie = end - 1;
  while (is < ie)
  {
    char tmp = *ie;
    *ie = *is;
    *is = tmp;
    ++is;
    --ie;
  }
}

char* itoa( int value, char* result, int base ) 
{
  if (base < 2 || base > 16) 
  { 
    *result = 0;
    return result; 
  }

  char* out = result;
  int quotient = abs(value);

  do 
  {
    const int tmp = quotient / base;
    *out = "0123456789abcdef"[ quotient - (tmp*base) ];
    ++out;
    quotient = tmp;
  } while ( quotient );

  // Apply negative sign
  if ( value < 0) *out++ = '-';

  reverse(result, out);
  *out = 0;
  return result;
}

char* ltoa( long value, char* result, int base ) 
{
  if (base < 2 || base > 16) 
  { 
    *result = 0;
    return result; 
  }

  char* out = result;
  long quotient = abs(value);

  do 
  {
    const long tmp = quotient / base;
    *out = "0123456789abcdef"[ quotient - (tmp*base) ];
    ++out;
    quotient = tmp;
  } while ( quotient );

  // Apply negative sign
  if ( value < 0) *out++ = '-';

  reverse(result, out);
  *out = 0;
  return result;
}
 
char* utoa( unsigned value, char* result, int base ) 
{
  if (base < 2 || base > 16) 
  { 
    *result = 0;
    return result; 
  }

  char* out = result;
  unsigned quotient = value;

  do 
  {
    const unsigned tmp = quotient / base;
    *out = "0123456789abcdef"[ quotient - (tmp*base) ];
    ++out;
    quotient = tmp;
  } while ( quotient );

  reverse(result, out);
  *out = 0;
  return result;
}
 
char* ultoa( unsigned long value, char* result, int base ) 
{
  if (base < 2 || base > 16) 
  { 
    *result = 0;
    return result; 
  }

  char* out = result;
  unsigned long quotient = value;

  do 
  {
    const unsigned long tmp = quotient / base;
    *out = "0123456789abcdef"[ quotient - (tmp*base) ];
    ++out;
    quotient = tmp;
  } while ( quotient );

  reverse(result, out);
  *out = 0;
  return result;
}

char * dtostrf (double number, signed char width, unsigned char prec, char *s)
{
  size_t n = 0;
  
  if (isnan(number)) 
  {
    strcpy(s, "nan");
    return s;
  }
  if (isinf(number)) 
  {
    strcpy(s, "inf");
    return s;
  }

  if (number > 4294967040.0 || 
      number <-4294967040.0) 
  {
    strcpy(s, "ovf");
    return s;
  }
  char* out = s;
  // Handle negative numbers
  if (number < 0.0)
  {
     *out = '-';
     ++out;
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<prec; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  out += sprintf(out, "%d", int_part);

  // Print the decimal point, but only if there are digits beyond
  if (prec > 0) {
    *out = '.';
    ++out;
  }

  while (prec-- > 0)
  {
    remainder *= 10.0;
  }
  sprintf(out, "%d", (int)remainder);
  
  return s;
}
