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
 Modified 18 December 2014 by Ivan Grokhotkov (esp8266 platform support)
 Modified 31 March 2015 by Markus Sattler (rewrite the code for UART0 + UART1 support in ESP8266)
 Modified 25 April 2015 by Thomas Flayols (add configuration different from 8N1 in ESP8266)
 */

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"

#define SERIAL_TX_BUFFER_SIZE 256
#define SERIAL_RX_BUFFER_SIZE 256

// Define config for Serial.begin(baud, config);
#define SERIAL_5N1 0x10
#define SERIAL_6N1 0x14
#define SERIAL_7N1 0x18
#define SERIAL_8N1 0x1c
#define SERIAL_5N2 0x30
#define SERIAL_6N2 0x34
#define SERIAL_7N2 0x38
#define SERIAL_8N2 0x3c
#define SERIAL_5E1 0x12
#define SERIAL_6E1 0x16
#define SERIAL_7E1 0x1a
#define SERIAL_8E1 0x1e
#define SERIAL_5E2 0x32
#define SERIAL_6E2 0x36
#define SERIAL_7E2 0x3a
#define SERIAL_8E2 0x3e
#define SERIAL_5O1 0x13
#define SERIAL_6O1 0x17
#define SERIAL_7O1 0x1b
#define SERIAL_8O1 0x1f
#define SERIAL_5O2 0x33
#define SERIAL_6O2 0x37
#define SERIAL_7O2 0x3b
#define SERIAL_8O2 0x3f

#define SERIAL_FULL     0
#define SERIAL_RX_ONLY  1
#define SERIAL_TX_ONLY  2

class cbuf;

struct uart_;
typedef struct uart_ uart_t;

class HardwareSerial: public Stream {
    public:
        HardwareSerial(int uart_nr);

        void begin(unsigned long baud) {
            begin(baud, SERIAL_8N1, SERIAL_FULL);
        }
        void begin(unsigned long baud, uint8_t config) {
            begin(baud, config, SERIAL_FULL);
        }
        void begin(unsigned long, uint8_t, uint8_t);
        void end();
        void swap();  //toggle between use of GPIO13/GPIO15 or GPIO3/GPIO1 as RX and TX
        int available(void) override;
        int peek(void) override;
        int read(void) override;
        int availableForWrite(void);
        void flush(void) override;
        size_t write(uint8_t) override;
        inline size_t write(unsigned long n) {
            return write((uint8_t) n);
        }
        inline size_t write(long n) {
            return write((uint8_t) n);
        }
        inline size_t write(unsigned int n) {
            return write((uint8_t) n);
        }
        inline size_t write(int n) {
            return write((uint8_t) n);
        }
        using Print::write; // pull in write(str) and write(buf, size) from Print
        operator bool() const;

        void setDebugOutput(bool);
        bool isTxEnabled(void);
        bool isRxEnabled(void);

    protected:
        friend void uart_interrupt_handler(uart_t* uart);
        void _rx_complete_irq(char c);
        void _tx_empty_irq(void);

    protected:
        int _uart_nr;
        uart_t* _uart;
        cbuf* _tx_buffer;
        cbuf* _rx_buffer;
};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;

#endif
