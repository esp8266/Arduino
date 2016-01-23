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


void uart_write_char(uart_t* uart, char c) {
  if(uart == 0 || !uart->txEnabled)
    return;
  while((USS(uart->uart_nr) >> USTXC) >= 0x7f);
  USF(uart->uart_nr) = c;
}

void uart_write(uart_t* uart, const char* buf, size_t size) {
  if(uart == 0 || !uart->txEnabled)
    return;
  while(size--)
    uart_write_char(uart, *buf++);
}

uint8_t uart_read_char(uart_t* uart){
  if(uart == 0 || !uart->rxEnabled)
    return 0;
  return USF(uart->uart_nr) & 0xff;
}

uint8_t uart_rx_available(uart_t* uart){
  if(uart == 0 || !uart->rxEnabled)
    return 0;
  return (USS(uart->uart_nr) >> USRXC) & 0xff;
}

uint8_t uart_tx_free(uart_t* uart){
  if(uart == 0 || !uart->txEnabled)
    return 0;
  return UART_TX_FIFO_SIZE - ((USS(uart->uart_nr) >> USTXC) & 0xff);
}

void uart_wait_tx_empty(uart_t* uart){
  if(uart == 0 || !uart->txEnabled)
    return;
  while(((USS(uart->uart_nr) >> USTXC) & 0xff) > 0) delay(0);
}

