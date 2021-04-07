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
#include <pgmspace.h>
#include "gdb_hooks.h"
#include "uart.h"
#include "esp8266_peri.h"
#include "user_interface.h"
#include "uart_register.h"

#define MODE2WIDTH(mode) (((mode%16)>>2)+5)
#define MODE2STOP(mode) (((mode)>>5)+1)
#define MODE2PARITY(mode) (mode%4)

/*
  Some general architecture for GDB integration with the UART to enable
  serial debugging.

  UART1 is transmit only and can never be used by GDB.

  When gdbstub_has_uart_isr_control() (only true in the case GDB is enabled),
  UART0 needs to be under the control of the GDB stub for enable/disable/irq
  (but speed, parity, etc. still alllowable).  Basically, GDB needs to make
  sure that UART0 is never really disabled.

  GDB sets up UART0 with a fifo and a 2-character timeout during init.  This
  is required to ensure that GDBStub can check every character coming in, even
  if it is not read by the user app or if the commands don't hit the FIFO
  interrupt level.  It checks every character that comes in, and if GDB isn't
  active just passes them to the user RAM FIFO unless it's a Ctrl-C (0x03).

  GDBStub doesn't care about the struct uart_*, and allocating it or freeing
  it has no effect (so you can do Serial.end() and free the uart...but as
  mentioned above even if you Serial.end, the actual UART0 HW will still be
  kept running to enable GDB to do commands.
*/

extern "C" {

static int s_uart_debug_nr = UART0;

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
    bool rx_error;
    uint8_t rx_pin;
    uint8_t tx_pin;
    struct uart_rx_buffer_ * rx_buffer;
};


/*
   In the context of the naming conventions in this file, "_unsafe" means two things:
   1. the input arguments are not checked. It is up to the caller to check argument sanity.
   2. The function body is not interrupt-safe, i.e.: the isr could fire anywhen during the
      body execution, leading to corruption of the data shared between the body and the isr
      (parts of the rx_buffer).

   The unsafe versions of the functions are private to this TU. There are "safe" versions that
   wrap the unsafe ones with disabling/enabling of the uart interrupt for safe public use.
*/



// called by ISR
inline size_t IRAM_ATTR
uart_rx_fifo_available(const int uart_nr)
{
    return (USS(uart_nr) >> USRXC) & 0xFF;
}


/**********************************************************/
/************ UNSAFE FUNCTIONS ****************************/
/**********************************************************/
inline size_t
uart_rx_buffer_available_unsafe(const struct uart_rx_buffer_ * rx_buffer)
{
    if(rx_buffer->wpos < rx_buffer->rpos)
      return (rx_buffer->wpos + rx_buffer->size) - rx_buffer->rpos;

    return rx_buffer->wpos - rx_buffer->rpos;
}

inline size_t
uart_rx_available_unsafe(uart_t* uart)
{
    return uart_rx_buffer_available_unsafe(uart->rx_buffer) + uart_rx_fifo_available(uart->uart_nr);
}

//#define UART_DISCARD_NEWEST

// Copy all the rx fifo bytes that fit into the rx buffer
// called by ISR
inline void IRAM_ATTR
uart_rx_copy_fifo_to_buffer_unsafe(uart_t* uart)
{
    struct uart_rx_buffer_ *rx_buffer = uart->rx_buffer;

    while(uart_rx_fifo_available(uart->uart_nr))
    {
        size_t nextPos = (rx_buffer->wpos + 1) % rx_buffer->size;
        if(nextPos == rx_buffer->rpos)
        {
            if (!uart->rx_overrun)
            {
                uart->rx_overrun = true;
                //os_printf_plus(overrun_str);
            }

            // a choice has to be made here,
            // do we discard newest or oldest data?
#ifdef UART_DISCARD_NEWEST
            // discard newest data
            // Stop copying if rx buffer is full
            USF(uart->uart_nr);
            break;
#else
            // discard oldest data
            if (++rx_buffer->rpos == rx_buffer->size)
                rx_buffer->rpos = 0;
#endif
        }
        uint8_t data = USF(uart->uart_nr);
        rx_buffer->buffer[rx_buffer->wpos] = data;
        rx_buffer->wpos = nextPos;
    }
}

