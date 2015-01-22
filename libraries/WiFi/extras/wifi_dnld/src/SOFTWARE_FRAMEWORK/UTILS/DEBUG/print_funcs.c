/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Strings and integers print module for debug purposes.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USART module can be used.
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#include "compiler.h"
#include "gpio.h"
#include "usart.h"
#include "print_funcs.h"


//! ASCII representation of hexadecimal digits.
static const char HEX_DIGITS[16] = "0123456789ABCDEF";


void init_dbg_rs232(long pba_hz)
{
  init_dbg_rs232_ex(DBG_USART_BAUDRATE, pba_hz);
}


void init_dbg_rs232_ex(unsigned long baudrate, long pba_hz)
{
  static const gpio_map_t DBG_USART_GPIO_MAP =
  {
    {DBG_USART_RX_PIN, DBG_USART_RX_FUNCTION},
    {DBG_USART_TX_PIN, DBG_USART_TX_FUNCTION}
  };

  // Options for debug USART.
  usart_options_t dbg_usart_options =
  {
    .baudrate = baudrate,
    .charlength = 8,
    .paritytype = USART_NO_PARITY,
    .stopbits = USART_1_STOPBIT,
    .channelmode = USART_NORMAL_CHMODE
  };

  // Setup GPIO for debug USART.
  gpio_enable_module(DBG_USART_GPIO_MAP,
                     sizeof(DBG_USART_GPIO_MAP) / sizeof(DBG_USART_GPIO_MAP[0]));

  // Initialize it in RS232 mode.
  usart_init_rs232(DBG_USART, &dbg_usart_options, pba_hz);
}


void print_dbg(const char *str)
{
  // Redirection to the debug USART.
  print(DBG_USART, str);
}


void print_dbg_char(int c)
{
  // Redirection to the debug USART.
  print_char(DBG_USART, c);
}


void print_dbg_ulong(unsigned long n)
{
  // Redirection to the debug USART.
  print_ulong(DBG_USART, n);
}


void print_dbg_char_hex(unsigned char n)
{
  // Redirection to the debug USART.
  print_char_hex(DBG_USART, n);
}


void print_dbg_short_hex(unsigned short n)
{
  // Redirection to the debug USART.
  print_short_hex(DBG_USART, n);
}


void print_dbg_hex(unsigned long n)
{
  // Redirection to the debug USART.
  print_hex(DBG_USART, n);
}


void print(volatile avr32_usart_t *usart, const char *str)
{
  // Invoke the USART driver to transmit the input string with the given USART.
  usart_write_line(usart, str);
}


void print_char(volatile avr32_usart_t *usart, int c)
{
  // Invoke the USART driver to transmit the input character with the given USART.
  usart_putchar(usart, c);
}


void print_ulong(volatile avr32_usart_t *usart, unsigned long n)
{
  char tmp[11];
  int i = sizeof(tmp) - 1;

  // Convert the given number to an ASCII decimal representation.
  tmp[i] = '\0';
  do
  {
    tmp[--i] = '0' + n % 10;
    n /= 10;
  } while (n);

  // Transmit the resulting string with the given USART.
  print(usart, tmp + i);
}


void print_char_hex(volatile avr32_usart_t *usart, unsigned char n)
{
  char tmp[3];
  int i;

  // Convert the given number to an ASCII hexadecimal representation.
  tmp[2] = '\0';
  for (i = 1; i >= 0; i--)
  {
    tmp[i] = HEX_DIGITS[n & 0xF];
    n >>= 4;
  }

  // Transmit the resulting string with the given USART.
  print(usart, tmp);
}


void print_short_hex(volatile avr32_usart_t *usart, unsigned short n)
{
  char tmp[5];
  int i;

  // Convert the given number to an ASCII hexadecimal representation.
  tmp[4] = '\0';
  for (i = 3; i >= 0; i--)
  {
    tmp[i] = HEX_DIGITS[n & 0xF];
    n >>= 4;
  }

  // Transmit the resulting string with the given USART.
  print(usart, tmp);
}


void print_hex(volatile avr32_usart_t *usart, unsigned long n)
{
  char tmp[9];
  int i;

  // Convert the given number to an ASCII hexadecimal representation.
  tmp[8] = '\0';
  for (i = 7; i >= 0; i--)
  {
    tmp[i] = HEX_DIGITS[n & 0xF];
    n >>= 4;
  }

  // Transmit the resulting string with the given USART.
  print(usart, tmp);
}
