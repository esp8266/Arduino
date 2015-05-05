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

 Modified 31 March 2015 by Markus Sattler (rewrite the code for UART0 + UART1 support in ESP8266)
 Modified 25 April 2015 by Thomas Flayols (add configuration different from 8N1 in ESP8266)
 Modified 3 May 2015 by Hristo Gochkov (change register access methods)
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
#include "user_interface.h"
}

#include "HardwareSerial.h"

#define UART_TX_FIFO_SIZE 0x80

struct uart_ {
    int uart_nr;
    int baud_rate;
    bool rxEnabled;
    bool txEnabled;
    uint8_t rxPin;
    uint8_t txPin;
};

static const int UART0 = 0;
static const int UART1 = 1;
static const int UART_NO = -1;


/**
 *  UART GPIOs
 *
 * UART0 TX: 1 or 2
 * UART0 RX: 3
 *
 * UART0 SWAP TX: 15
 * UART0 SWAP RX: 13
 *
 *
 * UART1 TX: 7 (NC) or 2
 * UART1 RX: 8 (NC)
 *
 * UART1 SWAP TX: 11 (NC)
 * UART1 SWAP RX: 6 (NC)
 *
 * NC = Not Connected to Module Pads --> No Access
 *
 */

// ####################################################################################################
// ####################################################################################################
// ####################################################################################################
HardwareSerial Serial(UART0);
HardwareSerial Serial1(UART1);

// ####################################################################################################
// ####################################################################################################
// ####################################################################################################

void uart_interrupt_handler(uart_t* uart);
void uart_wait_for_tx_fifo(uart_t* uart, size_t size_needed);

size_t uart_get_tx_fifo_room(uart_t* uart);
void uart_wait_for_transmit(uart_t* uart);
void uart_transmit_char(uart_t* uart, char c);
void uart_transmit(uart_t* uart, const char* buf, size_t size);
void uart_flush(uart_t* uart);
void uart_interrupt_enable(uart_t* uart);
void uart_interrupt_disable(uart_t* uart);
void uart_arm_tx_interrupt(uart_t* uart);
void uart_disarm_tx_interrupt(uart_t* uart);
void uart_set_baudrate(uart_t* uart, int baud_rate);
int uart_get_baudrate(uart_t* uart);

uart_t* uart_init(int uart_nr, int baudrate, byte config);
void uart_uninit(uart_t* uart);
void uart_swap(uart_t* uart);

void uart_ignore_char(char c);
void uart0_write_char(char c);
void uart1_write_char(char c);

void uart_set_debug(int uart_nr);
int uart_get_debug();

// ####################################################################################################
// ####################################################################################################
// ####################################################################################################

void ICACHE_RAM_ATTR uart_interrupt_handler(uart_t* uart) {

    // -------------- UART 0 --------------
    uint32_t status = U0IS;
    if(Serial.isRxEnabled()) {
        if(status & (1 << UIFF)) {
            while(true) {
                int rx_count = (U0S >> USTXC) & 0xff;
                if(!rx_count)
                    break;

                while(rx_count--) {
                    char c = U0F & 0xff;
                    Serial._rx_complete_irq(c);
                }
            }
            U0IC = (1 << UIFF);
        }
    }
    if(Serial.isTxEnabled()) {
        if(status & (1 << UIFE)) {
            U0IC = (1 << UIFE);
            Serial._tx_empty_irq();
        }
    }

    // -------------- UART 1 --------------

    status = U1IS;
    if(Serial1.isRxEnabled()) {
        if(status & (1 << UIFF)) {
            while(true) {
                int rx_count = (U1S >> USTXC) & 0xff;
                if(!rx_count)
                    break;

                while(rx_count--) {
                    char c = U1F & 0xff;
                    Serial1._rx_complete_irq(c);
                }
            }
            U1IC = (1 << UIFF);
        }
    }
    if(Serial1.isTxEnabled()) {
        status = U1IS;
        if(status & (1 << UIFE)) {
            U1IC = (1 << UIFE);
            Serial1._tx_empty_irq();
        }
    }
}

