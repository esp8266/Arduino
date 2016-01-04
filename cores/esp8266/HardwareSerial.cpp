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
#include "interrupts.h"

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

uart_t* uart_start_init(int uart_nr, int baudrate, byte config);
void uart_finish_init(uart_t* uart);
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

// These function internals can be used from interrupt handlers to ensure they
// are in instruction RAM, or anywhere that the uart_nr has been validated.
#define UART_GET_TX_FIFO_ROOM(uart_nr)     (UART_TX_FIFO_SIZE - ((USS(uart_nr) >> USTXC) & 0xff))
#define UART_TRANSMIT_CHAR(uart_nr, c)     do { USF(uart_nr) = (c); } while(0)
#define UART_ARM_TX_INTERRUPT(uart_nr)     do { USIE(uart_nr) |= (1 << UIFE); } while(0)
#define UART_DISARM_TX_INTERRUPT(uart_nr)  do { USIE(uart_nr) &= ~(1 << UIFE); } while(0)

void ICACHE_RAM_ATTR uart_interrupt_handler(uart_t* uart) {

    // -------------- UART 0 --------------
    if(Serial.isRxEnabled()) {
        while(U0IS & (1 << UIFF)) {
            Serial._rx_complete_irq((char) (U0F & 0xff));
            U0IC = (1 << UIFF);
        }
    }
    if(Serial.isTxEnabled()) {
        if(U0IS & (1 << UIFE)) {
            U0IC = (1 << UIFE);
            Serial._tx_empty_irq();
        }
    }

    // -------------- UART 1 --------------
    // Note: only TX is supported on UART 1.
    if(Serial1.isTxEnabled()) {
        if(U1IS & (1 << UIFE)) {
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
        return UART_GET_TX_FIFO_ROOM(uart->uart_nr);
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
        UART_TRANSMIT_CHAR(uart->uart_nr, c);
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
        UART_ARM_TX_INTERRUPT(uart->uart_nr);
    }
}

void uart_disarm_tx_interrupt(uart_t* uart) {
    if(uart == 0)
        return;
    if(uart->txEnabled) {
        UART_DISARM_TX_INTERRUPT(uart->uart_nr);
    }
}

void uart_set_baudrate(uart_t* uart, int baud_rate) {
    if(uart == 0)
        return;
    uart->baud_rate = baud_rate;
    USD(uart->uart_nr) = (ESP8266_CLOCK / uart->baud_rate);
}

int uart_get_baudrate(uart_t* uart) {
    if(uart == 0)
        return 0;
    return uart->baud_rate;
}

uart_t* uart_start_init(int uart_nr, int baudrate, byte config, byte mode) {

    uart_t* uart = (uart_t*) os_malloc(sizeof(uart_t));

    if(uart == 0) {
        return 0;
    }

    uart->uart_nr = uart_nr;

    switch(uart->uart_nr) {
        case UART0:
            uart->rxEnabled = (mode != SERIAL_TX_ONLY);
            uart->txEnabled = (mode != SERIAL_RX_ONLY);
            uart->rxPin = (uart->rxEnabled)?3:255;
            uart->txPin = (uart->txEnabled)?1:255;
            if(uart->rxEnabled) pinMode(uart->rxPin, SPECIAL);
            if(uart->txEnabled) pinMode(uart->txPin, SPECIAL);
            IOSWAP &= ~(1 << IOSWAPU0);
            break;
        case UART1:
            // Note: uart_interrupt_handler does not support RX on UART 1.
            uart->rxEnabled = false;
            uart->txEnabled = (mode != SERIAL_RX_ONLY);
            uart->rxPin = 255;
            uart->txPin = (uart->txEnabled)?2:255;
            if(uart->txEnabled) pinMode(uart->txPin, SPECIAL);
            break;
        case UART_NO:
        default:
            // big fail!
            os_free(uart);
            return 0;
    }
    uart_set_baudrate(uart, baudrate);
    USC0(uart->uart_nr) = config;

    return uart;
}

void uart_finish_init(uart_t* uart) {
    uint32_t conf1 = 0x00000000;

    uart_flush(uart);
    uart_interrupt_enable(uart);

    if(uart->rxEnabled) {
        conf1 |= (0x01 << UCFFT);
    }

    if(uart->txEnabled) {
        conf1 |= (0x20 << UCFET);
    }

    USC1(uart->uart_nr) = conf1;
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
            if((uart->txPin == 1 && uart->txEnabled) || (uart->rxPin == 3 && uart->rxEnabled)) {
                if(uart->txEnabled) pinMode(15, FUNCTION_4); //TX
                if(uart->rxEnabled) pinMode(13, FUNCTION_4); //RX
                IOSWAP |= (1 << IOSWAPU0);
                if(uart->txEnabled){ //TX
                  pinMode(1, INPUT);
                  uart->txPin = 15;
                }
                if(uart->rxEnabled){ //RX
                  pinMode(3, INPUT);
                  uart->rxPin = 13;
                }
            } else {
                if(uart->txEnabled) pinMode(1, SPECIAL); //TX
                if(uart->rxEnabled) pinMode(3, SPECIAL); //RX
                IOSWAP &= ~(1 << IOSWAPU0);
                if(uart->txEnabled){ //TX
                  pinMode(15, INPUT);
                  uart->txPin = 1;
                }
                if(uart->rxEnabled){ //RX
                  pinMode(13, INPUT); //RX
                  uart->rxPin = 3;
                }
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
        if(Serial.availableForWrite() > 0) {
            if(c == '\n') {
                Serial.write('\r');
            }
            Serial.write(c);
            return;
        }
    }

    // wait for the Hardware FIFO
    while(true) {
        if(((USS(0) >> USTXC) & 0xff) <= (UART_TX_FIFO_SIZE - 2)) {
            break;
        }
    }

    if(c == '\n') {
        USF(0) = '\r';
    }
    USF(0) = c;
}

void uart1_write_char(char c) {
    if(&Serial1 != NULL && Serial1.isTxEnabled()) {
        if(Serial1.availableForWrite() > 0) {
            if(c == '\n') {
                Serial1.write('\r');
            }
            Serial1.write(c);
            return;
        }
    }

    // wait for the Hardware FIFO
    while(true) {
        if(((USS(1) >> USTXC) & 0xff) <= (UART_TX_FIFO_SIZE - 2)) {
            break;
        }
    }

    if(c == '\n') {
        USF(1) = '\r';
    }
    USF(1) = c;
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
        _uart_nr(uart_nr), _uart(0), _tx_buffer(0), _rx_buffer(0) {
}

void HardwareSerial::begin(unsigned long baud, byte config, byte mode) {
    InterruptLock il;

    // disable debug for this interface
    if(uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
    }

    if (_uart) {
        os_free(_uart);
    }
    _uart = uart_start_init(_uart_nr, baud, config, mode);

    if(_uart == 0) {
        return;
    }

    // Disable the RX and/or TX functions if we fail to allocate circular buffers.
    // The user can confirm they are enabled with isRxEnabled() and isTxEnabled().
    if(_uart->rxEnabled && !_rx_buffer) {
        _rx_buffer = new cbuf(SERIAL_RX_BUFFER_SIZE);
        if(!_rx_buffer) {
            _uart->rxEnabled = false;
        }
    }
    if(_uart->txEnabled && !_tx_buffer) {
        _tx_buffer = new cbuf(SERIAL_TX_BUFFER_SIZE);
        if(!_tx_buffer) {
            _uart->txEnabled = false;
        }
    }
    delay(1);

    uart_finish_init(_uart);
}

void HardwareSerial::end() {
    InterruptLock il;

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
        if(_uart->txEnabled)
          uart_set_debug(_uart->uart_nr);
        else
          uart_set_debug(UART_NO);
    } else {
        // disable debug for this interface
        if(uart_get_debug() == _uart_nr) {
            uart_set_debug(UART_NO);
        }
    }
}

bool ICACHE_RAM_ATTR HardwareSerial::isTxEnabled(void) {
    if(_uart == 0)
        return false;
    return _uart->txEnabled;
}

bool ICACHE_RAM_ATTR HardwareSerial::isRxEnabled(void) {
    if(_uart == 0)
        return false;
    return _uart->rxEnabled;
}

int HardwareSerial::available(void) {
    int result = 0;

    if (_uart != NULL && _uart->rxEnabled) {
        InterruptLock il;
        result = static_cast<int>(_rx_buffer->getSize());
    }

    if (!result) {
        optimistic_yield(USD(_uart->uart_nr) / 128);
    }

    return result;
}

int HardwareSerial::peek(void) {
    if(_uart == 0)
        return -1;
    if(_uart->rxEnabled) {
        InterruptLock il;
        return _rx_buffer->peek();
    } else {
        return -1;
    }
}

int HardwareSerial::read(void) {
    if(_uart == 0)
        return -1;
    if(_uart->rxEnabled) {
        InterruptLock il;
        return _rx_buffer->read();
    } else {
        return -1;
    }
}

int HardwareSerial::availableForWrite(void) {
    if(_uart == 0)
        return 0;
    if(_uart->txEnabled) {
        InterruptLock il;
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

    const int uart_nr = _uart->uart_nr;
    while(true) {
        {
            InterruptLock il;
            if(_tx_buffer->getSize() == 0 &&
               UART_GET_TX_FIFO_ROOM(uart_nr) >= UART_TX_FIFO_SIZE) {
                break;
            } else if(il.savedInterruptLevel() > 0) {
                _tx_empty_irq();
                continue;
            }
        }
        yield();
    }
}

size_t HardwareSerial::write(uint8_t c) {
    if(_uart == 0 || !_uart->txEnabled)
        return 0;

    bool tx_now = false;
    const int uart_nr = _uart->uart_nr;
    while(true) {
        {
            InterruptLock il;
            if(_tx_buffer->empty()) {
                if(UART_GET_TX_FIFO_ROOM(uart_nr) > 0) {
                    tx_now = true;
                } else {
                    _tx_buffer->write(c);
                    UART_ARM_TX_INTERRUPT(uart_nr);
                }
                break;
            } else if(_tx_buffer->write(c)) {
                break;
            } else if(il.savedInterruptLevel() > 0) {
                _tx_empty_irq();
                continue;
            }
        }
        yield();
    }
    if (tx_now) {
        UART_TRANSMIT_CHAR(uart_nr, c);
    }
    return 1;
}

HardwareSerial::operator bool() const {
    return _uart != 0;
}

void ICACHE_RAM_ATTR HardwareSerial::_rx_complete_irq(char c) {
    _rx_buffer->write(c);
}

void ICACHE_RAM_ATTR HardwareSerial::_tx_empty_irq(void) {
    const int uart_nr = _uart->uart_nr;
    size_t queued = _tx_buffer->getSize();
    if(!queued) {
        UART_DISARM_TX_INTERRUPT(uart_nr);
        return;
    }

    size_t room = UART_GET_TX_FIFO_ROOM(uart_nr);
    int n = static_cast<int>((queued < room) ? queued : room);
    while(n--) {
        UART_TRANSMIT_CHAR(uart_nr, _tx_buffer->read());
    }
}
