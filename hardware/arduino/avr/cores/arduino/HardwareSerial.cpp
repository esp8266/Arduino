/*
  HardwareSerial.cpp - Hardware serial library for Wiring
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
  
  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "wiring_private.h"

// this next line disables the entire HardwareSerial.cpp, 
// this is so I can support Attiny series and any other chip without a uart
#if defined(UBRRH) || defined(UBRR0H) || defined(UBRR1H) || defined(UBRR2H) || defined(UBRR3H)

#include "HardwareSerial.h"

/*
 * on ATmega8, the uart and its bits are not numbered, so there is no "TXC0"
 * definition.
 */
#if !defined(TXC0)
#if defined(TXC)
#define TXC0 TXC
#elif defined(TXC1)
// Some devices have uart1 but no uart0
#define TXC0 TXC1
#else
#error TXC0 not definable in HardwareSerial.h
#endif
#endif

inline void store_char(unsigned char c, HardwareSerial *s)
{
  int i = (unsigned int)(s->_rx_buffer_head + 1) % SERIAL_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != s->_rx_buffer_tail) {
    s->_rx_buffer[s->_rx_buffer_head] = c;
    s->_rx_buffer_head = i;
  }
}

#if !defined(USART0_RX_vect) && defined(USART1_RX_vect)
// do nothing - on the 32u4 the first USART is USART1
#else
#if !defined(USART_RX_vect) && !defined(USART0_RX_vect) && \
    !defined(USART_RXC_vect)
  #error "Don't know what the Data Received vector is called for the first UART"
#else
  void serialEvent() __attribute__((weak));
  void serialEvent() {}
  #define serialEvent_implemented
#if defined(USART_RX_vect)
  ISR(USART_RX_vect)
#elif defined(USART0_RX_vect)
  ISR(USART0_RX_vect)
#elif defined(USART_RXC_vect)
  ISR(USART_RXC_vect) // ATmega8
#endif
  {
  #if defined(UDR0)
    if (bit_is_clear(UCSR0A, UPE0)) {
      unsigned char c = UDR0;
      store_char(c, &Serial);
    } else {
      unsigned char c = UDR0;
    };
  #elif defined(UDR)
    if (bit_is_clear(UCSRA, PE)) {
      unsigned char c = UDR;
      store_char(c, &Serial);
    } else {
      unsigned char c = UDR;
    };
  #else
    #error UDR not defined
  #endif
  }
#endif
#endif

#if defined(USART1_RX_vect)
  void serialEvent1() __attribute__((weak));
  void serialEvent1() {}
  #define serialEvent1_implemented
  ISR(USART1_RX_vect)
  {
    if (bit_is_clear(UCSR1A, UPE1)) {
      unsigned char c = UDR1;
      store_char(c, &Serial1);
    } else {
      unsigned char c = UDR1;
    };
  }
#endif

#if defined(USART2_RX_vect) && defined(UDR2)
  void serialEvent2() __attribute__((weak));
  void serialEvent2() {}
  #define serialEvent2_implemented
  ISR(USART2_RX_vect)
  {
    if (bit_is_clear(UCSR2A, UPE2)) {
      unsigned char c = UDR2;
      store_char(c, &Serial2);
    } else {
      unsigned char c = UDR2;
    };
  }
#endif

#if defined(USART3_RX_vect) && defined(UDR3)
  void serialEvent3() __attribute__((weak));
  void serialEvent3() {}
  #define serialEvent3_implemented
  ISR(USART3_RX_vect)
  {
    if (bit_is_clear(UCSR3A, UPE3)) {
      unsigned char c = UDR3;
      store_char(c, &Serial3);
    } else {
      unsigned char c = UDR3;
    };
  }
#endif

void serialEventRun(void)
{
#ifdef serialEvent_implemented
  if (Serial.available()) serialEvent();
#endif
#ifdef serialEvent1_implemented
  if (Serial1.available()) serialEvent1();
#endif
#ifdef serialEvent2_implemented
  if (Serial2.available()) serialEvent2();
#endif
#ifdef serialEvent3_implemented
  if (Serial3.available()) serialEvent3();
#endif
}


#if !defined(USART0_UDRE_vect) && defined(USART1_UDRE_vect)
// do nothing - on the 32u4 the first USART is USART1
#else
#if !defined(UART0_UDRE_vect) && !defined(UART_UDRE_vect) && !defined(USART0_UDRE_vect) && !defined(USART_UDRE_vect)
  #error "Don't know what the Data Register Empty vector is called for the first UART"