// ####################################################################################################

void uart_wait_for_tx_fifo(uart_t* uart, size_t size_needed) {
    if(uart == 0)
        return;
    if(uart->txEnabled) {
        while(true) {
            size_t tx_count = (USS(uart->uart_nr) >> USTXC) & 0xff;
            if(tx_count <= (UART_TX_FIFO_SIZE - size_needed))
                break;
        }
    }
}

size_t uart_get_tx_fifo_room(uart_t* uart) {
    if(uart == 0)
        return 0;
    if(uart->txEnabled) {
        return UART_TX_FIFO_SIZE - ((USS(uart->uart_nr) >> USTXC) & 0xff);
    }
    return 0;
}

void uart_wait_for_transmit(uart_t* uart) {
    if(uart == 0)
        return;
    if(uart->txEnabled) {
        uart_wait_for_tx_fifo(uart, UART_TX_FIFO_SIZE);
    }
}

void uart_transmit_char(uart_t* uart, char c) {
    if(uart == 0)
        return;
    if(uart->txEnabled) {
        USF(uart->uart_nr) = c;
    }
}

void uart_transmit(uart_t* uart, const char* buf, size_t size) {
    if(uart == 0)
        return;
    if(uart->txEnabled) {
        while(size) {
            size_t part_size = (size > UART_TX_FIFO_SIZE) ? UART_TX_FIFO_SIZE : size;
            size -= part_size;

            uart_wait_for_tx_fifo(uart, part_size);
            for(; part_size; --part_size, ++buf)
                USF(uart->uart_nr) = *buf;
        }
    }
}

void uart_flush(uart_t* uart) {
    uint32_t tmp = 0x00000000;

    if(uart == 0)
        return;

    if(uart->rxEnabled) {
        tmp |= (1 << UCRXRST);
    }

    if(uart->txEnabled) {
        tmp |= (1 << UCTXRST);
    }

    USC0(uart->uart_nr) |= (tmp);
    USC0(uart->uart_nr) &= ~(tmp);
}

void uart_interrupt_enable(uart_t* uart) {
    if(uart == 0)
        return;
    USIC(uart->uart_nr) = 0x1ff;
    ETS_UART_INTR_ATTACH(&uart_interrupt_handler, uart); // uart parameter is not osed in irq function!
    if(uart->rxEnabled) {
        USIE(uart->uart_nr) |= (1 << UIFF);
    }
    ETS_UART_INTR_ENABLE();
}

void uart_interrupt_disable(uart_t* uart) {
    if(uart == 0)
        return;
    if(uart->rxEnabled) {
        USIE(uart->uart_nr) &= ~(1 << UIFF);
    }
    if(uart->txEnabled) {
        USIE(uart->uart_nr) &= ~(1 << UIFE);
    }
    //ETS_UART_INTR_DISABLE(); // never disable irq complete may its needed by the other Serial Interface!
}

void uart_arm_tx_interrupt(uart_t* uart) {
    if(uart == 0)
        return;
    if(uart->txEnabled) {
        USIE(uart->uart_nr) |= (1 << UIFE);
    }
}

void uart_disarm_tx_interrupt(uart_t* uart) {
    if(uart == 0)
        return;
    if(uart->txEnabled) {
        USIE(uart->uart_nr) &= ~(1 << UIFE);
    }
}

void uart_set_baudrate(uart_t* uart, int baud_rate) {
    if(uart == 0)
        return;
    uart->baud_rate = baud_rate;
    USD(uart->uart_nr) = (80000000UL / uart->baud_rate);
}

int uart_get_baudrate(uart_t* uart) {
    if(uart == 0)
        return 0;
    return uart->baud_rate;
}

