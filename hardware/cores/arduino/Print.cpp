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

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "wiring.h"

#include "Print.h"

// Public Methods //////////////////////////////////////////////////////////////

void Print::print(uint8_t b)
{
  this->write(b);
}

void Print::print(char c)
{
  print((byte) c);
}

void Print::print(const char c[])
{
  while (*c)
    print(*c++);
}

void Print::print(int n)
{
  print((long) n);
}

void Print::print(unsigned int n)
{
  print((unsigned long) n);
}

void Print::print(long n)
{
  if (n < 0) {
    print('-');
    n = -n;
  }
  printNumber(n, 10);
}

void Print::print(unsigned long n)
{
  printNumber(n, 10);
}

void Print::print(long n, int base)
{
  if (base == 0)
    print((char) n);
  else if (base == 10)
    print(n);
  else
    printNumber(n, base);
}

void Print::println(void)
{
  print('\r');
  print('\n');  
}

void Print::println(char c)
{
  print(c);
  println();  
}

void Print::println(const char c[])
{
  print(c);
  println();
}

void Print::println(uint8_t b)
{
  print(b);
  println();
}

void Print::println(int n)
{
  print(n);
  println();
}

void Print::println(unsigned int n)
{
  print(n);
  println();
}

void Print::println(long n)
{
  print(n);
  println();  
}

void Print::println(unsigned long n)
{
  print(n);
  println();  
}

void Print::println(long n, int base)
{
  print(n, base);
  println();
}

// Private Methods /////////////////////////////////////////////////////////////

void Print::printNumber(unsigned long n, uint8_t base)
{
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
  unsigned long i = 0;

  if (n == 0) {
    print('0');
    return;
  } 

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    print((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}
