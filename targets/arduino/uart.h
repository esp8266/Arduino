/*! \file uart.h \brief UART driver with buffer support. */
//*****************************************************************************
//
// File Name	: 'uart.h'
// Title		: UART driver with buffer support
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 02/01/2004
// Version		: 1.3
// Target MCU	: ATMEL AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef UART_H
#define UART_H

#include "global.h"
#include "buffer.h"

//! default baud rate
//! can be changed by using uartSetBaudRate()
#define UART_DEFAULT_BAUD_RATE	9600

// buffer memory allocation defines
// buffer sizes
#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE		0x0040	///< number of bytes for uart transmit buffer
#endif
#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE		0x0040	///< number of bytes for uart receive buffer
#endif

// define this key if you wish to use
// external RAM for the	UART buffers
//#define UART_BUFFER_EXTERNAL_RAM
#ifdef UART_BUFFER_EXTERNAL_RAM
	// absolute address of uart buffers
	#define UART_TX_BUFFER_ADDR	0x1000
	#define UART_RX_BUFFER_ADDR	0x1100
#endif

// type of interrupt handler to use
// *do not change unless you know what you're doing
// Value may be SIGNAL or INTERRUPT
#ifndef UART_INTERRUPT_HANDLER
#define UART_INTERRUPT_HANDLER	SIGNAL
#endif

// compatibility with most newer processors
#ifdef UCSRB
	#define UCR					UCSRB
#endif
// compatibility with old Mega processors
#if defined(UBRR) && !defined(UBRRL)
	#define	UBRRL				UBRR
#endif
// DAM: it seems the ATmega168 includes a '0' in its register names, despite
// having only one uart
#if defined(__AVR_ATmega168__)
	#define RXCIE                                   RXCIE0
	#define TXCIE                                   TXCIE0
	#define RXEN                                    RXEN0
	#define TXEN                                    TXEN0
	#define UDR                                     UDR0
	#define UCR                                     UCSR0B
	#define UBRRL                           UBRR0L
	#define UBRRH                           UBRR0H
	#define SIG_UART_TRANS          USART_TX_vect
	#define SIG_UART_RECV           USART_RX_vect
#endif
// compatibility with dual-uart processors
// (if you need to use both uarts, please use the uart2 library)
#if defined(__AVR_ATmega128__)
	#define UDR					UDR0
	#define UCR					UCSR0B
	#define UBRRL				UBRR0L
	#define UBRRH				UBRR0H
	#define SIG_UART_TRANS		SIG_UART0_TRANS
	#define SIG_UART_RECV		SIG_UART0_RECV
	#define SIG_UART_DATA		SIG_UART0_DATA
#endif
#if defined(__AVR_ATmega161__)
	#define UDR					UDR0
	#define UCR					UCSR0B
	#define UBRRL				UBRR0
	#define SIG_UART_TRANS		SIG_UART0_TRANS
	#define SIG_UART_RECV		SIG_UART0_RECV
	#define SIG_UART_DATA		SIG_UART0_DATA
#endif

// functions

//! initializes transmit and receive buffers
// called from uartInit()
void uartInitBuffers(void);

//! initializes uart
void uartInit(void);

//! redirects received data to a user function
void uartSetRxHandler(void (*rx_func)(unsigned char c));

//! sets the uart baud rate
void uartSetBaudRate(u32 baudrate);

//! returns pointer to the receive buffer structure
cBuffer* uartGetRxBuffer(void);

//! returns pointer to the transmit buffer structure
cBuffer* uartGetTxBuffer(void);

//! sends a single byte over the uart
void uartSendByte(u08 data);

//! gets a single byte from the uart receive buffer (getchar-style)
// returns the byte, or -1 if no byte is available
int uartGetByte(void);

//! gets a single byte from the uart receive buffer
// Function returns TRUE if data was available, FALSE if not.
// Actual data is returned in variable pointed to by "data".
// example usage:
// char myReceivedByte;
// uartReceiveByte( &myReceivedByte );
u08 uartReceiveByte(u08* data);

//! returns TRUE/FALSE if receive buffer is empty/not-empty
u08 uartReceiveBufferIsEmpty(void);

//! flushes (deletes) all data from receive buffer
void uartFlushReceiveBuffer(void);

//! add byte to end of uart Tx buffer
void uartAddToTxBuffer(u08 data);

//! begins transmission of the transmit buffer under interrupt control
void uartSendTxBuffer(void);

//! sends a buffer of length nBytes via the uart using interrupt control
u08  uartSendBuffer(char *buffer, u16 nBytes);

#endif


