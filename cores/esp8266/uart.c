/*
 uart.cpp - esp8266 UART HAL

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
#include "Arduino.h"
#include "uart.h"
#include "esp8266_peri.h"
#include "user_interface.h"

static int s_uart_debug_nr = UART0;

struct uart_ {
    int uart_nr;
    int baud_rate;
    bool rx_enabled;
    bool tx_enabled;
    uint8_t rx_pin;
    uint8_t tx_pin;
};

void uart_write_char(uart_t* uart, char c)
{
    if(uart == NULL || !uart->tx_enabled) {
        return;
    }
    while((USS(uart->uart_nr) >> USTXC) >= 0x7f);
    USF(uart->uart_nr) = c;
}

void uart_write(uart_t* uart, const char* buf, size_t size)
{
    if(uart == NULL || !uart->tx_enabled) {
        return;
    }
    while(size--) {
        uart_write_char(uart, *buf++);
    }
}

int uart_read_char(uart_t* uart)
{
    if(uart == NULL || !uart->rx_enabled) {
        return -1;
    }
    if (!uart_rx_available(uart)) {
        return -1;
    }
    return USF(uart->uart_nr) & 0xff;
}

size_t uart_rx_available(uart_t* uart)
{
    if(uart == NULL || !uart->rx_enabled) {
        return -1;
    }
    return (USS(uart->uart_nr) >> USRXC) & 0xff;
}

size_t uart_tx_free(uart_t* uart)
{
    if(uart == NULL || !uart->tx_enabled) {
        return 0;
    }
    return UART_TX_FIFO_SIZE - ((USS(uart->uart_nr) >> USTXC) & 0xff);
}

void uart_wait_tx_empty(uart_t* uart)
{
    if(uart == NULL || !uart->tx_enabled) {
        return;
    }
    while(((USS(uart->uart_nr) >> USTXC) & 0xff) > 0) {
        delay(0);
    }
}

void uart_flush(uart_t* uart)
{
    if(uart == NULL) {
        return;
    }

    uint32_t tmp = 0x00000000;
    if(uart->rx_enabled) {
        tmp |= (1 << UCRXRST);
    }

    if(uart->tx_enabled) {
        tmp |= (1 << UCTXRST);
    }

    USC0(uart->uart_nr) |= (tmp);
    USC0(uart->uart_nr) &= ~(tmp);
}

void uart_set_baudrate(uart_t* uart, int baud_rate)
{
    if(uart == NULL) {
        return;
    }
    uart->baud_rate = baud_rate;
    USD(uart->uart_nr) = (ESP8266_CLOCK / uart->baud_rate);
}

int uart_get_baudrate(uart_t* uart)
{
    if(uart == NULL) {
        return 0;
    }
    return uart->baud_rate;
}

uart_t* uart_init(int uart_nr, int baudrate, int config, int mode, int tx_pin)
{
    uart_t* uart = (uart_t*) malloc(sizeof(uart_t));
    if(uart == NULL) {
        return NULL;
    }

    uart->uart_nr = uart_nr;

    switch(uart->uart_nr) {
    case UART0:
        uart->rx_enabled = (mode != UART_TX_ONLY);
        uart->tx_enabled = (mode != UART_RX_ONLY);
        uart->rx_pin = (uart->rx_enabled)?3:255;
        if(uart->tx_enabled) {
            if (tx_pin == 2) {
                uart->tx_pin = 2;
                pinMode(uart->tx_pin, FUNCTION_4);
            } else {
                uart->tx_pin = 1;
                pinMode(uart->tx_pin, FUNCTION_0);
            }
        } else {
            uart->tx_pin = 255;
        }
        if(uart->rx_enabled) {
            pinMode(uart->rx_pin, SPECIAL);
        }
        IOSWAP &= ~(1 << IOSWAPU0);
        break;
    case UART1:
        // Note: uart_interrupt_handler does not support RX on UART 1.
        uart->rx_enabled = false;
        uart->tx_enabled = (mode != UART_RX_ONLY);
        uart->rx_pin = 255;
        uart->tx_pin = (uart->tx_enabled)?2:255;  // GPIO7 as TX not possible! See GPIO pins used by UART
        if(uart->tx_enabled) {
            pinMode(uart->tx_pin, SPECIAL);
        }
        break;
    case UART_NO:
    default:
        // big fail!
        free(uart);
        return NULL;
    }

    uart_set_baudrate(uart, baudrate);
    USC0(uart->uart_nr) = config;
    uart_flush(uart);
    USC1(uart->uart_nr) = 0;

    return uart;
}

void uart_uninit(uart_t* uart)
{
    if(uart == NULL) {
        return;
    }

    switch(uart->rx_pin) {
    case 3:
        pinMode(3, INPUT);
        break;
    case 13:
        pinMode(13, INPUT);
        break;
    }

    switch(uart->tx_pin) {
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

    free(uart);
}

void uart_swap(uart_t* uart, int tx_pin)
{
    if(uart == NULL) {
        return;
    }
    switch(uart->uart_nr) {
    case UART0:
        if(((uart->tx_pin == 1 || uart->tx_pin == 2) && uart->tx_enabled) || (uart->rx_pin == 3 && uart->rx_enabled)) {
            if(uart->tx_enabled) { //TX
                pinMode(uart->tx_pin, INPUT);
                uart->tx_pin = 15;
            }
            if(uart->rx_enabled) { //RX
                pinMode(uart->rx_pin, INPUT);
                uart->rx_pin = 13;
            }
            if(uart->tx_enabled) {
                pinMode(uart->tx_pin, FUNCTION_4);    //TX
            }
            if(uart->rx_enabled) {
                pinMode(uart->rx_pin, FUNCTION_4);    //RX
            }
            IOSWAP |= (1 << IOSWAPU0);
        } else {
            if(uart->tx_enabled) { //TX
                pinMode(uart->tx_pin, INPUT);
                uart->tx_pin = (tx_pin == 2)?2:1;
            }
            if(uart->rx_enabled) { //RX
                pinMode(uart->rx_pin, INPUT);
                uart->rx_pin = 3;
            }
            if(uart->tx_enabled) {
                pinMode(uart->tx_pin, (tx_pin == 2)?FUNCTION_4:SPECIAL);    //TX
            }
            if(uart->rx_enabled) {
                pinMode(3, SPECIAL);    //RX
            }
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

void uart_set_tx(uart_t* uart, int tx_pin)
{
    if(uart == NULL) {
        return;
    }
    switch(uart->uart_nr) {
    case UART0:
        if(uart->tx_enabled) {
            if (uart->tx_pin == 1 && tx_pin == 2) {
                pinMode(uart->tx_pin, INPUT);
                uart->tx_pin = 2;
                pinMode(uart->tx_pin, FUNCTION_4);
            } else if (uart->tx_pin == 2 && tx_pin != 2) {
                pinMode(uart->tx_pin, INPUT);
                uart->tx_pin = 1;
                pinMode(uart->tx_pin, SPECIAL);
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

void uart_set_pins(uart_t* uart, int tx, int rx)
{
    if(uart == NULL) {
        return;
    }

    if(uart->uart_nr == UART0) { // Only UART0 allows pin changes
        if(uart->tx_enabled && uart->tx_pin != tx) {
            if( rx == 13 && tx == 15) {
                uart_swap(uart, 15);
            } else if (rx == 3 && (tx == 1 || tx == 2)) {
                if (uart->rx_pin != rx) {
                    uart_swap(uart, tx);
                } else {
                    uart_set_tx(uart, tx);
                }
            }
        }
        if(uart->rx_enabled && uart->rx_pin != rx && rx == 13 && tx == 15) {
            uart_swap(uart, 15);
        }
    }
}


bool uart_tx_enabled(uart_t* uart)
{
    if(uart == NULL) {
        return false;
    }
    return uart->tx_enabled;
}

bool uart_rx_enabled(uart_t* uart)
{
    if(uart == NULL) {
        return false;
    }
    return uart->rx_enabled;
}


static void uart_ignore_char(char c)
{
}

static void uart0_write_char(char c)
{
    while(((USS(0) >> USTXC) & 0xff) >= 0x7F) {
        delay(0);
    }
    USF(0) = c;
}

static void uart1_write_char(char c)
{
    while(((USS(1) >> USTXC) & 0xff) >= 0x7F) {
        delay(0);
    }
    USF(1) = c;
}

void uart_set_debug(int uart_nr)
{
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

int uart_get_debug()
{
    return s_uart_debug_nr;
}