inline int
uart_peek_char_unsafe(uart_t* uart)
{
    if (!uart_rx_available_unsafe(uart))
        return -1;

    //without the following if statement and body, there is a good chance of a fifo overrun
    if (uart_rx_buffer_available_unsafe(uart->rx_buffer) == 0)
        // hw fifo can't be peeked, data need to be copied to sw
        uart_rx_copy_fifo_to_buffer_unsafe(uart);

    return uart->rx_buffer->buffer[uart->rx_buffer->rpos];
}

// taking data straight from hw fifo: loopback-test BW jumps by 19%
inline int
uart_read_char_unsafe(uart_t* uart)
{
    if (uart_rx_buffer_available_unsafe(uart->rx_buffer))
    {
        // take oldest sw data
        int ret = uart->rx_buffer->buffer[uart->rx_buffer->rpos];
        uart->rx_buffer->rpos = (uart->rx_buffer->rpos + 1) % uart->rx_buffer->size;
        return ret;
    }
    // unavailable
    return -1;
}

uint8_t
uart_get_bit_length(const int uart_nr)
{
    // return bit length from uart mode, +1 for the start bit which is always there. 
    return MODE2WIDTH(USC0(uart_nr)) + MODE2PARITY(USC0(uart_nr)) + MODE2STOP(USC0(uart_nr)) + 1;
}

size_t 
uart_rx_available(uart_t* uart)
{
    if(uart == NULL || !uart->rx_enabled)
        return 0;

    ETS_UART_INTR_DISABLE();
    int uartrxbufferavailable = uart_rx_buffer_available_unsafe(uart->rx_buffer);
    ETS_UART_INTR_ENABLE();

    return uartrxbufferavailable + uart_rx_fifo_available(uart->uart_nr);
}

int
uart_peek_char(uart_t* uart)
{
    if(uart == NULL || !uart->rx_enabled)
        return -1;

    ETS_UART_INTR_DISABLE(); //access to rx_buffer can be interrupted by the isr (similar to a critical section), so disable interrupts here
    int ret = uart_peek_char_unsafe(uart);
    ETS_UART_INTR_ENABLE();
    return ret;
}

// return number of byte accessible by uart_peek_buffer()
size_t uart_peek_available (uart_t* uart)
{
    // path for further optimization:
    // - return already copied buffer pointer (= older data)
    // - or return fifo when buffer is empty but then any move from fifo to
    //   buffer should be blocked until peek_consume is called

    ETS_UART_INTR_DISABLE();
    uart_rx_copy_fifo_to_buffer_unsafe(uart);
    auto rpos = uart->rx_buffer->rpos;
    auto wpos = uart->rx_buffer->wpos;
    ETS_UART_INTR_ENABLE();
    if(wpos < rpos)
        return uart->rx_buffer->size - rpos;
    return wpos - rpos;
}

// return a pointer to available data buffer (size = available())
// semantic forbids any kind of read() between peekBuffer() and peekConsume()
const char* uart_peek_buffer (uart_t* uart)
{
    return (const char*)&uart->rx_buffer->buffer[uart->rx_buffer->rpos];
}

// consume bytes after use (see uart_peek_buffer)
void uart_peek_consume (uart_t* uart, size_t consume)
{
    ETS_UART_INTR_DISABLE();
    uart->rx_buffer->rpos += consume;
    if (uart->rx_buffer->rpos >= uart->rx_buffer->size)
        uart->rx_buffer->rpos -= uart->rx_buffer->size;
    ETS_UART_INTR_ENABLE();
}

int
uart_read_char(uart_t* uart)
{
    uint8_t ret;
    return uart_read(uart, (char*)&ret, 1)? ret: -1;
}