#else
#if defined(UART0_UDRE_vect)
ISR(UART0_UDRE_vect)
#elif defined(UART_UDRE_vect)
ISR(UART_UDRE_vect)
#elif defined(USART0_UDRE_vect)
ISR(USART0_UDRE_vect)
#elif defined(USART_UDRE_vect)
ISR(USART_UDRE_vect)
#endif
{
  if (Serial._tx_buffer_head == Serial._tx_buffer_tail) {
	// Buffer empty, so disable interrupts
#if defined(UCSR0B)
    cbi(UCSR0B, UDRIE0);
#else
    cbi(UCSRB, UDRIE);
#endif
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = Serial._tx_buffer[Serial._tx_buffer_tail];
    Serial._tx_buffer_tail = (Serial._tx_buffer_tail + 1) % SERIAL_BUFFER_SIZE;
	
  #if defined(UDR0)
    UDR0 = c;
  #elif defined(UDR)
    UDR = c;
  #else
    #error UDR not defined
  #endif
  }
}
#endif
#endif

#ifdef USART1_UDRE_vect
ISR(USART1_UDRE_vect)
{
  if (Serial1._tx_buffer_head == Serial1._tx_buffer_tail) {
	// Buffer empty, so disable interrupts
    cbi(UCSR1B, UDRIE1);
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = Serial1._tx_buffer[Serial1._tx_buffer_tail];
    Serial1._tx_buffer_tail = (Serial1._tx_buffer_tail + 1) % SERIAL_BUFFER_SIZE;
	
    UDR1 = c;
  }
}
#endif

#ifdef USART2_UDRE_vect
ISR(USART2_UDRE_vect)
{
  if (Serial2._tx_buffer_head == Serial2._tx_buffer_tail) {
	// Buffer empty, so disable interrupts
    cbi(UCSR2B, UDRIE2);
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = Serial2._tx_buffer[Serial2._tx_buffer_tail];
    Serial2._tx_buffer_tail = (Serial2._tx_buffer_tail + 1) % SERIAL_BUFFER_SIZE;
	
    UDR2 = c;
  }
}
#endif

#ifdef USART3_UDRE_vect
ISR(USART3_UDRE_vect)
{
  if (Serial3._tx_buffer_head == Serial3._tx_buffer_tail) {
	// Buffer empty, so disable interrupts
    cbi(UCSR3B, UDRIE3);
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = Serial3._tx_buffer[Serial3._tx_buffer_tail];
    Serial3._tx_buffer_tail = (Serial3._tx_buffer_tail + 1) % SERIAL_BUFFER_SIZE;
	
    UDR3 = c;
  }
}
#endif

// Constructors ////////////////////////////////////////////////////////////////

HardwareSerial::HardwareSerial(
  volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
  volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
  volatile uint8_t *ucsrc, volatile uint8_t *udr,
  uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udrie, uint8_t u2x)
{
  _tx_buffer_head = _tx_buffer_tail = 0;
  _rx_buffer_head = _rx_buffer_tail = 0;
  _ubrrh = ubrrh;
  _ubrrl = ubrrl;
  _ucsra = ucsra;
  _ucsrb = ucsrb;
  _ucsrc = ucsrc;
  _udr = udr;
  _rxen = rxen;
  _txen = txen;
  _rxcie = rxcie;
  _udrie = udrie;
  _u2x = u2x;
}

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::begin(unsigned long baud)
{
  uint16_t baud_setting;
  bool use_u2x = true;

#if F_CPU == 16000000UL
  // hardcoded exception for compatibility with the bootloader shipped
  // with the Duemilanove and previous boards and the firmware on the 8U2
  // on the Uno and Mega 2560.
  if (baud == 57600) {
    use_u2x = false;
  }
#endif

try_again:
  
  if (use_u2x) {
    *_ucsra = 1 << _u2x;
    baud_setting = (F_CPU / 4 / baud - 1) / 2;
  } else {
    *_ucsra = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }
  
  if ((baud_setting > 4095) && use_u2x)
  {
    use_u2x = false;
    goto try_again;
  }

  // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
  *_ubrrh = baud_setting >> 8;
  *_ubrrl = baud_setting;

  transmitting = false;

  sbi(*_ucsrb, _rxen);
  sbi(*_ucsrb, _txen);
  sbi(*_ucsrb, _rxcie);
  cbi(*_ucsrb, _udrie);
}