uart_t* uart_init(int uart_nr, int baudrate, byte config) {

    uint32_t conf1 = 0x00000000;
    uart_t* uart = (uart_t*) os_malloc(sizeof(uart_t));

    if(uart == 0) {
        return 0;
    }

    uart->uart_nr = uart_nr;

    switch(uart->uart_nr) {
        case UART0:
            pinMode(1, SPECIAL);
            pinMode(3, SPECIAL);
            uart->rxEnabled = true;
            uart->txEnabled = true;
            uart->rxPin = 3;
            uart->txPin = 1;
            break;
        case UART1:
            pinMode(2, SPECIAL);
            uart->rxEnabled = false;
            uart->txEnabled = true;
            uart->rxPin = 255;
            uart->txPin = 2;
            break;
        case UART_NO:
        default:
            // big fail!
            break;
    }
    uart_set_baudrate(uart, baudrate);
    USC0(uart->uart_nr) = config;

    uart_flush(uart);
    uart_interrupt_enable(uart);

    if(uart->rxEnabled) {
        conf1 |= (0x01 << UCFFT);
    }

    if(uart->txEnabled) {
        conf1 |= (0x20 << UCFET);
    }

    USC1(uart->uart_nr) = conf1;

    return uart;
}

void uart_uninit(uart_t* uart) {
    if(uart == 0)
        return;
    uart_interrupt_disable(uart);

    switch(uart->rxPin) {
        case 3:
            pinMode(3, INPUT);
            break;
        case 13:
            pinMode(13, INPUT);
            break;
    }

    switch(uart->txPin) {
        case 1:
            pinMode(1, INPUT);
            break;
        case 2:
            pinMode(2, INPUT);
            break;
        case 15:
            pinMode(15, INPUT);
            break;
    }

    os_free(uart);
}

void uart_swap(uart_t* uart) {
    if(uart == 0)
        return;
    switch(uart->uart_nr) {
        case UART0:
            if(uart->txPin == 1 && uart->rxPin == 3) {
                pinMode(15, FUNCTION_4);//TX
                pinMode(13, FUNCTION_4);//RX
                USWAP |= (1 << USWAP0);
                pinMode(1, INPUT);//TX
                pinMode(3, INPUT);//RX
                uart->rxPin = 13;
                uart->txPin = 15;
            } else {
                pinMode(1, SPECIAL);//TX
                pinMode(3, SPECIAL);//RX
                USWAP &= ~(1 << USWAP0);
                pinMode(15, INPUT);//TX
                pinMode(13, INPUT);//RX
                uart->rxPin = 3;
                uart->txPin = 1;
            }

            break;
        case UART1:
            // current no swap possible! see GPIO pins used by UART
            break;
        default:
            break;
    }
}

// ####################################################################################################
// ####################################################################################################
// ####################################################################################################

void uart_ignore_char(char c) {
}

void uart0_write_char(char c) {
    if(&Serial != NULL && Serial.isTxEnabled()) {
        if(c == '\n') {
            Serial.write('\r');
        }
        Serial.write(c);
    } else {
        if(c == '\n') {
            USF(0) = '\r';
        }
        USF(0) = c;
    }
}

void uart1_write_char(char c) {
    if(&Serial1 != NULL && Serial1.isTxEnabled()) {
        if(c == '\n') {
            Serial1.write('\r');
        }
        Serial1.write(c);
    } else {
        if(c == '\n') {
            USF(1) = '\r';
        }
        USF(1) = c;
    }
}

static int s_uart_debug_nr = UART0;

void uart_set_debug(int uart_nr) {
    s_uart_debug_nr = uart_nr;
    switch(s_uart_debug_nr) {
        case UART0:
            system_set_os_print(1);
            ets_install_putc1((void *) &uart0_write_char);
            break;
        case UART1:
            system_set_os_print(1);
            ets_install_putc1((void *) &uart1_write_char);
            break;
        case UART_NO:
        default:
            system_set_os_print(0);
            ets_install_putc1((void *) &uart_ignore_char);
            break;
    }
}

int uart_get_debug() {
    return s_uart_debug_nr;
}

// ####################################################################################################
// ####################################################################################################
// ####################################################################################################

HardwareSerial::HardwareSerial(int uart_nr) :
        _uart_nr(uart_nr), _uart(0), _tx_buffer(0), _rx_buffer(0), _written(false) {
}

