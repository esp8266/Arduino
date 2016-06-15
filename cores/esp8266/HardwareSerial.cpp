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
#include "HardwareSerial.h"


HardwareSerial::HardwareSerial(int uart_nr)
    : _uart_nr(uart_nr)
{}

void HardwareSerial::begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin)
{
    if(uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
    }

    if (_uart) {
        free(_uart);
    }

    _uart = uart_init(_uart_nr, baud, (int) config, (int) mode, tx_pin);
    _peek_char = -1;
}

void HardwareSerial::end()
{
    if(uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
    }

    uart_uninit(_uart);
    _uart = NULL;
}

void HardwareSerial::swap(uint8_t tx_pin)
{
    if(!_uart) {
        return;
    }
    uart_swap(_uart, tx_pin);
}

void HardwareSerial::set_tx(uint8_t tx_pin)
{
    if(!_uart) {
        return;
    }
    uart_set_tx(_uart, tx_pin);
}

void HardwareSerial::pins(uint8_t tx, uint8_t rx)
{
    if(!_uart) {
        return;
    }
    uart_set_pins(_uart, tx, rx);
}

void HardwareSerial::setDebugOutput(bool en)
{
    if(!_uart) {
        return;
    }
    if(en) {
        if(uart_tx_enabled(_uart)) {
            uart_set_debug(_uart_nr);
        } else {
            uart_set_debug(UART_NO);
        }
    } else {
        // disable debug for this interface
        if(uart_get_debug() == _uart_nr) {
            uart_set_debug(UART_NO);
        }
    }
}

bool HardwareSerial::isTxEnabled(void)
{
    return _uart && uart_tx_enabled(_uart);
}

bool HardwareSerial::isRxEnabled(void)
{
    return _uart && uart_rx_enabled(_uart);
}

int HardwareSerial::available(void)
{
    if(!_uart || !uart_rx_enabled(_uart)) {
        return 0;
    }

    int result = static_cast<int>(uart_rx_available(_uart));
    if (_peek_char != -1) {
        result += 1;
    }
    if (!result) {
        optimistic_yield(10000);
    }
    return result;
}

int HardwareSerial::peek(void)
{
    if (_peek_char != -1) {
        return _peek_char;
    }
    // this may return -1, but that's okay
    _peek_char = uart_read_char(_uart);
    return _peek_char;
}

int HardwareSerial::read(void)
{
    if(!_uart || !uart_rx_enabled(_uart)) {
        return -1;
    }

    if (_peek_char != -1) {
        auto tmp = _peek_char;
        _peek_char = -1;
        return tmp;
    }
    return uart_read_char(_uart);
}

int HardwareSerial::availableForWrite(void)
{
    if(!_uart || !uart_tx_enabled(_uart)) {
        return 0;
    }

    return static_cast<int>(uart_tx_free(_uart));
}

void HardwareSerial::flush()
{
    if(!_uart || !uart_tx_enabled(_uart)) {
        return;
    }

    uart_wait_tx_empty(_uart);
}

size_t HardwareSerial::write(uint8_t c)
{
    if(!_uart || !uart_tx_enabled(_uart)) {
        return 0;
    }

    uart_write_char(_uart, c);
    return 1;
}

int HardwareSerial::baudRate(void)
{
    // Null pointer on _uart is checked by SDK
    return uart_get_baudrate(_uart);
}


HardwareSerial::operator bool() const
{
    return _uart != 0;
}


HardwareSerial Serial(UART0);
HardwareSerial Serial1(UART1);