void HardwareSerial::begin(unsigned long baud, byte config)
{
  uint16_t baud_setting;
  uint8_t current_config;
  bool use_u2x = true;

#if F_CPU == 16000000UL
  // hardcoded exception for compatibility with the bootloader shipped
  // with the Duemilanove and previous boards and the firmware on the 8U2
  // on the Uno and Mega 2560.
  if (baud == 57600) {
    use_u2x = false;
  }
#endif

try_again:
  
  if (use_u2x) {
    *_ucsra = 1 << _u2x;
    baud_setting = (F_CPU / 4 / baud - 1) / 2;
  } else {
    *_ucsra = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }
  
  if ((baud_setting > 4095) && use_u2x)
  {
    use_u2x = false;
    goto try_again;
  }

  // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
  *_ubrrh = baud_setting >> 8;
  *_ubrrl = baud_setting;

  //set the data bits, parity, and stop bits
#if defined(__AVR_ATmega8__)
  config |= 0x80; // select UCSRC register (shared with UBRRH)
#endif
  *_ucsrc = config;
  
  sbi(*_ucsrb, _rxen);
  sbi(*_ucsrb, _txen);
  sbi(*_ucsrb, _rxcie);
  cbi(*_ucsrb, _udrie);
}

void HardwareSerial::end()
{
  // wait for transmission of outgoing data
  while (_tx_buffer_head != _tx_buffer_tail)
    ;

  cbi(*_ucsrb, _rxen);
  cbi(*_ucsrb, _txen);
  cbi(*_ucsrb, _rxcie);  
  cbi(*_ucsrb, _udrie);
  
  // clear any received data
  _rx_buffer_head = _rx_buffer_tail;
}

int HardwareSerial::available(void)
{
  return (unsigned int)(SERIAL_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail) % SERIAL_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    return _rx_buffer[_rx_buffer_tail];
  }
}

int HardwareSerial::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    unsigned char c = _rx_buffer[_rx_buffer_tail];
    _rx_buffer_tail = (unsigned int)(_rx_buffer_tail + 1) % SERIAL_BUFFER_SIZE;
    return c;
  }
}

void HardwareSerial::flush()
{
  // UDR is kept full while the buffer is not empty, so TXC triggers when EMPTY && SENT
  while (transmitting && ! (*_ucsra & _BV(TXC0)));
  transmitting = false;
}

size_t HardwareSerial::write(uint8_t c)
{
  int i = (_tx_buffer_head + 1) % SERIAL_BUFFER_SIZE;
	
  // If the output buffer is full, there's nothing for it other than to 
  // wait for the interrupt handler to empty it a bit
  // ???: return 0 here instead?
  while (i == _tx_buffer_tail)
    ;
	
  _tx_buffer[_tx_buffer_head] = c;
  _tx_buffer_head = i;
	
  sbi(*_ucsrb, _udrie);
  // clear the TXC bit -- "can be cleared by writing a one to its bit location"
  transmitting = true;
  sbi(*_ucsra, TXC0);
  
  return 1;
}

HardwareSerial::operator bool() {
	return true;
}

// Preinstantiate Objects //////////////////////////////////////////////////////

#if defined(UBRRH) && defined(UBRRL)
  HardwareSerial Serial(&UBRRH, &UBRRL, &UCSRA, &UCSRB, &UCSRC, &UDR, RXEN, TXEN, RXCIE, UDRIE, U2X);
#elif defined(UBRR0H) && defined(UBRR0L)
  HardwareSerial Serial(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0, RXEN0, TXEN0, RXCIE0, UDRIE0, U2X0);
#elif defined(USBCON)
  // do nothing - Serial object and buffers are initialized in CDC code
#else
  #error no serial port defined  (port 0)
#endif

#if defined(UBRR1H)
  HardwareSerial Serial1(&UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1, RXEN1, TXEN1, RXCIE1, UDRIE1, U2X1);
#endif
#if defined(UBRR2H)
  HardwareSerial Serial2(&UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UCSR2C, &UDR2, RXEN2, TXEN2, RXCIE2, UDRIE2, U2X2);
#endif
#if defined(UBRR3H)
  HardwareSerial Serial3(&UBRR3H, &UBRR3L, &UCSR3A, &UCSR3B, &UCSR3C, &UDR3, RXEN3, TXEN3, RXCIE3, UDRIE3, U2X3);
#endif

#endif // whole file

