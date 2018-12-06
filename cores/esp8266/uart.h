/*
 uart.h - UART HAL

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
#ifndef ESP_UART_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#if defined (__cplusplus)
extern "C" {
#endif

#define UART0    0
#define UART1    1
#define UART_NO -1

// Options for `config` argument of uart_init
#define UART_NB_BIT_MASK      0B00001100
#define UART_NB_BIT_5         0B00000000
#define UART_NB_BIT_6         0B00000100
#define UART_NB_BIT_7         0B00001000
#define UART_NB_BIT_8         0B00001100

#define UART_PARITY_MASK      0B00000011
#define UART_PARITY_NONE      0B00000000
#define UART_PARITY_EVEN      0B00000010
#define UART_PARITY_ODD       0B00000011

#define UART_NB_STOP_BIT_MASK 0B00110000
#define UART_NB_STOP_BIT_0    0B00000000
#define UART_NB_STOP_BIT_1    0B00010000
#define UART_NB_STOP_BIT_15   0B00100000
#define UART_NB_STOP_BIT_2    0B00110000

#define UART_5N1 ( UART_NB_BIT_5 | UART_PARITY_NONE | UART_NB_STOP_BIT_1 )
#define UART_6N1 ( UART_NB_BIT_6 | UART_PARITY_NONE | UART_NB_STOP_BIT_1 )
#define UART_7N1 ( UART_NB_BIT_7 | UART_PARITY_NONE | UART_NB_STOP_BIT_1 )
#define UART_8N1 ( UART_NB_BIT_8 | UART_PARITY_NONE | UART_NB_STOP_BIT_1 )
#define UART_5N2 ( UART_NB_BIT_5 | UART_PARITY_NONE | UART_NB_STOP_BIT_2 )
#define UART_6N2 ( UART_NB_BIT_6 | UART_PARITY_NONE | UART_NB_STOP_BIT_2 )
#define UART_7N2 ( UART_NB_BIT_7 | UART_PARITY_NONE | UART_NB_STOP_BIT_2 )
#define UART_8N2 ( UART_NB_BIT_8 | UART_PARITY_NONE | UART_NB_STOP_BIT_2 )
#define UART_5E1 ( UART_NB_BIT_5 | UART_PARITY_EVEN | UART_NB_STOP_BIT_1 )
#define UART_6E1 ( UART_NB_BIT_6 | UART_PARITY_EVEN | UART_NB_STOP_BIT_1 )
#define UART_7E1 ( UART_NB_BIT_7 | UART_PARITY_EVEN | UART_NB_STOP_BIT_1 )
#define UART_8E1 ( UART_NB_BIT_8 | UART_PARITY_EVEN | UART_NB_STOP_BIT_1 )
#define UART_5E2 ( UART_NB_BIT_5 | UART_PARITY_EVEN | UART_NB_STOP_BIT_2 )
#define UART_6E2 ( UART_NB_BIT_6 | UART_PARITY_EVEN | UART_NB_STOP_BIT_2 )
#define UART_7E2 ( UART_NB_BIT_7 | UART_PARITY_EVEN | UART_NB_STOP_BIT_2 )
#define UART_8E2 ( UART_NB_BIT_8 | UART_PARITY_EVEN | UART_NB_STOP_BIT_2 )
#define UART_5O1 ( UART_NB_BIT_5 | UART_PARITY_ODD  | UART_NB_STOP_BIT_1 )
#define UART_6O1 ( UART_NB_BIT_6 | UART_PARITY_ODD  | UART_NB_STOP_BIT_1 )
#define UART_7O1 ( UART_NB_BIT_7 | UART_PARITY_ODD  | UART_NB_STOP_BIT_1 )
#define UART_8O1 ( UART_NB_BIT_8 | UART_PARITY_ODD  | UART_NB_STOP_BIT_1 )
#define UART_5O2 ( UART_NB_BIT_5 | UART_PARITY_ODD  | UART_NB_STOP_BIT_2 )
#define UART_6O2 ( UART_NB_BIT_6 | UART_PARITY_ODD  | UART_NB_STOP_BIT_2 )
#define UART_7O2 ( UART_NB_BIT_7 | UART_PARITY_ODD  | UART_NB_STOP_BIT_2 )
#define UART_8O2 ( UART_NB_BIT_8 | UART_PARITY_ODD  | UART_NB_STOP_BIT_2 )

/*
#define UART_5N1 0x10
#define UART_6N1 0x14
#define UART_7N1 0x18
#define UART_8N1 0x1c
#define UART_5N2 0x30
#define UART_6N2 0x34
#define UART_7N2 0x38
#define UART_8N2 0x3c
#define UART_5E1 0x12
#define UART_6E1 0x16
#define UART_7E1 0x1a
#define UART_8E1 0x1e
#define UART_5E2 0x32
#define UART_6E2 0x36
#define UART_7E2 0x3a
#define UART_8E2 0x3e
#define UART_5O1 0x13
#define UART_6O1 0x17
#define UART_7O1 0x1b
#define UART_8O1 0x1f
#define UART_5O2 0x33
#define UART_6O2 0x37
#define UART_7O2 0x3b
#define UART_8O2 0x3f
*/

// Options for `mode` argument of uart_init
#define UART_FULL     0
#define UART_RX_ONLY  1
#define UART_TX_ONLY  2

#define UART_TX_FIFO_SIZE 0x80

struct uart_;
typedef struct uart_ uart_t;

uart_t* uart_init(int uart_nr, int baudrate, int config, int mode, int tx_pin, size_t rx_size);
void uart_uninit(uart_t* uart);

void uart_swap(uart_t* uart, int tx_pin);
void uart_set_tx(uart_t* uart, int tx_pin);
void uart_set_pins(uart_t* uart, int tx, int rx);
bool uart_tx_enabled(uart_t* uart);
bool uart_rx_enabled(uart_t* uart);

void uart_set_baudrate(uart_t* uart, int baud_rate);
int uart_get_baudrate(uart_t* uart);

size_t uart_resize_rx_buffer(uart_t* uart, size_t new_size);
size_t uart_get_rx_buffer_size(uart_t* uart);

size_t uart_write_char(uart_t* uart, char c);
size_t uart_write(uart_t* uart, const char* buf, size_t size);
int uart_read_char(uart_t* uart);
int uart_peek_char(uart_t* uart);
size_t uart_read(uart_t* uart, char* buffer, size_t size);
size_t uart_rx_available(uart_t* uart);
size_t uart_tx_free(uart_t* uart);
void uart_wait_tx_empty(uart_t* uart);
void uart_flush(uart_t* uart);

bool uart_has_overrun (uart_t* uart); // returns then clear overrun flag
bool uart_has_rx_error (uart_t* uart); // returns then clear rxerror flag

void uart_set_debug(int uart_nr);
int uart_get_debug();

void uart_start_detect_baudrate(int uart_nr);
int uart_detect_baudrate(int uart_nr);


#if defined (__cplusplus)
} // extern "C"
#endif

#endif // ESP_UART_H
