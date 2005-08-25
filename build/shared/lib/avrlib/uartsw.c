/*! \file uartsw.c \brief Software Interrupt-driven UART function library. */
//*****************************************************************************
//
// File Name	: 'uartsw.c'
// Title		: Software Interrupt-driven UART function library
// Author		: Pascal Stang - Copyright (C) 2002-2004
// Created		: 7/20/2002
// Revised		: 4/27/2004
// Version		: 0.1
// Target MCU	: Atmel AVR Series (intended for the ATmega16 and ATmega32)
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#include "global.h"
#include "timer.h"
#include "uartsw.h"

// Program ROM constants

// Global variables

// uartsw transmit status and data variables
static volatile u08 UartswTxBusy;
static volatile u08 UartswTxData;
static volatile u08 UartswTxBitNum;

// baud rate common to transmit and receive
static volatile u16 UartswBaudRateDiv;

// uartsw receive status and data variables
static volatile u08 UartswRxBusy;
static volatile u08 UartswRxData;
static volatile u08 UartswRxBitNum;
// receive buffer
static cBuffer uartswRxBuffer;               ///< uartsw receive buffer
// automatically allocate space in ram for each buffer
static char uartswRxData[UARTSW_RX_BUFFER_SIZE];

// functions

//! enable and initialize the software uart
void uartswInit(void)
{
    // initialize the buffers
	uartswInitBuffers();
	// initialize the ports
	sbi(UARTSW_TX_DDR, UARTSW_TX_PIN);
	cbi(UARTSW_RX_DDR, UARTSW_RX_PIN);
	cbi(UARTSW_RX_PORT, UARTSW_RX_PIN);
	// initialize baud rate
	uartswSetBaudRate(9600);

	// setup the transmitter
	UartswTxBusy = FALSE;
	// disable OC1A interrupt
	cbi(TIMSK, OCIE1A);
	// attach TxBit service routine to OC1A
	timerAttach(TIMER1OUTCOMPAREA_INT, uartswTxBitService);

	// setup the receiver
	UartswRxBusy = FALSE;
	// disable OC1B interrupt
	cbi(TIMSK, OCIE1B);
	// attach RxBit service routine to OC1B
	timerAttach(TIMER1OUTCOMPAREB_INT, uartswRxBitService);
	// attach RxBit service routine to ICP
	timerAttach(TIMER1INPUTCAPTURE_INT, uartswRxBitService);
	// trigger on rising edge
	sbi(TCCR1B, ICES1);
	// enable ICP interrupt
	sbi(TIMSK, TICIE1);

	// turn on interrupts
	sei();
}

//! create and initialize the uart buffers
void uartswInitBuffers(void)
{
	// initialize the UART receive buffer
	bufferInit(&uartswRxBuffer, uartswRxData, UARTSW_RX_BUFFER_SIZE);
}

//! turns off software UART
void uartswOff(void)
{
	// disable interrupts
	cbi(TIMSK, OCIE1A);
	cbi(TIMSK, OCIE1B);
	cbi(TIMSK, TICIE1);
	// detach the service routines
	timerDetach(TIMER1OUTCOMPAREA_INT);
	timerDetach(TIMER1OUTCOMPAREB_INT);
	timerDetach(TIMER1INPUTCAPTURE_INT);
}

void uartswSetBaudRate(u32 baudrate)
{
	// set timer prescaler
	timer1SetPrescaler(TIMER_CLK_DIV1);
	// calculate division factor for requested baud rate, and set it
	UartswBaudRateDiv = (u16)((F_CPU+(baudrate/2L))/(baudrate*1L));
}

//! returns the receive buffer structure 
cBuffer* uartswGetRxBuffer(void)
{
	// return rx buffer pointer
	return &uartswRxBuffer;
}

void uartswSendByte(u08 data)
{
	// wait until uart is ready
	while(UartswTxBusy);
	// set busy flag
	UartswTxBusy = TRUE;
	// save data
	UartswTxData = data;
	// set number of bits (+1 for stop bit)
	UartswTxBitNum = 9;
	
	// set the start bit
	#ifdef UARTSW_INVERT
	sbi(UARTSW_TX_PORT, UARTSW_TX_PIN);
	#else
	cbi(UARTSW_TX_PORT, UARTSW_TX_PIN);
	#endif

	// schedule the next bit
	outw(OCR1A, inw(TCNT1) + UartswBaudRateDiv);
	// enable OC1A interrupt
	sbi(TIMSK, OCIE1A);
}

//! gets a byte (if available) from the uart receive buffer
u08 uartswReceiveByte(u08* rxData)
{
	// make sure we have a receive buffer
	if(uartswRxBuffer.size)
	{
		// make sure we have data
		if(uartswRxBuffer.datalength)
		{
			// get byte from beginning of buffer
			*rxData = bufferGetFromFront(&uartswRxBuffer);
			return TRUE;
		}
		else
		{
			// no data
			return FALSE;
		}
	}
	else
	{
		// no buffer
		return FALSE;
	}
}

