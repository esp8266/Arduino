/*
 MockUART.cpp - esp8266 UART HAL EMULATION

 Copyright (c) 2019 Clemens Kirchgatterer. All rights reserved.
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

/*
 This UART driver is directly derived from the ESP8266 UART HAL driver
 Copyright (c) 2014 Ivan Grokhotkov. It provides the same API as the
 original driver and was striped from all HW dependent interfaces.

 UART0 writes got to stdout, while UART1 writes got to stderr. The user
 is responsible for feeding the RX FIFO new data by calling uart_new_data().
 */

#include <unistd.h> // write
#include <sys/time.h> // gettimeofday
#include <time.h> // localtime

#include "Arduino.h"
#include "uart.h"

//#define UART_DISCARD_NEWEST

extern "C" {

bool blocking_uart = true; // system default

static int s_uart_debug_nr = UART1;

static uart_t *UART[2] = { NULL, NULL };

struct uart_rx_buffer_
{
	size_t size;
	size_t rpos;
	size_t wpos;
	uint8_t * buffer;
};

struct uart_
{
	int uart_nr;
	int baud_rate;
	bool rx_enabled;
	bool tx_enabled;
	bool rx_overrun;
	struct uart_rx_buffer_ * rx_buffer;
};

bool serial_timestamp = false;

// write one byte to the emulated UART
static void
uart_do_write_char(const int uart_nr, char c)
{
	static bool w = false;

	if (uart_nr >= UART0 && uart_nr <= UART1)
	{
		if (serial_timestamp && (c == '\n' || c == '\r'))
		{
			if (w)
			{
				FILE* out = uart_nr == UART0? stdout: stderr;
				timeval tv;
				gettimeofday(&tv, nullptr);
				const tm* tm = localtime(&tv.tv_sec);
				fprintf(out, "\r\n%d:%02d:%02d.%06d: ", tm->tm_hour, tm->tm_min, tm->tm_sec, (int)tv.tv_usec);
				fflush(out);
				w = false;
			}
		}
		else
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
			write(uart_nr + 1, &c, 1);
#pragma GCC diagnostic pop
			w = true;
		}
	}
}

// write a new byte into the RX FIFO buffer
static void
uart_handle_data(uart_t* uart, uint8_t data)
{
	struct uart_rx_buffer_ *rx_buffer = uart->rx_buffer;

	size_t nextPos = (rx_buffer->wpos + 1) % rx_buffer->size;
	if(nextPos == rx_buffer->rpos)
	{
		uart->rx_overrun = true;
#ifdef UART_DISCARD_NEWEST
		return;
#else
		if (++rx_buffer->rpos == rx_buffer->size)
			rx_buffer->rpos = 0;
#endif
	}
	rx_buffer->buffer[rx_buffer->wpos] = data;
	rx_buffer->wpos = nextPos;
}

// insert a new byte into the RX FIFO nuffer
void
uart_new_data(const int uart_nr, uint8_t data)
{
	uart_t* uart = UART[uart_nr];

	if(uart == NULL || !uart->rx_enabled) {
		return;
	}

	uart_handle_data(uart, data);
}

static size_t
uart_rx_available_unsafe(const struct uart_rx_buffer_ * rx_buffer)
{
	size_t ret = rx_buffer->wpos - rx_buffer->rpos;

	if(rx_buffer->wpos < rx_buffer->rpos)
		ret = (rx_buffer->wpos + rx_buffer->size) - rx_buffer->rpos;

	return ret;
}

// taking data straight from fifo, only needed in uart_resize_rx_buffer()
static int
uart_read_char_unsafe(uart_t* uart)
{
	if (uart_rx_available_unsafe(uart->rx_buffer))
	{
		// take oldest sw data
		int ret = uart->rx_buffer->buffer[uart->rx_buffer->rpos];
		uart->rx_buffer->rpos = (uart->rx_buffer->rpos + 1) % uart->rx_buffer->size;
		return ret;
	}
	// unavailable
	return -1;
}

/**********************************************************/
/************ UART API FUNCTIONS **************************/
/**********************************************************/

