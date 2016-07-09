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
#include "uart.h"

enum SerialConfig {
    SERIAL_5N1 = UART_5N1,
    SERIAL_6N1 = UART_6N1,
    SERIAL_7N1 = UART_7N1,
    SERIAL_8N1 = UART_8N1,
    SERIAL_5N2 = UART_5N2,
    SERIAL_6N2 = UART_6N2,
    SERIAL_7N2 = UART_7N2,
    SERIAL_8N2 = UART_8N2,
    SERIAL_5E1 = UART_5E1,
    SERIAL_6E1 = UART_6E1,
    SERIAL_7E1 = UART_7E1,
    SERIAL_8E1 = UART_8E1,
    SERIAL_5E2 = UART_5E2,
    SERIAL_6E2 = UART_6E2,
    SERIAL_7E2 = UART_7E2,
    SERIAL_8E2 = UART_8E2,
    SERIAL_5O1 = UART_5O1,
    SERIAL_6O1 = UART_6O1,
    SERIAL_7O1 = UART_7O1,
    SERIAL_8O1 = UART_8O1,
    SERIAL_5O2 = UART_5O2,
    SERIAL_6O2 = UART_6O2,
    SERIAL_7O2 = UART_7O2,
    SERIAL_8O2 = UART_8O2,
};

enum SerialMode {
    SERIAL_FULL = UART_FULL,
    SERIAL_RX_ONLY = UART_RX_ONLY,
    SERIAL_TX_ONLY = UART_TX_ONLY
};

class HardwareSerial: public Stream
{
public:
    HardwareSerial(int uart_nr);
    virtual ~HardwareSerial() {}

    void begin(unsigned long baud)
    {
        begin(baud, SERIAL_8N1, SERIAL_FULL, 1);
    }
    void begin(unsigned long baud, SerialConfig config)
    {
        begin(baud, config, SERIAL_FULL, 1);
    }
    void begin(unsigned long baud, SerialConfig config, SerialMode mode)
    {
        begin(baud, config, mode, 1);
    }

    void begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin);

    void end();

    size_t setRxBufferSize(size_t size);

    void swap()
    {
        swap(1);
    }
    void swap(uint8_t tx_pin);    //toggle between use of GPIO13/GPIO15 or GPIO3/GPIO(1/2) as RX and TX

    /*
     * Toggle between use of GPIO1 and GPIO2 as TX on UART 0.
     * Note: UART 1 can't be used if GPIO2 is used with UART 0!
     */
    void set_tx(uint8_t tx_pin);

    /*
     * UART 0 possible options are (1, 3), (2, 3) or (15, 13)
     * UART 1 allows only TX on 2 if UART 0 is not (2, 3)
     */
    void pins(uint8_t tx, uint8_t rx);

    int available(void) override;
    int peek(void) override;
    int read(void) override;
    int availableForWrite(void);
    void flush(void) override;
    size_t write(uint8_t) override;
    inline size_t write(unsigned long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t) n);
    }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() const;

    void setDebugOutput(bool);
    bool isTxEnabled(void);
    bool isRxEnabled(void);
    int  baudRate(void);

protected:
    int _uart_nr;
    uart_t* _uart = nullptr;
    size_t _rx_size;
};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;

#endif
