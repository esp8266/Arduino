/*! \file uart2.h \brief Dual UART driver with buffer support. */
//*****************************************************************************
//
// File Name	: 'uart2.h'
// Title		: Dual UART driver with buffer support
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/20/2000
// Revised		: 07/04/2004
// Version		: 1.0
// Target MCU	: ATMEL AVR Series
// Editor Tabs	: 4
//
// Description	: This is a UART driver for AVR-series processors with two
//		hardware UARTs such as the mega161 and mega128 
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef UART2_H
#define UART2_H

#include "global.h"
#include "buffer.h"

// default baud rate
// can be changed by using uartSetBaudRate()
#define UART0_DEFAULT_BAUD_RATE		9600	///< default baud rate for UART0
#define UART1_DEFAULT_BAUD_RATE		9600	///< default baud rate for UART1

// buffer memory allocation defines
// buffer sizes
#ifndef UART0_TX_BUFFER_SIZE
#define UART0_TX_BUFFER_SIZE		0x0010	///< number of bytes for uart0 transmit buffer
#endif
#ifndef UART0_RX_BUFFER_SIZE
#define UART0_RX_BUFFER_SIZE		0x0080	///< number of bytes for uart0 receive buffer
#endif
#ifndef UART1_TX_BUFFER_SIZE
#define UART1_TX_BUFFER_SIZE		0x0010	///< number of bytes for uart1 transmit buffer
#endif
#ifndef UART1_RX_BUFFER_SIZE
#define UART1_RX_BUFFER_SIZE		0x0080	///< number of bytes for uart1 receive buffer
#endif

// define this key if you wish to use
// external RAM for the	UART buffers
//#define UART_BUFFER_EXTERNAL_RAM
#ifdef UART_BUFFER_EXTERNAL_RAM
	// absolute address of uart0 buffers
	#define UART0_TX_BUFFER_ADDR	0x1000
	#define UART0_RX_BUFFER_ADDR	0x1100
	// absolute address of uart1 buffers
	#define UART1_TX_BUFFER_ADDR	0x1200
	#define UART1_RX_BUFFER_ADDR	0x1300
#endif

// type of interrupt handler to use
// *do not change unless you know what you're doing
// Value may be SIGNAL or INTERRUPT
#ifndef UART_INTERRUPT_HANDLER
#define UART_INTERRUPT_HANDLER	SIGNAL
#endif

// compatibility for the mega161
#ifndef RXCIE
	#define RXCIE	RXCIE0
	#define TXCIE	TXCIE0
	#define UDRIE	UDRIE0
	#define RXEN	RXEN0
	#define TXEN	TXEN0
	#define CHR9	CHR90
	#define RXB8	RXB80
	#define TXB8	TXB80
#endif
#ifndef UBRR0L
	#define UBRR0L	UBRR0
	#define UBRR1L	UBRR1
#endif

// functions

//! initializes both uarts
void uartInit(void);

//! initializes UART0 only
void uart0Init(void);

//! initializes UART1 only
void uart1Init(void);

//! initializes transmit and receive buffers
// called from uartInit()
void uart0InitBuffers(void);
void uart1InitBuffers(void);

//! redirects received data to a user function
void uartSetRxHandler(u08 nUart, void (*rx_func)(unsigned char c));

//! sets the uart baud rate
void uartSetBaudRate(u08 nUart, u32 baudrate);

//! returns pointer to the receive buffer structure
cBuffer* uartGetRxBuffer(u08 nUart);

//! returns pointer to the transmit buffer structure
cBuffer* uartGetTxBuffer(u08 nUart);

//! sends a single byte over the uart
void uartSendByte(u08 nUart, u08 data);
//! SendByte commands with the UART number hardcoded
// use this with printfInit() - example: printfInit(uart0SendByte);
void uart0SendByte(u08 data);
void uart1SendByte(u08 data);

//! gets a single byte from the uart receive buffer (getchar-style)
// returns the byte, or -1 if no byte is available
int uart0GetByte(void);
int uart1GetByte(void);

//! gets a single byte from the uart receive buffer
// function returns TRUE if data was available, FALSE if not
// actual data is returned in variable pointed to by "data"
// example usage:
// char myReceivedByte;
// uartReceiveByte( &myReceivedByte );
u08 uartReceiveByte(u08 nUart, u08* data);

//! returns TRUE/FALSE if receive buffer is empty/not-empty
u08 uartReceiveBufferIsEmpty(u08 nUart);

//! flushes (deletes) all data from receive buffer
void uartFlushReceiveBuffer(u08 nUart);

//! add byte to end of uart Tx buffer
void uartAddToTxBuffer(u08 nUart, u08 data);
//! AddToTxBuffer commands with the UART number hardcoded
// use this with printfInit() - example: printfInit(uart0AddToTxBuffer);
void uart0AddToTxBuffer(u08 data);
void uart1AddToTxBuffer(u08 data);

//! begins transmission of the transmit buffer under interrupt control
void uartSendTxBuffer(u08 nUart);

//! sends a buffer of length nBytes via the uart using interrupt control
u08 uartSendBuffer(u08 nUart, char *buffer, u16 nBytes);

//! interrupt service handlers
void uartTransmitService(u08 nUart);
void uartReceiveService(u08 nUart);

#endif

