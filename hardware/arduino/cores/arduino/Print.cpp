/*
 Print.cpp - Base class that provides print() and println()
 Copyright (c) 2008 David A. Mellis.  All right reserved.
 
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
 
 Modified 23 November 2006 by David A. Mellis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Arduino.h"

#include "Print.h"

// Public Methods //////////////////////////////////////////////////////////////

/* default implementation: may be overridden */
long Print::write(const char *str)
{
  long n = 0;
  while (*str) {
    if (write(*str++) <= 0) break;
    n++;
  }
  return n;
}

/* default implementation: may be overridden */
long Print::write(const uint8_t *buffer, size_t size)
{
  long n = 0;
  while (size--) {
    if (write(*buffer++) <= 0) break;
    n++;
  }
  return n;
}

long Print::print(const __FlashStringHelper *ifsh)
{
  const prog_char *p = (const prog_char *)ifsh;
  long n = 0;
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    if (write(c) <= 0) break;
    n++;
  }
  return n;
}

long Print::print(const String &s)
{
  long n = 0;
  for (int i = 0; i < s.length(); i++) {
    if (write(s[i]) < 0) break;
    n++;
  }
  return n;
}

long Print::print(const char str[])
{
  return write(str);
}

long Print::print(char c)
{
  return write(c);
}

long Print::print(unsigned char b, int base)
{
  return print((unsigned long) b, base);
}

long Print::print(int n, int base)
{
  return print((long) n, base);
}

long Print::print(unsigned int n, int base)
{
  return print((unsigned long) n, base);
}

long Print::print(long n, int base)
{
  if (base == 0) {
    return write(n);
  } else if (base == 10) {
    if (n < 0) {
      long t = print('-');
      if (t <= 0) return t;
      n = -n;
      return printNumber(n, 10) + 1;
    }
    return printNumber(n, 10);
  } else {
    return printNumber(n, base);
  }
}

long Print::print(unsigned long n, int base)
{
  if (base == 0) return write(n);
  else return printNumber(n, base);
}

long Print::print(double n, int digits)
{
  return printFloat(n, digits);
}

long Print::println(const __FlashStringHelper *ifsh)
{
  long n = print(ifsh);
  if (n >= 0) n += println();
  return n;
}

long Print::print(const Printable& x)
{
  return x.printTo(*this);
}

long Print::println(void)
{
  long t = print('\r');
  if (t <= 0) return t;
  if (print('\n') <= 0) return 1;
  return 2;
}

long Print::println(const String &s)
{
  long n = print(s);
  if (n >= 0) n += println();
  return n;
}

long Print::println(const char c[])
{
  long n = print(c);
  if (n >= 0) n += println();
  return n;
}

long Print::println(char c)
{
  long n = print(c);
  if (n > 0) n += println();
  return n;
}

long Print::println(unsigned char b, int base)
{
  long n = print(b, base);
  if (n >= 0) n += println();
  return n;
}

long Print::println(int num, int base)
{
  long n = print(num, base);
  if (n >= 0) n += println();
  return n;
}

long Print::println(unsigned int num, int base)
{
  long n = print(num, base);
  if (n >= 0) n += println();
  return n;
}

long Print::println(long num, int base)
{
  long n = print(num, base);
  if (n >= 0) n += println();
  return n;
}

long Print::println(unsigned long num, int base)
{
  long n = print(num, base);
  if (n >= 0) n += println();
  return n;
}

long Print::println(double num, int digits)
{
  long n = print(num, digits);
  if (n >= 0) n += println();
  return n;
}

long Print::println(const Printable& x)
{
  long n = print(x);
  if (n >= 0) n += println();
  return n;
}

// Private Methods /////////////////////////////////////////////////////////////

long Print::printNumber(unsigned long n, uint8_t base) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return write(str);
}

long Print::printFloat(double number, uint8_t digits) 
{ 
  long n = 0, t;
  
  // Handle negative numbers
  if (number < 0.0)
  {
     if ((n = print('-')) <= 0) return n;
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  if ((t = print(int_part)) < 0) return n;
  
  n += t;

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    t = print("."); 
    if (t <= 0) return n;
    n += t;
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    t = print(toPrint);
    if (t <= 0) return n;
    n += t;
    remainder -= toPrint; 
  } 
  
  return n;
}
