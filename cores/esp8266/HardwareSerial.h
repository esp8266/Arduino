/*
  HardwareSerial.h - Hardware serial library for Wiring
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

  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"

#if !(defined(SERIAL_TX_BUFFER_SIZE) && defined(SERIAL_RX_BUFFER_SIZE))
#define SERIAL_TX_BUFFER_SIZE 64
#define SERIAL_RX_BUFFER_SIZE 64
#endif

typedef uint32_t tx_buffer_index_t;
typedef uint32_t rx_buffer_index_t;

// // Define config for Serial.begin(baud, config);
// #define SERIAL_5N1 0x00
// #define SERIAL_6N1 0x02
// #define SERIAL_7N1 0x04
// #define SERIAL_8N1 0x06
// #define SERIAL_5N2 0x08
// #define SERIAL_6N2 0x0A
// #define SERIAL_7N2 0x0C
// #define SERIAL_8N2 0x0E
// #define SERIAL_5E1 0x20
// #define SERIAL_6E1 0x22
// #define SERIAL_7E1 0x24
// #define SERIAL_8E1 0x26
// #define SERIAL_5E2 0x28
// #define SERIAL_6E2 0x2A
// #define SERIAL_7E2 0x2C
// #define SERIAL_8E2 0x2E
// #define SERIAL_5O1 0x30
// #define SERIAL_6O1 0x32
// #define SERIAL_7O1 0x34
// #define SERIAL_8O1 0x36
// #define SERIAL_5O2 0x38
// #define SERIAL_6O2 0x3A
// #define SERIAL_7O2 0x3C
// #define SERIAL_8O2 0x3E

struct uart_;
typedef uart_ uart_t;
class HardwareSerial : public Stream
{
  protected:
    uart_t* _uart;
    // Has any byte been written to the UART since begin()
    bool _written;

    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    unsigned char _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];

  public:
    HardwareSerial() : _uart(0) { }

    void begin(unsigned long baud) { begin(baud, 0); }
    void begin(unsigned long, uint8_t);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    int availableForWrite(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() { return true; }

    // Interrupt handlers - Not intended to be called externally
    void _rx_complete_irq(char c);
    void _tx_udr_empty_irq(void);
};

extern HardwareSerial Serial;

#endif
