/*
  Serial.c - Serial library for Wiring
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

#include "Serial.h"
#include "uart.h"

void uart_init(uint8_t uart, long baudrate)
{
	uartInit();
	uartSetBaudRate(baudrate);
}

int uart_read(uint8_t uart)
{
	return uartGetByte();
}

uint8_t uart_available(uint8_t uart)
{
	return uartGetRxBuffer()->datalength;
}

void uart_write(uint8_t uart, char *buf, uint8_t len)
{
  int i;
  for (i = 0; i < len; i++)
    uartSendByte(buf[i]);
}