void HardwareSerial::begin(unsigned long baud, byte config) {

    // disable debug for this interface
    if(uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
    }

    _uart = uart_init(_uart_nr, baud, config);

    if(_uart == 0) {
        return;
    }

    if(_uart->rxEnabled) {
        if (!_rx_buffer)
            _rx_buffer = new cbuf(SERIAL_RX_BUFFER_SIZE);
    }
    if(_uart->txEnabled) {
        if (!_tx_buffer)
            _tx_buffer = new cbuf(SERIAL_TX_BUFFER_SIZE);
    }
    _written = false;
    delay(1);
}

void HardwareSerial::end() {
    if(uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
    }
    uart_uninit(_uart);
    delete _rx_buffer;
    delete _tx_buffer;
    _uart = 0;
    _rx_buffer = 0;
    _tx_buffer = 0;
}

void HardwareSerial::swap() {
    if(_uart == 0)
        return;
    uart_swap(_uart);
}

void HardwareSerial::setDebugOutput(bool en) {
    if(_uart == 0)
        return;
    if(en) {
        uart_set_debug(_uart->uart_nr);
    } else {
        // disable debug for this interface
        if(uart_get_debug() == _uart_nr) {
            uart_set_debug(UART_NO);
        }
    }
}

bool HardwareSerial::isTxEnabled(void) {
    if(_uart == 0)
        return false;
    return _uart->txEnabled;
}

bool HardwareSerial::isRxEnabled(void) {
    if(_uart == 0)
        return false;
    return _uart->rxEnabled;
}

int HardwareSerial::available(void) {
    if(_uart == 0)
        return 0;
    if(_uart->rxEnabled) {
        return static_cast<int>(_rx_buffer->getSize());
    } else {
        return 0;
    }
}

int HardwareSerial::peek(void) {
    if(_uart == 0)
        return -1;
    if(_uart->rxEnabled) {
        return _rx_buffer->peek();
    } else {
        return -1;
    }
}

int HardwareSerial::read(void) {
    if(_uart == 0)
        return -1;
    if(_uart->rxEnabled) {
        return _rx_buffer->read();
    } else {
        return -1;
    }
}

int HardwareSerial::availableForWrite(void) {
    if(_uart == 0)
        return 0;
    if(_uart->txEnabled) {
        return static_cast<int>(_tx_buffer->room());
    } else {
        return 0;
    }
}

void HardwareSerial::flush() {
    if(_uart == 0)
        return;
    if(!_uart->txEnabled)
        return;
    if(!_written)
        return;

    while(_tx_buffer->getSize() || uart_get_tx_fifo_room(_uart) < UART_TX_FIFO_SIZE)
        yield();

    _written = false;
}

size_t HardwareSerial::write(uint8_t c) {
    if(_uart == 0 || !_uart->txEnabled)
        return 0;
    _written = true;
    size_t room = uart_get_tx_fifo_room(_uart);
    if(room > 0 && _tx_buffer->empty()) {
        uart_transmit_char(_uart, c);
        if(room < 10) {
            uart_arm_tx_interrupt(_uart);
        }
        return 1;
    }

    while(_tx_buffer->room() == 0) {
        yield();
    }

    _tx_buffer->write(c);
    return 1;
}

HardwareSerial::operator bool() const {
    return _uart != 0;
}

void HardwareSerial::_rx_complete_irq(char c) {
    if(_rx_buffer) {
        _rx_buffer->write(c);
    }
}

void HardwareSerial::_tx_empty_irq(void) {
    if(_uart == 0)
        return;
    if(_tx_buffer == 0)
        return;
    size_t queued = _tx_buffer->getSize();
    if(!queued) {
        uart_disarm_tx_interrupt(_uart);
        return;
    }

    size_t room = uart_get_tx_fifo_room(_uart);
    int n = static_cast<int>((queued < room) ? queued : room);
    while(n--) {
        uart_transmit_char(_uart, _tx_buffer->read());
    }
}