// loopback-test BW jumps by 190%
size_t
uart_read(uart_t* uart, char* userbuffer, size_t usersize)
{
    if(uart == NULL || !uart->rx_enabled)
        return 0;

    size_t ret = 0;
    ETS_UART_INTR_DISABLE();

    while (ret < usersize && uart_rx_available_unsafe(uart))
    {
        if (!uart_rx_buffer_available_unsafe(uart->rx_buffer))
        {
            // no more data in sw buffer, take them from hw fifo
            while (ret < usersize && uart_rx_fifo_available(uart->uart_nr))
                userbuffer[ret++] = USF(uart->uart_nr);

	    // no more sw/hw data available
            break;
        }

        // pour sw buffer to user's buffer
        // get largest linear length from sw buffer
        size_t chunk = uart->rx_buffer->rpos < uart->rx_buffer->wpos?
                           uart->rx_buffer->wpos - uart->rx_buffer->rpos:
                           uart->rx_buffer->size - uart->rx_buffer->rpos;
        if (ret + chunk > usersize)
            chunk = usersize - ret;
        memcpy(userbuffer + ret, uart->rx_buffer->buffer + uart->rx_buffer->rpos, chunk);
        uart->rx_buffer->rpos = (uart->rx_buffer->rpos + chunk) % uart->rx_buffer->size;
        ret += chunk;
    }

    ETS_UART_INTR_ENABLE();
    return ret;
}

// When GDB is running, this is called one byte at a time to stuff the user FIFO
// instead of the uart_isr...uart_rx_copy_fifo_to_buffer_unsafe()
// Since we've already read the bytes from the FIFO, can't use that
// function directly and need to implement it bytewise here
static void IRAM_ATTR uart_isr_handle_data(void* arg, uint8_t data)
{
    uart_t* uart = (uart_t*)arg;
    if(uart == NULL || !uart->rx_enabled) {
        return;
    }

// Copy all the rx fifo bytes that fit into the rx buffer
// called by ISR
    struct uart_rx_buffer_ *rx_buffer = uart->rx_buffer;

    size_t nextPos = (rx_buffer->wpos + 1) % rx_buffer->size;
    if(nextPos == rx_buffer->rpos)
    {
        uart->rx_overrun = true;
        //os_printf_plus(overrun_str);

        // a choice has to be made here,
        // do we discard newest or oldest data?
#ifdef UART_DISCARD_NEWEST
        // discard newest data
        // Stop copying if rx buffer is full
        return;
#else
        // discard oldest data
        if (++rx_buffer->rpos == rx_buffer->size)
            rx_buffer->rpos = 0;
#endif
    }
    rx_buffer->buffer[rx_buffer->wpos] = data;
    rx_buffer->wpos = nextPos;

    // Check the UART flags and note hardware overflow/etc.
    uint32_t usis = USIS(uart->uart_nr);

    if(usis & (1 << UIOF))
        uart->rx_overrun = true;

    if (usis & ((1 << UIFR) | (1 << UIPE) | (1 << UITO)))
        uart->rx_error = true;

    USIC(uart->uart_nr) = usis;
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
    ETS_UART_INTR_DISABLE();
    while(uart_rx_available_unsafe(uart) && new_wpos < new_size)
        new_buf[new_wpos++] = uart_read_char_unsafe(uart); //if uart_rx_available_unsafe() returns non-0, uart_read_char_unsafe() can't return -1
    if (new_wpos == new_size)
        new_wpos = 0;

    uint8_t * old_buf = uart->rx_buffer->buffer;
    uart->rx_buffer->rpos = 0;
    uart->rx_buffer->wpos = new_wpos;
    uart->rx_buffer->size = new_size;
    uart->rx_buffer->buffer = new_buf;
    ETS_UART_INTR_ENABLE();
    free(old_buf);
    return uart->rx_buffer->size;
}

size_t
uart_get_rx_buffer_size(uart_t* uart)
{
    return uart && uart->rx_enabled? uart->rx_buffer->size: 0;
}

