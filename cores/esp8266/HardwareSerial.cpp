/*
    HardwareSerial.cpp - Hardware serial library for Wiring
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
    
    Modified 23 November 2006 by David A. Mellis
    Modified 28 September 2010 by Mark Sproul
    Modified 14 August 2012 by Alarus
    Modified 3 December 2013 by Matthijs Kooijman
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

extern "C" {
#include "osapi.h"
#include "ets_sys.h"
#include "mem.h"
#include "uart_register.h"
#include "user_interface.h"
}

#include "HardwareSerial.h"

typedef void (*uart_rx_handler_t)(char);

uart_t* uart0_init(int baud_rate, uart_rx_handler_t rx_handler);
void uart0_set_baudrate(uart_t* uart, int baud_rate);
int  uart0_get_baudrate(uart_t* uart);
void uart0_uninit(uart_t* uart);
void uart0_transmit(uart_t* uart, const char* buf, size_t size);    // may block on TX fifo
void uart0_wait_for_transmit(uart_t* uart);
void uart0_transmit_char(uart_t* uart, char c);  // does not block, but character will be lost if FIFO is full

void uart_set_debug(int enabled);
int  uart_get_debug();

struct uart_
{
    int  baud_rate;
    uart_rx_handler_t rx_handler;
};



#define UART_TX_FIFO_SIZE 0x7f

void ICACHE_FLASH_ATTR uart0_rx_handler(uart_t* uart)
{
    if (READ_PERI_REG(UART_INT_ST(0)) & UART_RXFIFO_FULL_INT_ST)
    {
        while(true)
        {
            int rx_count = (READ_PERI_REG(UART_STATUS(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
            if (!rx_count)
                break;

            for(int cnt = 0; cnt < rx_count; ++cnt)
            {
                char c = READ_PERI_REG(UART_FIFO(0)) & 0xFF;
                (*uart->rx_handler)(c);
            }
        }
        WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_FULL_INT_CLR);
    }
}

void ICACHE_FLASH_ATTR uart0_wait_for_tx_fifo(size_t size_needed)
{
    while (true)
    {
        size_t tx_count = (READ_PERI_REG(UART_STATUS(0)) >> UART_TXFIFO_CNT_S) & UART_TXFIFO_CNT;
        if (tx_count <= (UART_TX_FIFO_SIZE - size_needed))
            break;
    }
}

void ICACHE_FLASH_ATTR uart0_wait_for_transmit(uart_t* uart)
{
    uart0_wait_for_tx_fifo(UART_TX_FIFO_SIZE);
}

void ICACHE_FLASH_ATTR uart0_transmit_char(uart_t* uart, char c)
{
    WRITE_PERI_REG(UART_FIFO(0), c);
}

void ICACHE_FLASH_ATTR uart0_transmit(uart_t* uart, const char* buf, size_t size)
{
    while (size)
    {
        size_t part_size = (size > UART_TX_FIFO_SIZE) ? UART_TX_FIFO_SIZE : size;
        size -= part_size;

        uart0_wait_for_tx_fifo(part_size);
        for(;part_size;--part_size, ++buf)
            WRITE_PERI_REG(UART_FIFO(0), *buf);
    }
}

void ICACHE_FLASH_ATTR uart0_flush(uart_t* uart)
{
    SET_PERI_REG_MASK(UART_CONF0(0), UART_RXFIFO_RST | UART_TXFIFO_RST);
    CLEAR_PERI_REG_MASK(UART_CONF0(0), UART_RXFIFO_RST | UART_TXFIFO_RST);
}

void ICACHE_FLASH_ATTR uart0_interrupt_enable(uart_t* uart)
{
    WRITE_PERI_REG(UART_INT_CLR(0), 0x1ff);
    ETS_UART_INTR_ATTACH(&uart0_rx_handler, uart);
    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA);
    ETS_UART_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR uart0_interrupt_disable(uart_t* uart)
{
    SET_PERI_REG_MASK(UART_INT_ENA(0), 0);
    ETS_UART_INTR_DISABLE();
}

void ICACHE_FLASH_ATTR uart0_set_baudrate(uart_t* uart, int baud_rate)
{
    uart->baud_rate = baud_rate;
    uart_div_modify(0, UART_CLK_FREQ / (uart->baud_rate));
}

int ICACHE_FLASH_ATTR uart0_get_baudrate(uart_t* uart)
{
    return uart->baud_rate;
}

uart_t* ICACHE_FLASH_ATTR uart0_init(int baudrate, uart_rx_handler_t rx_handler)
{
    uart_t* uart = (uart_t*) os_malloc(sizeof(uart_t));

    uart->rx_handler = rx_handler;

    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);

    uart0_set_baudrate(uart, baudrate);
    WRITE_PERI_REG(UART_CONF0(0), 0x3 << UART_BIT_NUM_S);   // 8n1

    uart0_flush(uart);
    uart0_interrupt_enable(uart);
    
    WRITE_PERI_REG(UART_CONF1(0), ((0x01 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S));

    return uart;
}

void ICACHE_FLASH_ATTR uart0_uninit(uart_t* uart)
{
    uart0_interrupt_disable(uart);
    // TODO: revert pin functions
    os_free(uart);
}


void ICACHE_FLASH_ATTR
uart_ignore_char(char c)
{    
}

void ICACHE_FLASH_ATTR
uart_write_char(char c)
{
    if (c == '\n')
        WRITE_PERI_REG(UART_FIFO(0), '\r');
    
    WRITE_PERI_REG(UART_FIFO(0), c);
}

int s_uart_debug_enabled = 1;
void ICACHE_FLASH_ATTR uart_set_debug(int enabled)
{
    s_uart_debug_enabled = enabled;
    if (enabled)
        ets_install_putc1((void *)&uart_write_char);
    else
        ets_install_putc1((void *)&uart_ignore_char);
}

int ICACHE_FLASH_ATTR uart_get_debug()
{
    return s_uart_debug_enabled;
}

HardwareSerial Serial;

void ICACHE_FLASH_ATTR serial_rx_handler(char c)
{
    Serial._rx_complete_irq(c);
}
extern "C" size_t ets_printf(const char*, ...);

ICACHE_FLASH_ATTR HardwareSerial::HardwareSerial() : 
    _rx_buffer_head(0), _rx_buffer_tail(0),
    _tx_buffer_head(0), _tx_buffer_tail(0),
    _uart(0)
{
}

void ICACHE_FLASH_ATTR HardwareSerial::begin(unsigned long baud, byte config)
{
    _uart = uart0_init(baud, &serial_rx_handler);
    _written = false;
}

void ICACHE_FLASH_ATTR HardwareSerial::end()
{
    uart0_uninit(_uart);
    _uart = 0;
}

int ICACHE_FLASH_ATTR HardwareSerial::available(void)
{
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}

int ICACHE_FLASH_ATTR HardwareSerial::peek(void)
{
    if (_rx_buffer_head == _rx_buffer_tail) {
        return -1;
    } else {
        return _rx_buffer[_rx_buffer_tail];
    }
}

int ICACHE_FLASH_ATTR HardwareSerial::read(void)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (_rx_buffer_head == _rx_buffer_tail) {
        return -1;
    } else {
        unsigned char c = _rx_buffer[_rx_buffer_tail];
        _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
        return c;
    }
}

int ICACHE_FLASH_ATTR HardwareSerial::availableForWrite(void)
{
    tx_buffer_index_t head = _tx_buffer_head;
    tx_buffer_index_t tail = _tx_buffer_tail;
    if (head >= tail) return SERIAL_TX_BUFFER_SIZE - 1 - head + tail;
    return tail - head - 1;
}


void ICACHE_FLASH_ATTR HardwareSerial::flush()
{
    if (!_written)
        return;
}


size_t ICACHE_FLASH_ATTR HardwareSerial::write(uint8_t c)
{
    uart0_transmit_char(_uart, c);
    // // If the buffer and the data register is empty, just write the byte
    // // to the data register and be done. This shortcut helps
    // // significantly improve the effective datarate at high (>
    // // 500kbit/s) bitrates, where interrupt overhead becomes a slowdown.
    // if (_tx_buffer_head == _tx_buffer_tail && bit_is_set(*_ucsra, UDRE0)) {
    //     *_udr = c;
    //     sbi(*_ucsra, TXC0);
    //     return 1;
    // }
    // tx_buffer_index_t i = (_tx_buffer_head + 1) % SERIAL_TX_BUFFER_SIZE;
    
    // // If the output buffer is full, there's nothing for it other than to 
    // // wait for the interrupt handler to empty it a bit
    // while (i == _tx_buffer_tail) {
    //     if (bit_is_clear(SREG, SREG_I)) {
    //         // Interrupts are disabled, so we'll have to poll the data
    //         // register empty flag ourselves. If it is set, pretend an
    //         // interrupt has happened and call the handler to free up
    //         // space for us.
    //         if(bit_is_set(*_ucsra, UDRE0))
    // _tx_udr_empty_irq();
    //     } else {
    //         // nop, the interrupt handler will free up space for us
    //     }
    // }

    // _tx_buffer[_tx_buffer_head] = c;
    // _tx_buffer_head = i;
    
    // sbi(*_ucsrb, UDRIE0);
    _written = true;
    
    return 1;
}

void ICACHE_FLASH_ATTR HardwareSerial::_rx_complete_irq(char c)
{
    rx_buffer_index_t i = (unsigned int)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;

    if (i != _rx_buffer_tail) {
        _rx_buffer[_rx_buffer_head] = c;
        _rx_buffer_head = i;
    }
}


// void HardwareSerial::_tx_udr_empty_irq(void)
// {
//   // If interrupts are enabled, there must be more data in the output
//   // buffer. Send the next byte
//   unsigned char c = _tx_buffer[_tx_buffer_tail];
//   _tx_buffer_tail = (_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;

//   *_udr = c;

//   // clear the TXC bit -- "can be cleared by writing a one to its bit
//   // location". This makes sure flush() won't return until the bytes
//   // actually got written
//   sbi(*_ucsra, TXC0);

//   if (_tx_buffer_head == _tx_buffer_tail) {
//     // Buffer empty, so disable interrupts
//     cbi(*_ucsrb, UDRIE0);
//   }
// }

