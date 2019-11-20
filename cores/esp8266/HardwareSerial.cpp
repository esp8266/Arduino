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
#include <PolledTimeout.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include "Esp.h"

HardwareSerial::HardwareSerial(int uart_nr)
    : _uart_nr(uart_nr), _rx_size(256)
{}

void HardwareSerial::begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin, bool invert)
{
    end();
    _uart = uart_init(_uart_nr, baud, (int) config, (int) mode, tx_pin, _rx_size);
    if (0 == _uart_nr && invert)
    {
        U0C0 |= BIT(UCDTRI) | BIT(UCRTSI) | BIT(UCTXI) | BIT(UCDSRI) | BIT(UCCTSI) | BIT(UCRXI);
    }
#if defined(DEBUG_ESP_PORT) && !defined(NDEBUG)
    if (static_cast<void*>(this) == static_cast<void*>(&DEBUG_ESP_PORT))
    {
        setDebugOutput(true);
        println();
        println(ESP.getFullVersion());
    }
#endif
}

void HardwareSerial::end()
{
    if(uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
    }

    uart_uninit(_uart);
    _uart = NULL;
}

void HardwareSerial::updateBaudRate(unsigned long baud)
{
    if(!_uart) {
        return;
    }

    uart_set_baudrate(_uart, baud);
}

size_t HardwareSerial::setRxBufferSize(size_t size){
    if(_uart) {
        _rx_size = uart_resize_rx_buffer(_uart, size);
    } else {
        _rx_size = size;
    }
    return _rx_size;
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

int HardwareSerial::available(void)
{
    int result = static_cast<int>(uart_rx_available(_uart));
    if (!result) {
        optimistic_yield(10000);
    }
    return result;
}

void HardwareSerial::flush()
{
    uint8_t bit_length = 0;
    if(!_uart || !uart_tx_enabled(_uart)) {
        return;
    }

    bit_length = uart_get_bit_length(_uart_nr); // data width, parity and stop
    uart_wait_tx_empty(_uart);
    //Workaround for a bug in serial not actually being finished yet
    //Wait for 8 data bits, 1 parity and 2 stop bits, just in case
    delayMicroseconds(bit_length * 1000000 / uart_get_baudrate(_uart) + 1);
}

void HardwareSerial::startDetectBaudrate()
{
    uart_start_detect_baudrate(_uart_nr);
}

unsigned long HardwareSerial::testBaudrate()
{
    return uart_detect_baudrate(_uart_nr);
}

unsigned long HardwareSerial::detectBaudrate(time_t timeoutMillis)
{
    esp8266::polledTimeout::oneShotFastMs timeOut(timeoutMillis);
    unsigned long detectedBaudrate;
    while (!timeOut) {
        if ((detectedBaudrate = testBaudrate())) {
          break;
        }
        yield();
        delay(100);
    }
    return detectedBaudrate;
}

size_t HardwareSerial::readBytes(char* buffer, size_t size)
{
    size_t got = 0;

    while (got < size)
    {
        esp8266::polledTimeout::oneShotFastMs timeOut(_timeout);
        size_t avail;
        while ((avail = available()) == 0 && !timeOut);
        if (avail == 0)
            break;
        got += read(buffer + got, std::min(size - got, avail));
    }
    return got;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
HardwareSerial Serial(UART0);
#endif
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL1)
HardwareSerial Serial1(UART1);
#endif