size_t
uart_rx_available(uart_t* uart)
{
	if(uart == NULL || !uart->rx_enabled)
		return 0;

	return uart_rx_available_unsafe(uart->rx_buffer);
}

int
uart_peek_char(uart_t* uart)
{
	if(uart == NULL || !uart->rx_enabled)
		return -1;

	if (!uart_rx_available_unsafe(uart->rx_buffer))
		return -1;

	return uart->rx_buffer->buffer[uart->rx_buffer->rpos];
}

int
uart_read_char(uart_t* uart)
{
	uint8_t ret;
	return uart_read(uart, (char*)&ret, 1) ? ret : -1;
}

size_t
uart_read(uart_t* uart, char* userbuffer, size_t usersize)
{
	if(uart == NULL || !uart->rx_enabled)
		return 0;

    if (!blocking_uart)
    {
        char c;
        if (read(0, &c, 1) == 1)
            uart_new_data(0, c);
    }

	size_t ret = 0;
	while (ret < usersize && uart_rx_available_unsafe(uart->rx_buffer))
	{
		// pour sw buffer to user's buffer
		// get largest linear length from sw buffer
		size_t chunk = uart->rx_buffer->rpos < uart->rx_buffer->wpos ?
		               uart->rx_buffer->wpos - uart->rx_buffer->rpos :
		               uart->rx_buffer->size - uart->rx_buffer->rpos;
		if (ret + chunk > usersize)
			chunk = usersize - ret;
		memcpy(userbuffer + ret, uart->rx_buffer->buffer + uart->rx_buffer->rpos, chunk);
		uart->rx_buffer->rpos = (uart->rx_buffer->rpos + chunk) % uart->rx_buffer->size;
		ret += chunk;
	}
	return ret;
}

size_t
uart_resize_rx_buffer(uart_t* uart, size_t new_size)
{
	if(uart == NULL || !uart->rx_enabled)
		return 0;

	if(uart->rx_buffer->size == new_size)
		return uart->rx_buffer->size;

	uint8_t * new_buf = (uint8_t*)malloc(new_size);
	if(!new_buf)
		return uart->rx_buffer->size;

	size_t new_wpos = 0;
	// if uart_rx_available_unsafe() returns non-0, uart_read_char_unsafe() can't return -1
	while(uart_rx_available_unsafe(uart->rx_buffer) && new_wpos < new_size)
		new_buf[new_wpos++] = uart_read_char_unsafe(uart);
	if (new_wpos == new_size)
		new_wpos = 0;

	uint8_t * old_buf = uart->rx_buffer->buffer;
	uart->rx_buffer->rpos = 0;
	uart->rx_buffer->wpos = new_wpos;
	uart->rx_buffer->size = new_size;
	uart->rx_buffer->buffer = new_buf;
	free(old_buf);
	return uart->rx_buffer->size;
}

size_t
uart_get_rx_buffer_size(uart_t* uart)
{
	return uart && uart->rx_enabled ? uart->rx_buffer->size : 0;
}

size_t
uart_write_char(uart_t* uart, char c)
{
	if(uart == NULL || !uart->tx_enabled)
		return 0;

	uart_do_write_char(uart->uart_nr, c);

	return 1;
}

size_t
uart_write(uart_t* uart, const char* buf, size_t size)
{
	if(uart == NULL || !uart->tx_enabled)
		return 0;

	size_t ret = size;
	const int uart_nr = uart->uart_nr;
	while (size--)
		uart_do_write_char(uart_nr, *buf++);

	return ret;
}

size_t
uart_tx_free(uart_t* uart)
{
	if(uart == NULL || !uart->tx_enabled)
		return 0;

	return UART_TX_FIFO_SIZE;
}

void
uart_wait_tx_empty(uart_t* uart)
{
	(void) uart;
}

void
uart_flush(uart_t* uart)
{
	if(uart == NULL)
		return;

	if(uart->rx_enabled)
	{
		uart->rx_buffer->rpos = 0;
		uart->rx_buffer->wpos = 0;
	}
}

