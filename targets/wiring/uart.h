/*
  uart.h - UART Serial library for Wiring
  Based on Hernando Barragan's original C implementation
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

#ifndef uart_h
#define uart_h

  #include <inttypes.h>
  
  #ifndef CPU_FREQ
  #define CPU_FREQ 16000000L
  #endif

  #ifndef UART_BUFFER_LENGTH
  #define UART_BUFFER_LENGTH 32
  #endif
  
  void uart_init(uint8_t, long);
  int uart_read(uint8_t);
  uint8_t uart_available(uint8_t);
  void uart_write(uint8_t, char*, uint8_t);

#endif