void uart_flush(uart_t* uart) {
  if(uart == 0)
    return;

  uint32_t tmp = 0x00000000;
  if(uart->rxEnabled) {
    tmp |= (1 << UCRXRST);
  }

  if(uart->txEnabled) {
    tmp |= (1 << UCTXRST);
  }

  USC0(uart->uart_nr) |= (tmp);
  USC0(uart->uart_nr) &= ~(tmp);
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

// ####################################################################################################
// ####################################################################################################
// ####################################################################################################

uart_t* uart_init(int uart_nr, int baudrate, byte config, byte mode, uint8_t use_tx) {

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
      if(uart->rxEnabled) {
        if (use_tx == 2) {
          uart->txPin = 2;
          pinMode(uart->rxPin, FUNCTION_4);
        } else {
          uart->txPin = 1;
          pinMode(uart->rxPin, SPECIAL);
        }
      } else uart->txPin = 255;
      if(uart->txEnabled) pinMode(uart->txPin, SPECIAL);
      IOSWAP &= ~(1 << IOSWAPU0);
      break;
    case UART1:
      // Note: uart_interrupt_handler does not support RX on UART 1.
      uart->rxEnabled = false;
    uart->txEnabled = (mode != SERIAL_RX_ONLY);
    uart->rxPin = 255;
    uart->txPin = (uart->txEnabled)?2:255;  // GPIO7 as TX not possible! See GPIO pins used by UART
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
  uart_flush(uart);
  USC1(uart->uart_nr) = 0;

  return uart;
}

void uart_uninit(uart_t* uart) {
  if(uart == 0)
    return;

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

void uart_swap(uart_t* uart, uint8_t use_tx) {
  if(uart == 0)
    return;
  switch(uart->uart_nr) {
    case UART0:
      if(((uart->txPin == 1 || uart->txPin == 2) && uart->txEnabled) || (uart->rxPin == 3 && uart->rxEnabled)) {
        if(uart->txEnabled){ //TX
          pinMode(uart->txPin, INPUT);
          uart->txPin = 15;
        }
        if(uart->rxEnabled){ //RX
          pinMode(uart->rxPin, INPUT);
          uart->rxPin = 13;
        }
        if(uart->txEnabled) pinMode(uart->txPin, FUNCTION_4); //TX
        if(uart->rxEnabled) pinMode(uart->rxPin, FUNCTION_4); //RX
        IOSWAP |= (1 << IOSWAPU0);
      } else {
        if(uart->txEnabled){ //TX
          pinMode(uart->txPin, INPUT);
          uart->txPin = (use_tx == 2)?2:1;
        }
        if(uart->rxEnabled){ //RX
          pinMode(uart->rxPin, INPUT);
          uart->rxPin = 3;
        }
        if(uart->txEnabled) pinMode(uart->txPin, (use_tx == 2)?FUNCTION_4:SPECIAL); //TX
        if(uart->rxEnabled) pinMode(3, SPECIAL); //RX
        IOSWAP &= ~(1 << IOSWAPU0);
      }

      break;
    case UART1:
      // Currently no swap possible! See GPIO pins used by UART
      break;
    default:
      break;
  }
}

void uart_set_tx(uart_t* uart, uint8_t use_tx) {
  if(uart == 0)
    return;
  switch(uart->uart_nr) {
    case UART0:
      if(uart->txEnabled) {
        if (uart->txPin == 1 && use_tx == 2) {
          pinMode(uart->txPin, INPUT);
          uart->txPin = 2;
          pinMode(uart->txPin, FUNCTION_4);
        } else if (uart->txPin == 2 && use_tx != 2) {
          pinMode(uart->txPin, INPUT);
          uart->txPin = 1;
          pinMode(uart->txPin, SPECIAL);
        }
      }

      break;
    case UART1:
      // GPIO7 as TX not possible! See GPIO pins used by UART
      break;
    default:
      break;
  }
}

void uart_set_pins(uart_t* uart, uint8_t tx, uint8_t rx) {
  if(uart == 0)
    return;

  if(uart->uart_nr == UART0) { // Only UART0 allows pin changes
    if(uart->txEnabled && uart->txPin != tx) {
      if( rx == 13 && tx == 15) {
        uart_swap(uart, 15);
      } else if (rx == 3 && (tx == 1 || tx == 2)) {
        if (uart->rxPin != rx) uart_swap(uart, tx);
        else uart_set_tx(uart, tx);
      }
    }
    if(uart->rxEnabled && uart->rxPin != rx && rx == 13 && tx == 15) {
      uart_swap(uart, 15);
    }
  }
}

// ####################################################################################################
// ####################################################################################################
// ####################################################################################################

void uart_ignore_char(char c) {}

void uart0_write_char(char c) {
  while(((USS(0) >> USTXC) & 0xff) >= 0x7F) delay(0);
  USF(0) = c;
}

void uart1_write_char(char c) {
  while(((USS(1) >> USTXC) & 0xff) >= 0x7F) delay(0);
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

HardwareSerial::HardwareSerial(int uart_nr)
  : _uart_nr(uart_nr)
  , _uart(0)
{}

void HardwareSerial::begin(unsigned long baud, byte config, byte mode, uint8_t use_tx) {
  if(uart_get_debug() == _uart_nr)
    uart_set_debug(UART_NO);

  if (_uart)
    os_free(_uart);

  _uart = uart_init(_uart_nr, baud, config, mode, use_tx);
}

void HardwareSerial::end() {
  if(uart_get_debug() == _uart_nr)
    uart_set_debug(UART_NO);

  uart_uninit(_uart);
}

void HardwareSerial::swap(uint8_t use_tx) {
  if(_uart == 0)
    return;
  uart_swap(_uart, use_tx);
}

void HardwareSerial::set_tx(uint8_t use_tx) {
  if(_uart == 0)
    return;
  uart_set_tx(_uart, use_tx);
}

void HardwareSerial::pins(uint8_t tx, uint8_t rx) {
  if(_uart == 0)
    return;
  uart_set_pins(_uart, tx, rx);
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
  return _uart != 0 && _uart->txEnabled;
}

bool ICACHE_RAM_ATTR HardwareSerial::isRxEnabled(void) {
  return _uart != 0 && _uart->rxEnabled;
}

int HardwareSerial::available(void) {
  if(_uart == 0 || !_uart->rxEnabled)
      return 0;

  int result = static_cast<int>(uart_rx_available(_uart));
  if (!result) {
    optimistic_yield(USD(_uart->uart_nr) / 128);
  }
  return result;
}

int HardwareSerial::peek(void) {
  return -1;
}

int HardwareSerial::read(void) {
  if(_uart == 0 || !_uart->rxEnabled)
    return -1;

  return static_cast<int>(uart_read_char(_uart));
}

int HardwareSerial::availableForWrite(void) {
  if(_uart == 0 || !_uart->txEnabled)
    return 0;

  return static_cast<int>(uart_tx_free(_uart));
}

void HardwareSerial::flush() {
  if(_uart == 0 || !_uart->txEnabled)
    return;

  uart_wait_tx_empty(_uart);
}

size_t HardwareSerial::write(uint8_t c) {
  if(_uart == 0 || !_uart->txEnabled)
    return 0;

  uart_write_char(_uart, c);
  return 1;
}

HardwareSerial::operator bool() const {
  return _uart != 0;
}
