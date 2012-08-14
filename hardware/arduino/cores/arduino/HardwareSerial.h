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
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"

struct ring_buffer;

class HardwareSerial : public Stream
{
  private:
    ring_buffer *_rx_buffer;
    ring_buffer *_tx_buffer;
    volatile uint8_t *_ubrrh;
    volatile uint8_t *_ubrrl;
    volatile uint8_t *_ucsra;
    volatile uint8_t *_ucsrb;
    volatile uint8_t *_ucsrc;
    volatile uint8_t *_udr;
    uint8_t _rxen;
    uint8_t _txen;
    uint8_t _rxcie;
    uint8_t _udrie;
    uint8_t _u2x;
  public:
    HardwareSerial(ring_buffer *rx_buffer, ring_buffer *tx_buffer,
      volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
      volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
      volatile uint8_t *ucsrc, volatile uint8_t *udr,
      uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udrie, uint8_t u2x);
    void begin(unsigned long);
    void begin(unsigned long, byte);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool();
};

// Define config for Serial.begin(baud, config);
#define _5n1_ 0x80
#define _5N1_ 0x80
#define _6n1_ 0x82
#define _6N1_ 0x82
#define _7n1_ 0x84
#define _7N1_ 0x84
#define _8n1_ 0x86
#define _8N1_ 0x86
#define _5n2_ 0x88
#define _5N2_ 0x88
#define _6n2_ 0x8A
#define _6N2_ 0x8A
#define _7n2_ 0x8C
#define _7N2_ 0x8C
#define _8n2_ 0x8E
#define _8N2_ 0x8E
#define _5e1_ 0xA0
#define _5E1_ 0xA0
#define _6e1_ 0xA2
#define _6E1_ 0xA2
#define _7e1_ 0xA4
#define _7E1_ 0xA4
#define _8e1_ 0xA6
#define _8E1_ 0xA6
#define _5e2_ 0xA8
#define _5E2_ 0xA8
#define _6e2_ 0xAA
#define _6E2_ 0xAA
#define _7e2_ 0xAC
#define _7E2_ 0xAC
#define _8e2_ 0xAE
#define _8E2_ 0xAE
#define _5o1_ 0xB0
#define _5O1_ 0xB0
#define _6o1_ 0xB2
#define _6O1_ 0xB2
#define _7o1_ 0xB4
#define _7O1_ 0xB4
#define _8o1_ 0xB6
#define _8O1_ 0xB6
#define _5o2_ 0xB8
#define _5O2_ 0xB8
#define _6o2_ 0xBA
#define _6O2_ 0xBA
#define _7o2_ 0xBC
#define _7O2_ 0xBC
#define _8o2_ 0xBE
#define _8O2_ 0xBE

#if defined(UBRRH) || defined(UBRR0H)
  extern HardwareSerial Serial;
#elif defined(USBCON)
  #include "USBAPI.h"
//  extern HardwareSerial Serial_;  
#endif
#if defined(UBRR1H)
  extern HardwareSerial Serial1;
#endif
#if defined(UBRR2H)
  extern HardwareSerial Serial2;
#endif
#if defined(UBRR3H)
  extern HardwareSerial Serial3;
#endif

extern void serialEventRun(void) __attribute__((weak));

#endif