void
uart_set_baudrate(uart_t* uart, int baud_rate)
{
	if(uart == NULL)
		return;

	uart->baud_rate = baud_rate;
}

int
uart_get_baudrate(uart_t* uart)
{
	if(uart == NULL)
		return 0;

	return uart->baud_rate;
}

uint8_t
uart_get_bit_length(const int uart_nr)
{
	uint8_t width = ((uart_nr % 16) >> 2) + 5;
	uint8_t parity = (uart_nr >> 5) + 1;
	uint8_t stop = uart_nr % 4;
	return (width + parity + stop + 1);
}

uart_t*
uart_init(int uart_nr, int baudrate, int config, int mode, int tx_pin, size_t rx_size, bool invert)
{
	(void) config;
	(void) tx_pin;
	(void) invert;
	uart_t* uart = (uart_t*) malloc(sizeof(uart_t));
	if(uart == NULL)
		return NULL;

	uart->uart_nr = uart_nr;
	uart->rx_overrun = false;

	switch(uart->uart_nr)
	{
	case UART0:
		uart->rx_enabled = (mode != UART_TX_ONLY);
		uart->tx_enabled = (mode != UART_RX_ONLY);
		if(uart->rx_enabled)
		{
			struct uart_rx_buffer_ * rx_buffer = (struct uart_rx_buffer_ *)malloc(sizeof(struct uart_rx_buffer_));
			if(rx_buffer == NULL)
			{
				free(uart);
				return NULL;
			}
			rx_buffer->size = rx_size;//var this
			rx_buffer->rpos = 0;
			rx_buffer->wpos = 0;
			rx_buffer->buffer = (uint8_t *)malloc(rx_buffer->size);
			if(rx_buffer->buffer == NULL)
			{
				free(rx_buffer);
				free(uart);
				return NULL;
			}
			uart->rx_buffer = rx_buffer;
		}
		break;

	case UART1:
		// Note: uart_interrupt_handler does not support RX on UART 1.
		uart->rx_enabled = false;
		uart->tx_enabled = (mode != UART_RX_ONLY);
		break;

	case UART_NO:
	default:
		// big fail!
		free(uart);
		return NULL;
	}

	uart_set_baudrate(uart, baudrate);

	UART[uart_nr] = uart;

	return uart;
}

void
uart_uninit(uart_t* uart)
{
	if(uart == NULL)
		return;

	if(uart->rx_enabled) {
		free(uart->rx_buffer->buffer);
		free(uart->rx_buffer);
	}
	free(uart);
}

void
uart_swap(uart_t* uart, int tx_pin)
{
	(void) uart;
	(void) tx_pin;
}

void
uart_set_tx(uart_t* uart, int tx_pin)
{
	(void) uart;
	(void) tx_pin;
}

void
uart_set_pins(uart_t* uart, int tx, int rx)
{
	(void) uart;
	(void) tx;
	(void) rx;
}

bool
uart_tx_enabled(uart_t* uart)
{
	if(uart == NULL)
		return false;

	return uart->tx_enabled;
}

bool
uart_rx_enabled(uart_t* uart)
{
	if(uart == NULL)
		return false;

	return uart->rx_enabled;
}

bool
uart_has_overrun(uart_t* uart)
{
	if(uart == NULL || !uart->rx_overrun)
		return false;

	// clear flag
	uart->rx_overrun = false;
	return true;
}

bool
uart_has_rx_error(uart_t* uart)
{
	(void) uart;
	return false;
}

void
uart_set_debug(int uart_nr)
{
	(void)uart_nr;
}

int
uart_get_debug()
{
	return s_uart_debug_nr;
}

void
uart_start_detect_baudrate(int uart_nr)
{
	(void) uart_nr;
}

int
uart_detect_baudrate(int uart_nr)
{
	(void) uart_nr;
	return 115200;
}

};


size_t uart_peek_available (uart_t* uart) { return 0; }
const char* uart_peek_buffer (uart_t* uart) { return nullptr; }
void uart_peek_consume (uart_t* uart, size_t consume) { (void)uart; (void)consume; }