// The default ISR handler called when GDB is not enabled
void IRAM_ATTR
uart_isr(void * arg, void * frame)
{
    (void) frame;
    uart_t* uart = (uart_t*)arg;
    uint32_t usis = USIS(uart->uart_nr);

    if(uart == NULL || !uart->rx_enabled)
    {
        USIC(uart->uart_nr) = usis;
        ETS_UART_INTR_DISABLE();
        return;
    }

    if(usis & (1 << UIFF))
        uart_rx_copy_fifo_to_buffer_unsafe(uart);

    if(usis & (1 << UIOF))
    {
        uart->rx_overrun = true;
        //os_printf_plus(overrun_str);
    }

    if (usis & ((1 << UIFR) | (1 << UIPE) | (1 << UITO)))
        uart->rx_error = true;

    USIC(uart->uart_nr) = usis;
}

static void
uart_start_isr(uart_t* uart)
{
    if(uart == NULL || !uart->rx_enabled)
        return;

    if(gdbstub_has_uart_isr_control()) {
        gdbstub_set_uart_isr_callback(uart_isr_handle_data,  (void *)uart);
        return;
    }

    // UCFFT value is when the RX fifo full interrupt triggers.  A value of 1
    // triggers the IRS very often.  A value of 127 would not leave much time
    // for ISR to clear fifo before the next byte is dropped.  So pick a value
    // in the middle.
    // update: loopback test @ 3Mbauds/8n1 (=2343Kibits/s):
    // - 4..120 give > 2300Kibits/s
    // - 1, 2, 3 are below
    // was 100, use 16 to stay away from overrun
    #define INTRIGG 16

    //was:USC1(uart->uart_nr) = (INTRIGG << UCFFT) | (0x02 << UCTOT) | (1 <<UCTOE);
    USC1(uart->uart_nr) = (INTRIGG << UCFFT);
    USIC(uart->uart_nr) = 0xffff;
    //was: USIE(uart->uart_nr) = (1 << UIFF) | (1 << UIFR) | (1 << UITO);
    // UIFF: rx fifo full
    // UIOF: rx fifo overflow (=overrun)
    // UIFR: frame error
    // UIPE: parity error
    // UITO: rx fifo timeout
    USIE(uart->uart_nr) = (1 << UIFF) | (1 << UIOF) | (1 << UIFR) | (1 << UIPE) | (1 << UITO);
    ETS_UART_INTR_ATTACH(uart_isr,  (void *)uart);
    ETS_UART_INTR_ENABLE();
}

static void
uart_stop_isr(uart_t* uart)
{
    if(uart == NULL || !uart->rx_enabled)
        return;

    if(gdbstub_has_uart_isr_control()) {
        gdbstub_set_uart_isr_callback(NULL, NULL);
        return;
    }

    ETS_UART_INTR_DISABLE();
    USC1(uart->uart_nr) = 0;
    USIC(uart->uart_nr) = 0xffff;
    USIE(uart->uart_nr) = 0;
    ETS_UART_INTR_ATTACH(NULL, NULL);
}

/*
  Reference for uart_tx_fifo_available() and uart_tx_fifo_full():
  -Espressif Techinical Reference doc, chapter 11.3.7
  -tools/sdk/uart_register.h
  -cores/esp8266/esp8266_peri.h
  */
inline size_t
uart_tx_fifo_available(const int uart_nr)
{
    return (USS(uart_nr) >> USTXC) & 0xff;
}

inline bool
uart_tx_fifo_full(const int uart_nr)
{
    return uart_tx_fifo_available(uart_nr) >= 0x7f;
}


static void
uart_do_write_char(const int uart_nr, char c)
{
    while(uart_tx_fifo_full(uart_nr));

    USF(uart_nr) = c;
}

size_t
uart_write_char(uart_t* uart, char c)
{
    if(uart == NULL || !uart->tx_enabled)
        return 0;

    if(gdbstub_has_uart_isr_control() && uart->uart_nr == UART0) {
        gdbstub_write_char(c);
        return 1;
    }
    uart_do_write_char(uart->uart_nr, c);
    return 1;
}

