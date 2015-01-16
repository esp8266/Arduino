/* 
  HardwareSerial.cpp - esp8266 UART support

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
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "cbuf.h"

extern "C" {
#include "osapi.h"
#include "ets_sys.h"
#include "mem.h"
#include "uart_register.h"
#include "user_interface.h"
}

#include "HardwareSerial.h"

HardwareSerial Serial;

uart_t* uart0_init(int baud_rate);
void uart0_set_baudrate(uart_t* uart, int baud_rate);
int  uart0_get_baudrate(uart_t* uart);
void uart0_uninit(uart_t* uart);
void uart0_transmit(uart_t* uart, const char* buf, size_t size);    // may block on TX fifo
void uart0_wait_for_transmit(uart_t* uart);
void uart0_transmit_char(uart_t* uart, char c);  // does not block, but character will be lost if FIFO is full

void uart_set_debug(bool enabled);
bool uart_get_debug();

struct uart_
{
    int  baud_rate;
};

#define UART_TX_FIFO_SIZE 0x80

void ICACHE_FLASH_ATTR uart0_interrupt_handler(uart_t* uart)
{
    uint32_t status = READ_PERI_REG(UART_INT_ST(0));
    if (status & UART_RXFIFO_FULL_INT_ST)
    {
        while(true)
        {
            int rx_count = (READ_PERI_REG(UART_STATUS(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
            if (!rx_count)
                break;

            while(rx_count--)
            {
                char c = READ_PERI_REG(UART_FIFO(0)) & 0xFF;
                Serial._rx_complete_irq(c);
            }
        }
        WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_FULL_INT_CLR);
    }
    if (status & UART_TXFIFO_EMPTY_INT_ST)
    {
        WRITE_PERI_REG(UART_INT_CLR(0), UART_TXFIFO_EMPTY_INT_CLR);
        Serial._tx_empty_irq();
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

size_t ICACHE_FLASH_ATTR uart0_get_tx_fifo_room()
{
    return UART_TX_FIFO_SIZE - ((READ_PERI_REG(UART_STATUS(0)) >> UART_TXFIFO_CNT_S) & UART_TXFIFO_CNT);        
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
    ETS_UART_INTR_ATTACH(&uart0_interrupt_handler, uart);
    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA);
    ETS_UART_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR uart0_interrupt_disable(uart_t* uart)
{
    CLEAR_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA);
    ETS_UART_INTR_DISABLE();
}

void ICACHE_FLASH_ATTR uart0_arm_tx_interrupt()
{
    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_TXFIFO_EMPTY_INT_ENA);
}

void ICACHE_FLASH_ATTR uart0_disarm_tx_interrupt()
{
    CLEAR_PERI_REG_MASK(UART_INT_ENA(0), UART_TXFIFO_EMPTY_INT_ENA);
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

uart_t* ICACHE_FLASH_ATTR uart0_init(int baudrate)
{
    uart_t* uart = (uart_t*) os_malloc(sizeof(uart_t));

    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
    PIN_PULLUP_EN(PERIPHS_IO_MUX_U0RXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD);

    uart0_set_baudrate(uart, baudrate);
    WRITE_PERI_REG(UART_CONF0(0), 0x3 << UART_BIT_NUM_S);   // 8n1

    uart0_flush(uart);
    uart0_interrupt_enable(uart);

    WRITE_PERI_REG(UART_CONF1(0), ((0x01 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |
                                ((0x20 & UART_TXFIFO_EMPTY_THRHD) << UART_TXFIFO_EMPTY_THRHD_S));

    return uart;
}

void ICACHE_FLASH_ATTR uart0_uninit(uart_t* uart)
{
    uart0_interrupt_disable(uart);
    // TODO: revert pin functions
    os_free(uart);
}

void ICACHE_FLASH_ATTR uart0_swap(uart_t* uart)
{
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_UART0_CTS);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_UART0_RTS);
    //SWAP PIN : U0TXD<==>U0RTS(MTDO, GPIO15) , U0RXD<==>U0CTS(MTCK, GPIO13)
    SET_PERI_REG_MASK(0x3ff00028 , BIT2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);
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

bool s_uart_debug_enabled = true;
void ICACHE_FLASH_ATTR uart_set_debug(bool enabled)
{
    s_uart_debug_enabled = enabled;
    if (enabled)
    {
        system_set_os_print(1);
        ets_install_putc1((void *)&uart_write_char);
    }
    else
        ets_install_putc1((void *)&uart_ignore_char);
}

bool ICACHE_FLASH_ATTR uart_get_debug()
{
    return s_uart_debug_enabled;
}

ICACHE_FLASH_ATTR HardwareSerial::HardwareSerial() : 
    _uart(0), _rx_buffer(0), _tx_buffer(0)
{
}

void ICACHE_FLASH_ATTR HardwareSerial::begin(unsigned long baud, byte config)
{
    _rx_buffer = new cbuf(SERIAL_RX_BUFFER_SIZE);
    _tx_buffer = new cbuf(SERIAL_TX_BUFFER_SIZE);
    uart_set_debug(false);
    _uart = uart0_init(baud);
    _written = false;
    delay(1);
}

void ICACHE_FLASH_ATTR HardwareSerial::end()
{
    uart0_uninit(_uart);
    delete _rx_buffer;
    delete _tx_buffer;
    _uart = 0;
    _rx_buffer = 0;
    _tx_buffer = 0;
}

void ICACHE_FLASH_ATTR HardwareSerial::swap()
{
    uart0_swap(_uart);
    pinMode(1, INPUT);
    pinMode(3, INPUT);
}

void ICACHE_FLASH_ATTR HardwareSerial::setDebugOutput(bool en)
{
    uart_set_debug(en);
}

int ICACHE_FLASH_ATTR HardwareSerial::available(void)
{
    return static_cast<int>(_rx_buffer->getSize());
}

int ICACHE_FLASH_ATTR HardwareSerial::peek(void)
{
    return _rx_buffer->peek();
}

int ICACHE_FLASH_ATTR HardwareSerial::read(void)
{
    return _rx_buffer->read();
}

int ICACHE_FLASH_ATTR HardwareSerial::availableForWrite(void)
{
    return static_cast<int>(_tx_buffer->room());
}

void ICACHE_FLASH_ATTR HardwareSerial::flush()
{
    if (!_written)
        return;

    while (_tx_buffer->getSize() || uart0_get_tx_fifo_room() < UART_TX_FIFO_SIZE)
        yield();

    _written = false;
}

size_t ICACHE_FLASH_ATTR HardwareSerial::write(uint8_t c)
{
    _written = true;
    size_t room = uart0_get_tx_fifo_room();
    if (room > 0 && _tx_buffer->empty())
    {
        uart0_transmit_char(_uart, c);
        if (room < 10)
        {
            uart0_arm_tx_interrupt();
        }
        return 1;
    }
    
    while (_tx_buffer->room() == 0)
    {
        yield();
    }
    
    _tx_buffer->write(c);
    return 1;
}

ICACHE_FLASH_ATTR HardwareSerial::operator bool() const
{
    return _uart != 0;
}

void ICACHE_FLASH_ATTR HardwareSerial::_rx_complete_irq(char c)
{
    _rx_buffer->write(c);
}

void ICACHE_FLASH_ATTR HardwareSerial::_tx_empty_irq(void)
{
    size_t queued = _tx_buffer->getSize();
    if (!queued)
    {
        uart0_disarm_tx_interrupt();
        return;
    }

    size_t room = uart0_get_tx_fifo_room();
    int n = static_cast<int>((queued < room) ? queued : room);
    while (n--)
    {
        uart0_transmit_char(_uart, _tx_buffer->read());
    }
}

