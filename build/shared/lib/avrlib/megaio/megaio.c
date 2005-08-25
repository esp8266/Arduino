/*! \file megaio.c \brief MegaIO Control/Access function library. */
//*****************************************************************************
//
// File Name	: 'megaio.c'
// Title		: MegaIO Control/Access function library
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 5/18/2004
// Revised		: 5/18/2004
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "buffer.h"		// include buffer support
#include "i2c.h"		// include I2C functions
#include "megaio/megaioreg.h"	// include MegaIO register definitions

#include "megaio.h"

// MegaIO local receive buffer size
#define MEGAIO_UART_RX_BUFFER_SIZE		0x80
// MegaIO local receive buffer data array
static char megaioUartRxData[MEGAIO_UART_RX_BUFFER_SIZE];
// MegaIO local receive buffer
cBuffer megaioUartRxBuffer;

//! initialize the MegaIO interface
u08 megaioInit(void)
{
	// initialize the UART receive buffer
	bufferInit(&megaioUartRxBuffer, megaioUartRxData, MEGAIO_UART_RX_BUFFER_SIZE);
	// initialize i2c interface
	i2cInit();
	i2cSetBitrate(30);
	// check for presence of megaio chip
	if( megaioReadReg(MEGAIOREG_IDSTRING, 1) == 'M' )
	{
		// megaio responded correctly
		// initialization succeeded
		return TRUE;
	}
	else
	{
		// megaio responded incorrectly
		// initialization failed
		return FALSE;
	}
}

//! write an 8-32 bit number to a MegaIO register
void megaioWriteReg(unsigned char regnum, unsigned char nbytes, unsigned long data)
{
	u08 packet[5];

	// construct I2c data packet
	//   first byte is register address
	//   following bytes are the data that will be written to that register
	packet[0] = regnum;
	packet[1] = data;
	packet[2] = data>>8;
	packet[3] = data>>16;
	packet[4] = data>>24;
	// send 2 bytes (register and data) to MegaIO
	i2cMasterSend(MEGAIO_I2C_ADDR, 1+nbytes, packet);
}

//! read an 8-32 bit number from a MegaIO register
unsigned long megaioReadReg(unsigned char regnum, unsigned char nbytes)
{
	unsigned long data = 0;

	// first select the register by writing 1 byte (register)
	i2cMasterSend(MEGAIO_I2C_ADDR, 1, &regnum);
	// then read n byte(s) from the selected MegaIO register
	i2cMasterReceive(MEGAIO_I2C_ADDR, nbytes, (u08*)&data);
	// return the results
	return data;
}

//! set the baudrate of the megaio serial port
void megaioSetBaudRate(u32 baudrate)
{
	megaioWriteReg(MEGAIOREG_UARTBAUD, 4, baudrate);
}

//! send a byte out the megaio serial port
void megaioSendByte(u08 data)
{
	megaioWriteReg(MEGAIOREG_UARTDATA, 1, data);
}

//! get a byte from the megaio serial port
int megaioGetByte(void)
{
	u08 data;

	// check the number of bytes in the megaio receive buffer
	if( megaioReadReg(MEGAIOREG_UARTRXBUFBYTES, 1) )
	{
		// one or more bytes are available
		// get first byte
		data = megaioReadReg(MEGAIOREG_UARTDATA, 1);
		return data;
	}
	else
	{
		// no bytes were available
		// (no bytes have arrived and are waiting to be read)
		return -1;
	}
}

//! returns the receive buffer structure 
cBuffer* megaioGetRxBuffer(void)
{
	u08 nbytes;
	// get the number of bytes waiting in the MegaIO buffer
	nbytes = megaioReadReg(MEGAIOREG_UARTRXBUFBYTES, 1);
	// get all available bytes from the MegaIO chip
	// and add them to the receive buffer
	while(megaioReadReg(MEGAIOREG_UARTRXBUFBYTES, 1))
	{
		bufferAddToEnd(&megaioUartRxBuffer, megaioReadReg(MEGAIOREG_UARTDATA, 1));
		nbytes--;
	}
	// return rx buffer pointer
	return &megaioUartRxBuffer;
}

//! turn on megaio PWM and set for bitRes resolution
void megaioPWMInit(u08 bitRes)
{
	megaioWriteReg(MEGAIOREG_PWM1CTRL, 1, bitRes);
}

//! turn off megaio PWM
void megaioPWMOff(void)
{
	megaioWriteReg(MEGAIOREG_PWM1CTRL, 1, 0);
}

//! set megaio PWM1A duty cycle
void megaioPWMASet(u16 pwmDuty)
{
	megaioWriteReg(MEGAIOREG_PWM1ADUTY, 2, pwmDuty);
}

//! set megaio PWM1B duty cycle
void megaioPWMBSet(u16 pwmDuty)
{
	megaioWriteReg(MEGAIOREG_PWM1BDUTY, 2, pwmDuty);
}

//! set megaio prescaler division rate
void megaioSetPrescaler(u08 prescaleDiv)
{
	megaioWriteReg(MEGAIOREG_PWM1FREQ, 1, prescaleDiv);
}

//! do A/D conversion on channel [ch] and return result
u16 megaioA2DConvert(u08 ch)
{
	// set channel
	megaioWriteReg(MEGAIOREG_ADCCHSEL, 1, ch);
	// start single conversion
	megaioWriteReg(MEGAIOREG_ADCCTRL, 1, 0x01);
	// wait for conversion to be complete
	while( megaioReadReg(MEGAIOREG_ADCCTRL, 1) );
	// get result and return it
	return megaioReadReg(MEGAIOREG_ADCRESULT, 2);
}