size_t
uart_write(uart_t* uart, const char* buf, size_t size)
{
    if(uart == NULL || !uart->tx_enabled)
        return 0;

    if(gdbstub_has_uart_isr_control() && uart->uart_nr == UART0) {
        gdbstub_write(buf, size);
        return 0;
    }

    size_t ret = size;
    const int uart_nr = uart->uart_nr;
    while (size--) {
        uart_do_write_char(uart_nr, pgm_read_byte(buf++));
        yield();
    }

    return ret;
}


size_t
uart_tx_free(uart_t* uart)
{
    if(uart == NULL || !uart->tx_enabled)
        return 0;

    return UART_TX_FIFO_SIZE - uart_tx_fifo_available(uart->uart_nr);
}

void
uart_wait_tx_empty(uart_t* uart)
{
    if(uart == NULL || !uart->tx_enabled)
        return;

    while(uart_tx_fifo_available(uart->uart_nr) > 0)
        delay(0);

}

void
uart_flush(uart_t* uart)
{
    if(uart == NULL)
        return;

    uint32_t tmp = 0x00000000;
    if(uart->rx_enabled)
    {
        tmp |= (1 << UCRXRST);
        ETS_UART_INTR_DISABLE();
        uart->rx_buffer->rpos = 0;
        uart->rx_buffer->wpos = 0;
        ETS_UART_INTR_ENABLE();
    }

    if(uart->tx_enabled)
        tmp |= (1 << UCTXRST);

    if(!gdbstub_has_uart_isr_control() || uart->uart_nr != UART0) {
        USC0(uart->uart_nr) |= (tmp);
        USC0(uart->uart_nr) &= ~(tmp);
    }
}

void
uart_set_baudrate(uart_t* uart, int baud_rate)
{
    if(uart == NULL)
        return;

    uart->baud_rate = baud_rate;
    USD(uart->uart_nr) = (ESP8266_CLOCK / uart->baud_rate);
}

int
uart_get_baudrate(uart_t* uart)
{
    if(uart == NULL)
        return 0;

    return uart->baud_rate;
}

uart_t*
uart_init(int uart_nr, int baudrate, int config, int mode, int tx_pin, size_t rx_size, bool invert)
{
    uart_t* uart = (uart_t*) malloc(sizeof(uart_t));
    if(uart == NULL)
        return NULL;

    uart->uart_nr = uart_nr;
    uart->rx_overrun = false;
    uart->rx_error = false;

    switch(uart->uart_nr)
    {
    case UART0:
        ETS_UART_INTR_DISABLE();
        if(!gdbstub_has_uart_isr_control()) {
            ETS_UART_INTR_ATTACH(NULL, NULL);
        }
        uart->rx_enabled = (mode != UART_TX_ONLY);
        uart->tx_enabled = (mode != UART_RX_ONLY);
        uart->rx_pin = (uart->rx_enabled)?3:255;
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
            pinMode(uart->rx_pin, SPECIAL);
        }
        if(uart->tx_enabled)
        {
            if (tx_pin == 2)
            {
                uart->tx_pin = 2;
                pinMode(uart->tx_pin, FUNCTION_4);
            }
            else
            {
                uart->tx_pin = 1;
                pinMode(uart->tx_pin, FUNCTION_0);
            }
        }
        else
        {
            uart->tx_pin = 255;
        }
        IOSWAP &= ~(1 << IOSWAPU0);
        break;

    case UART1:
        // Note: uart_interrupt_handler does not support RX on UART 1.
        uart->rx_enabled = false;
        uart->tx_enabled = (mode != UART_RX_ONLY);
        uart->rx_pin = 255;
        uart->tx_pin = (uart->tx_enabled)?2:255;  // GPIO7 as TX not possible! See GPIO pins used by UART
        if(uart->tx_enabled)
            pinMode(uart->tx_pin, SPECIAL);

        break;

    case UART_NO:
    default:
        // big fail!
        free(uart);
        return NULL;
    }

    uart_set_baudrate(uart, baudrate);
    if(uart->uart_nr == UART0 && invert)
    {
        config |= BIT(UCDTRI) | BIT(UCRTSI) | BIT(UCTXI) | BIT(UCDSRI) | BIT(UCCTSI) | BIT(UCRXI);
    }
    USC0(uart->uart_nr) = config;

    if(!gdbstub_has_uart_isr_control() || uart->uart_nr != UART0) {
        uart_flush(uart);
        USC1(uart->uart_nr) = 0;
        USIC(uart->uart_nr) = 0xffff;
        USIE(uart->uart_nr) = 0;
    }
    if(uart->uart_nr == UART0) {
        if(uart->rx_enabled) {
            uart_start_isr(uart);
        }
        if(gdbstub_has_uart_isr_control()) {
            ETS_UART_INTR_ENABLE(); // Undo the disable in the switch() above
        }
    }

    return uart;
}

