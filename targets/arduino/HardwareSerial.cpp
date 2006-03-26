/*
  HarwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

extern "C" {
  #include <stdio.h>
  #include <string.h>
  #include <inttypes.h>
  #include "Serial.h"
}

#include "HardwareSerial.h"

// Constructors ////////////////////////////////////////////////////////////////

HardwareSerial::HardwareSerial(uint8_t uart)
{
  if(uart == 0){
    _uart = 0;
  }else{
    _uart = 1;
  }
}

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::begin(long speed)
{
  uart_init(_uart, speed);
}

uint8_t HardwareSerial::available(void)
{
  return uart_available(_uart);
}

int HardwareSerial::read(void)
{
  return uart_read(_uart);
}

void HardwareSerial::print(char c)
{
  uart_write(_uart, &c, 1);
}

void HardwareSerial::print(char c[])
{
  uart_write(_uart, c, strlen(c));
}

void HardwareSerial::print(uint8_t b)
{
  char c = b;
  uart_write(_uart, &c, 1);
}

void HardwareSerial::print(int n)
{
  print((long) n);
}

void HardwareSerial::print(long n)
{
  if (n < 0) {
    print('-');
    n = -n;
  }
  printNumber(n, 10);
}

void HardwareSerial::print(unsigned long n)
{
  printNumber(n, 10);
}

void HardwareSerial::print(long n, int base)
{
  if (base == 0)
    print((char) n);
  else if (base == 10)
    print(n);
  else
    printNumber(n, base);
}

void HardwareSerial::println(void)
{
  print('\r');
  print('\n');  
}

void HardwareSerial::println(char c)
{
  print(c);
  println();  
}

void HardwareSerial::println(char c[])
{
  uart_write(_uart, c, strlen(c));
  println();
}

void HardwareSerial::println(uint8_t b)
{
  print(b);
  println();
}

void HardwareSerial::println(int n)
{
  println((long) n);
}

void HardwareSerial::println(long n)
{
  print(n);
  println();  
}

void HardwareSerial::println(unsigned long n)
{
  print(n);
  println();  
}

void HardwareSerial::println(long n, int base)
{
  print(n, base);
  println();
}

// Private Methods /////////////////////////////////////////////////////////////

void HardwareSerial::printNumber(unsigned long n, uint8_t base)
{
  uint8_t buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
  int i = 0;
  if (n == 0) {
    print('0');
    return;
  }
  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }
  for (i--; i >= 0; i--){
    print((char)(buf[i] < 10 ? '0' + buf[i] : 'A' + buf[i] - 10));
  }
}

// Preinstantiate Objects //////////////////////////////////////////////////////

HardwareSerial Serial = HardwareSerial(0);
//HardwareSerial Serial1 = HardwareSerial(1);