void uartswTxBitService(void)
{
	if(UartswTxBitNum)
	{
		// there are bits still waiting to be transmitted
		if(UartswTxBitNum > 1)
		{
			// transmit data bits (inverted, LSB first)
			#ifdef UARTSW_INVERT
			if( !(UartswTxData & 0x01) )
			#else
			if( (UartswTxData & 0x01) )
			#endif
				sbi(UARTSW_TX_PORT, UARTSW_TX_PIN);
			else
				cbi(UARTSW_TX_PORT, UARTSW_TX_PIN);
			// shift bits down
			UartswTxData = UartswTxData>>1;
		}
		else
		{
			// transmit stop bit
			#ifdef UARTSW_INVERT
			cbi(UARTSW_TX_PORT, UARTSW_TX_PIN);
			#else
			sbi(UARTSW_TX_PORT, UARTSW_TX_PIN);
			#endif
		}
		// schedule the next bit
		outw(OCR1A, inw(OCR1A) + UartswBaudRateDiv);
		// count down
		UartswTxBitNum--;
	}
	else
	{
		// transmission is done
		// clear busy flag
		UartswTxBusy = FALSE;
	}
}

void uartswRxBitService(void)
{
	// this function runs on either:
	// - a rising edge interrupt
	// - OC1B
	if(!UartswRxBusy)
	{
		// this is a start bit
		// disable ICP interrupt
		cbi(TIMSK, TICIE1);
		// schedule data bit sampling 1.5 bit periods from now
		outw(OCR1B, inw(TCNT1) + UartswBaudRateDiv + UartswBaudRateDiv/2);
		// clear OC1B interrupt flag
		sbi(TIFR, OCF1B);
		// enable OC1B interrupt
		sbi(TIMSK, OCIE1B);
		// set start bit flag
		UartswRxBusy = TRUE;
		// reset bit counter
		UartswRxBitNum = 0;
		// reset data
		UartswRxData = 0;
	}
	else
	{
		// start bit has already been received
		// we're in the data bits
		
		// shift data byte to make room for new bit
		UartswRxData = UartswRxData>>1;

		// sample the data line
		#ifdef UARTSW_INVERT
		if( !(inb(UARTSW_RX_PORTIN) & (1<<UARTSW_RX_PIN)) )
		#else
		if( !(inb(UARTSW_RX_PORTIN) & (1<<UARTSW_RX_PIN)) )
		#endif
		{
			// serial line is marking
			// record '1' bit
			UartswRxData |= 0x80;
		}

		// increment bit counter
		UartswRxBitNum++;
		// schedule next bit sample
		outw(OCR1B, inw(OCR1B) + UartswBaudRateDiv);

		// check if we have a full byte
		if(UartswRxBitNum >= 8)
		{
			// save data in receive buffer
			bufferAddToEnd(&uartswRxBuffer, UartswRxData);
			// disable OC1B interrupt
			cbi(TIMSK, OCIE1B);
			// clear ICP interrupt flag
			sbi(TIFR, ICF1);
			// enable ICP interrupt
			sbi(TIMSK, TICIE1);
			// clear start bit flag
			UartswRxBusy = FALSE;
		}
	}
}

/*
void uartswRxBitService(void)
{
	u16 thisBitTime;
	u08 bitperiods;
	u08 i;

	// bit transition was detected
	// record bit's edge time
	thisBitTime = inw(ICR1);

	cbi(PORTB, 0);

	if(!UartswRxStartBit)
	{
		// this is a start bit
		// switch to falling-edge trigger
		cbi(TCCR1B, ICES1);
		// record bit time
		UartswRxBitTime = thisBitTime;
		// set start bit flag
		UartswRxStartBit = TRUE;
		// reset bit counter
		UartswRxBitNum = 0;
		// reset data
		UartswRxData = 0;
	}
	else
	{
		// start bit has already been received
		// we're in the data bits
		
		// how many bit periods since last edge?
		bitperiods = (thisBitTime - UartswRxBitTime + UartswBaudRateDiv/2)/UartswBaudRateDiv;
		// set last edge time
		UartswRxBitTime = thisBitTime;

		if(bitperiods > 10)
		{
			// switch to trigger on rising edge
			sbi(TCCR1B, ICES1);
			// clear start bit flag
			UartswRxStartBit = FALSE;
		}
		else
		{


		if( inb(TCCR1B) & (1<<ICES1) )
		{
			// just triggered on a rising edge
			// previous bits were zero
			// shift in the data (data bits are inverted)
			for(i=0; i<bitperiods; i++)
			{
				UartswRxData = UartswRxData<<1;
				UartswRxData |= 0x01;
			}
			// switch to trigger on falling edge
			cbi(TCCR1B, ICES1);
		}
		else
		{
			// just triggered on a falling edge
			// previous bits were one
			// shift in the data (data bits are inverted)
			for(i=0; i<bitperiods; i++)
			{
				UartswRxData = UartswRxData<<1;
			}
			// switch to trigger on rising edge
			sbi(TCCR1B, ICES1);
		}
		
		// increment bit counter
		UartswRxBitNum += bitperiods;
		
		// check if we have a full byte + start bit
		if(bitperiods > 8)
		{
			// save data in receive buffer
			bufferAddToEnd(&uartswRxBuffer, UartswRxData);
			// switch to trigger on rising edge
			sbi(TCCR1B, ICES1);
			// clear start bit flag
			UartswRxStartBit = FALSE;
		}
		}
	}

	// turn off debug LEDs
	delay(10);
	sbi(PORTB, 0);
	sbi(PORTB, 1);
}
*/