void
uart_uninit(uart_t* uart)
{
    if(uart == NULL)
        return;

    uart_stop_isr(uart);

    if(uart->tx_enabled && (!gdbstub_has_uart_isr_control() || uart->uart_nr != UART0)) {
        switch(uart->tx_pin)
        {
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
    }

    if(uart->rx_enabled) {
        free(uart->rx_buffer->buffer);
        free(uart->rx_buffer);
        if(!gdbstub_has_uart_isr_control()) {
            switch(uart->rx_pin)
            {
            case 3:
                pinMode(3, INPUT);
                break;
            case 13:
                pinMode(13, INPUT);
                break;
            }
        }
    }
    free(uart);
}

bool
uart_swap(uart_t* uart, int tx_pin)
{
    if(uart == NULL)
        return false;

    switch(uart->uart_nr)
    {
    case UART0:
        if(((uart->tx_pin == 1 || uart->tx_pin == 2) && uart->tx_enabled) || (uart->rx_pin == 3 && uart->rx_enabled))
        {
            if(uart->tx_enabled) //TX
            {
                pinMode(uart->tx_pin, INPUT);
                uart->tx_pin = 15;
                pinMode(uart->tx_pin, FUNCTION_4);
            }
            if(uart->rx_enabled) //RX
            {
                pinMode(uart->rx_pin, INPUT);
                uart->rx_pin = 13;
                pinMode(uart->rx_pin, FUNCTION_4);
            }

            IOSWAP |= (1 << IOSWAPU0);
            return true;
        }
        else
        {
            if(uart->tx_enabled) //TX
            {
                pinMode(uart->tx_pin, INPUT);
                uart->tx_pin = (tx_pin == 2)?2:1;
                pinMode(uart->tx_pin, (tx_pin == 2)?FUNCTION_4:SPECIAL);
            }
            if(uart->rx_enabled) //RX
            {
                pinMode(uart->rx_pin, INPUT);
                uart->rx_pin = 3;
                pinMode(3, SPECIAL);
            }

            IOSWAP &= ~(1 << IOSWAPU0);
            return true;
        }
        break;
    case UART1:
        // Currently no swap possible! See GPIO pins used by UART
        break;
    default:
        break;
    }
    return false;
}

bool
uart_set_tx(uart_t* uart, int tx_pin)
{
    if(uart == NULL)
        return false;

    switch(uart->uart_nr)
    {
    case UART0:
        if(uart->tx_enabled)
        {
            if (uart->tx_pin == tx_pin)
            {
                return true;
            }
            else if (tx_pin == 1 || tx_pin == 2)
            {
                pinMode(uart->tx_pin, INPUT);
                uart->tx_pin = tx_pin;
                pinMode(uart->tx_pin, tx_pin == 1 ? SPECIAL : FUNCTION_4);
                return true;
            }
        }

        break;
    case UART1:
        // GPIO7 as TX not possible! See GPIO pins used by UART
        break;
    default:
        break;
    }
    return false;
}

bool
uart_set_pins(uart_t* uart, int tx, int rx)
{
    if(uart == NULL)
        return false;

    if(uart->uart_nr != UART0) // Only UART0 allows pin changes
        return false;

    if(uart->tx_enabled && uart->tx_pin != tx)
    {
        if( rx == 13 && tx == 15)
        {
            if (!uart_swap(uart, 15))
                return false;
        }
        else if (rx == 3 && (tx == 1 || tx == 2))
        {
            if (uart->rx_pin != rx)
            {
                if (!uart_swap(uart, tx))
                    return false;
            }
            else
            {
                if (!uart_set_tx(uart, tx))
                    return false;
            }
        }
        else
            return false;
    }

    if (uart->rx_enabled && uart->rx_pin != rx)
    {
        if (rx == 13 && tx == 15)
        {
            if (!uart_swap(uart, 15))
                return false;
        }
        else
            return false;
    }

    return true;
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
uart_has_overrun (uart_t* uart)
{
    if (uart == NULL || !uart->rx_overrun)
        return false;

    // clear flag
    uart->rx_overrun = false;
    return true;
}

bool
uart_has_rx_error (uart_t* uart)
{
    if (uart == NULL || !uart->rx_error)
        return false;

    // clear flag
    uart->rx_error = false;
    return true;
}

static void
uart_ignore_char(char c)
{
    (void) c;
}

inline void
uart_write_char_delay(const int uart_nr, char c)
{
    while(uart_tx_fifo_full(uart_nr))
        delay(0);

    USF(uart_nr) = c;

}

static void
uart0_write_char(char c)
{
    uart_write_char_delay(0, c);
}

static void
uart1_write_char(char c)
{
    uart_write_char_delay(1, c);
}

void
uart_set_debug(int uart_nr)
{
    s_uart_debug_nr = uart_nr;
    fp_putc_t func = NULL;
    switch(s_uart_debug_nr) 
    {
    case UART0:
        func = &uart0_write_char;
        // This selects the UART for ROM ets_putc which is used by
        // ::printf, ets_printf_P in core_esp_postmortem.cpp and others.
        // Has a side effect of clearing RX FIFO for UART0
        uart_buff_switch(0);
        break;
    case UART1:
        func = &uart1_write_char;
        uart_buff_switch(1);
        break;
    case UART_NO:
    default:
        func = &uart_ignore_char;
        // There is no disable option for ets_putc,
        // we switch to UART0 for disable case.
        uart_buff_switch(0);
        break;
    }
    if(gdbstub_has_putc1_control()) {
        gdbstub_set_putc1_callback(func);
    } else {
        if (uart_nr == UART0 || uart_nr == UART1) {
            system_set_os_print(1);
        } else {
            system_set_os_print(0);
        }
        ets_install_putc1(func);
    }
}

int
uart_get_debug()
{
    return s_uart_debug_nr;
}

/*
To start detection of baud rate with the UART the UART_AUTOBAUD_EN bit needs to be cleared and set. The ROM function uart_baudrate_detect() does this only once, so on a next call the UartDev.rcv_state is not equal to BAUD_RATE_DET. Instead of poking around in the UartDev struct with unknown effect, the UART_AUTOBAUD_EN bit is directly triggered by the function uart_detect_baudrate().
*/
void
uart_start_detect_baudrate(int uart_nr)
{
    USA(uart_nr) &= ~(UART_GLITCH_FILT << UART_GLITCH_FILT_S | UART_AUTOBAUD_EN);
    USA(uart_nr) = 0x08 << UART_GLITCH_FILT_S | UART_AUTOBAUD_EN;
}

int
uart_detect_baudrate(int uart_nr)
{
    static bool doTrigger = true;

    if (doTrigger)
    {
        uart_start_detect_baudrate(uart_nr);
        doTrigger = false;
    }

    int32_t divisor = uart_baudrate_detect(uart_nr, 1);
    if (!divisor) {
        return 0;
    }

    doTrigger = true;    // Initialize for a next round
    int32_t baudrate = UART_CLK_FREQ / divisor;

    static const int default_rates[] = {300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880, 115200, 230400, 256000, 460800, 921600, 1843200, 3686400};

    size_t i;
    for (i = 1; i < sizeof(default_rates) / sizeof(default_rates[0]) - 1; i++)	// find the nearest real baudrate
    {
        if (baudrate <= default_rates[i])
        {
            if (baudrate - default_rates[i - 1] < default_rates[i] - baudrate) {
                i--;
            }
            break;
        }
    }

    return default_rates[i];
}

};
