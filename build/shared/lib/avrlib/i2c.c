/*! \file i2c.c \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Description : I2C (pronounced "eye-squared-see") is a two-wire bidirectional
//		network designed for easy transfer of information between a wide variety
//		of intelligent devices.  Many of the Atmel AVR series processors have
//		hardware support for transmitting and receiving using an I2C-type bus.
//		In addition to the AVRs, there are thousands of other parts made by
//		manufacturers like Philips, Maxim, National, TI, etc that use I2C as
//		their primary means of communication and control.  Common device types
//		are A/D & D/A converters, temp sensors, intelligent battery monitors,
//		MP3 decoder chips, EEPROM chips, multiplexing switches, etc.
//
//		I2C uses only two wires (SDA and SCL) to communicate bidirectionally
//		between devices.  I2C is a multidrop network, meaning that you can have
//		several devices on a single bus.  Because I2C uses a 7-bit number to
//		identify which device it wants to talk to, you cannot have more than
//		127 devices on a single bus.
//
//		I2C ordinarily requires two 4.7K pull-up resistors to power (one each on
//		SDA and SCL), but for small numbers of devices (maybe 1-4), it is enough
//		to activate the internal pull-up resistors in the AVR processor.  To do
//		this, set the port pins, which correspond to the I2C pins SDA/SCL, high.
//		For example, on the mega163, sbi(PORTC, 0); sbi(PORTC, 1);.
//
//		For complete information about I2C, see the Philips Semiconductor
//		website.  They created I2C and have the largest family of devices that
//		work with I2C.
//
// Note: Many manufacturers market I2C bus devices under a different or generic
//		bus name like "Two-Wire Interface".  This is because Philips still holds
//		"I2C" as a trademark.  For example, SMBus and SMBus devices are hardware
//		compatible and closely related to I2C.  They can be directly connected
//		to an I2C bus along with other I2C devices are are generally accessed in
//		the same way as I2C devices.  SMBus is often found on modern motherboards
//		for temp sensing and other low-level control tasks.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "i2c.h"

#include "rprintf.h"	// include printf function library
#include "uart2.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

//#define I2C_DEBUG

// I2C state and address variables
static volatile eI2cStateType I2cState;
static u08 I2cDeviceAddrRW;
// send/transmit buffer (outgoing data)
static u08 I2cSendData[I2C_SEND_DATA_BUFFER_SIZE];
static u08 I2cSendDataIndex;
static u08 I2cSendDataLength;
// receive buffer (incoming data)
static u08 I2cReceiveData[I2C_RECEIVE_DATA_BUFFER_SIZE];
static u08 I2cReceiveDataIndex;
static u08 I2cReceiveDataLength;

// function pointer to i2c receive routine
//! I2cSlaveReceive is called when this processor
// is addressed as a slave for writing
static void (*i2cSlaveReceive)(u08 receiveDataLength, u08* recieveData);
//! I2cSlaveTransmit is called when this processor
// is addressed as a slave for reading
static u08 (*i2cSlaveTransmit)(u08 transmitDataLengthMax, u08* transmitData);

// functions
void i2cInit(void)
{
	// set pull-up resistors on I2C bus pins
	// TODO: should #ifdef these
	sbi(PORTC, 0);	// i2c SCL on ATmega163,323,16,32,etc
	sbi(PORTC, 1);	// i2c SDA on ATmega163,323,16,32,etc
	sbi(PORTD, 0);	// i2c SCL on ATmega128,64
	sbi(PORTD, 1);	// i2c SDA on ATmega128,64

	// clear SlaveReceive and SlaveTransmit handler to null
	i2cSlaveReceive = 0;
	i2cSlaveTransmit = 0;
	// set i2c bit rate to 100KHz
	i2cSetBitrate(100);
	// enable TWI (two-wire interface)
	sbi(TWCR, TWEN);
	// set state
	I2cState = I2C_IDLE;
	// enable TWI interrupt and slave address ACK
	sbi(TWCR, TWIE);
	sbi(TWCR, TWEA);
	//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	// enable interrupts
	sei();
}

void i2cSetBitrate(u16 bitrateKHz)
{
	u08 bitrate_div;
	// set i2c bitrate
	// SCL freq = F_CPU/(16+2*TWBR))
	#ifdef TWPS0
		// for processors with additional bitrate division (mega128)
		// SCL freq = F_CPU/(16+2*TWBR*4^TWPS)
		// set TWPS to zero
		cbi(TWSR, TWPS0);
		cbi(TWSR, TWPS1);
	#endif
	// calculate bitrate division	
	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div-16)/2;
	outb(TWBR, bitrate_div);
}

void i2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn)
{
	// set local device address (used in slave mode only)
	outb(TWAR, ((deviceAddr&0xFE) | (genCallEn?1:0)) );
}

void i2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(u08 receiveDataLength, u08* recieveData))
{
	i2cSlaveReceive = i2cSlaveRx_func;
}

void i2cSetSlaveTransmitHandler(u08 (*i2cSlaveTx_func)(u08 transmitDataLengthMax, u08* transmitData))
{
	i2cSlaveTransmit = i2cSlaveTx_func;
}

inline void i2cSendStart(void)
{
	// send start condition
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
}

inline void i2cSendStop(void)
{
	// transmit stop condition
	// leave with TWEA on for slave receiving
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA)|BV(TWSTO));
}

inline void i2cWaitForComplete(void)
{
	// wait for i2c interface to complete operation
	while( !(inb(TWCR) & BV(TWINT)) );
}

inline void i2cSendByte(u08 data)
{
	// save data to the TWDR
	outb(TWDR, data);
	// begin send
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
}

inline void i2cReceiveByte(u08 ackFlag)
{
	// begin receive over i2c
	if( ackFlag )
	{
		// ackFlag = TRUE: ACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}
	else
	{
		// ackFlag = FALSE: NACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}
}

inline u08 i2cGetReceivedByte(void)
{
	// retieve received data byte from i2c TWDR
	return( inb(TWDR) );
}

inline u08 i2cGetStatus(void)
{
	// retieve current i2c status from i2c TWSR
	return( inb(TWSR) );
}

void i2cMasterSend(u08 deviceAddr, u08 length, u08* data)
{
	u08 i;
	// wait for interface to be ready
	while(I2cState);
	// set state
	I2cState = I2C_MASTER_TX;
	// save data
	I2cDeviceAddrRW = (deviceAddr & 0xFE);	// RW cleared: write operation
	for(i=0; i<length; i++)
		I2cSendData[i] = *data++;
	I2cSendDataIndex = 0;
	I2cSendDataLength = length;
	// send start condition
	i2cSendStart();
}

void i2cMasterReceive(u08 deviceAddr, u08 length, u08* data)
{
	u08 i;
	// wait for interface to be ready
	while(I2cState);
	// set state
	I2cState = I2C_MASTER_RX;
	// save data
	I2cDeviceAddrRW = (deviceAddr|0x01);	// RW set: read operation
	I2cReceiveDataIndex = 0;
	I2cReceiveDataLength = length;
	// send start condition
	i2cSendStart();
	// wait for data
	while(I2cState);
	// return data
	for(i=0; i<length; i++)
		*data++ = I2cReceiveData[i];
}

u08 i2cMasterSendNI(u08 deviceAddr, u08 length, u08* data)
{
	u08 retval = I2C_OK;

	// disable TWI interrupt
	cbi(TWCR, TWIE);

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();

	// send device address with write
	i2cSendByte( deviceAddr & 0xFE );
	i2cWaitForComplete();

	// check if device is present and live
	if( inb(TWSR) == TW_MT_SLA_ACK)
	{
		// send data
		while(length)
		{
			i2cSendByte( *data++ );
			i2cWaitForComplete();
			length--;
		}
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();
	while( !(inb(TWCR) & BV(TWSTO)) );

	// enable TWI interrupt
	sbi(TWCR, TWIE);

	return retval;
}

u08 i2cMasterReceiveNI(u08 deviceAddr, u08 length, u08 *data)
{
	u08 retval = I2C_OK;

	// disable TWI interrupt
	cbi(TWCR, TWIE);

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();

	// send device address with read
	i2cSendByte( deviceAddr | 0x01 );
	i2cWaitForComplete();

	// check if device is present and live
	if( inb(TWSR) == TW_MR_SLA_ACK)
	{
		// accept receive data and ack it
		while(length > 1)
		{
			i2cReceiveByte(TRUE);
			i2cWaitForComplete();
			*data++ = i2cGetReceivedByte();
			// decrement length
			length--;
		}

		// accept receive data and nack it (last-byte signal)
		i2cReceiveByte(FALSE);
		i2cWaitForComplete();
		*data++ = i2cGetReceivedByte();
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();

	// enable TWI interrupt
	sbi(TWCR, TWIE);

	return retval;
}
/*
void i2cMasterTransferNI(u08 deviceAddr, u08 sendlength, u08* senddata, u08 receivelength, u08* receivedata)
{
	// disable TWI interrupt
	cbi(TWCR, TWIE);

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();

	// if there's data to be sent, do it
	if(sendlength)
	{
		// send device address with write
		i2cSendByte( deviceAddr & 0xFE );
		i2cWaitForComplete();
		
		// send data
		while(sendlength)
		{
			i2cSendByte( *senddata++ );
			i2cWaitForComplete();
			sendlength--;
		}
	}

	// if there's data to be received, do it
	if(receivelength)
	{
		// send repeated start condition
		i2cSendStart();
		i2cWaitForComplete();

		// send device address with read
		i2cSendByte( deviceAddr | 0x01 );
		i2cWaitForComplete();

		// accept receive data and ack it
		while(receivelength > 1)
		{
			i2cReceiveByte(TRUE);
			i2cWaitForComplete();
			*receivedata++ = i2cGetReceivedByte();
			// decrement length
			receivelength--;
		}

		// accept receive data and nack it (last-byte signal)
		i2cReceiveByte(TRUE);
		i2cWaitForComplete();
		*receivedata++ = i2cGetReceivedByte();
	}
	
	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();
	while( !(inb(TWCR) & BV(TWSTO)) );

	// enable TWI interrupt
	sbi(TWCR, TWIE);
}
*/

//! I2C (TWI) interrupt service routine
SIGNAL(SIG_2WIRE_SERIAL)
{
	// read status bits
	u08 status = inb(TWSR) & TWSR_STATUS_MASK;

	switch(status)
	{
	// Master General
	case TW_START:						// 0x08: Sent start condition
	case TW_REP_START:					// 0x10: Sent repeated start condition
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: M->START\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// send device address
		i2cSendByte(I2cDeviceAddrRW);
		break;
	
	// Master Transmitter & Receiver status codes
	case TW_MT_SLA_ACK:					// 0x18: Slave address acknowledged
	case TW_MT_DATA_ACK:				// 0x28: Data acknowledged
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: MT->SLA_ACK or DATA_ACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		if(I2cSendDataIndex < I2cSendDataLength)
		{
			// send data
			i2cSendByte( I2cSendData[I2cSendDataIndex++] );
		}
		else
		{
			// transmit stop condition, enable SLA ACK
			i2cSendStop();
			// set state
			I2cState = I2C_IDLE;
		}
		break;
	case TW_MR_DATA_NACK:				// 0x58: Data received, NACK reply issued
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: MR->DATA_NACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// store final received data byte
		I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
		// continue to transmit STOP condition
	case TW_MR_SLA_NACK:				// 0x48: Slave address not acknowledged
	case TW_MT_SLA_NACK:				// 0x20: Slave address not acknowledged
	case TW_MT_DATA_NACK:				// 0x30: Data not acknowledged
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: MTR->SLA_NACK or MT->DATA_NACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// transmit stop condition, enable SLA ACK
		i2cSendStop();
		// set state
		I2cState = I2C_IDLE;
		break;
	case TW_MT_ARB_LOST:				// 0x38: Bus arbitration lost
	//case TW_MR_ARB_LOST:				// 0x38: Bus arbitration lost
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: MT->ARB_LOST\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// release bus
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		// set state
		I2cState = I2C_IDLE;
		// release bus and transmit start when bus is free
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
		break;
	case TW_MR_DATA_ACK:				// 0x50: Data acknowledged
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: MR->DATA_ACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// store received data byte
		I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
		// fall-through to see if more bytes will be received
	case TW_MR_SLA_ACK:					// 0x40: Slave address acknowledged
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: MR->SLA_ACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		if(I2cReceiveDataIndex < (I2cReceiveDataLength-1))
			// data byte will be received, reply with ACK (more bytes in transfer)
			i2cReceiveByte(TRUE);
		else
			// data byte will be received, reply with NACK (final byte in transfer)
			i2cReceiveByte(FALSE);
		break;

	// Slave Receiver status codes
	case TW_SR_SLA_ACK:					// 0x60: own SLA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_SLA_ACK:		// 0x68: own SLA+W has been received, ACK has been returned
	case TW_SR_GCALL_ACK:				// 0x70:     GCA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_GCALL_ACK:		// 0x78:     GCA+W has been received, ACK has been returned
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: SR->SLA_ACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// we are being addressed as slave for writing (data will be received from master)
		// set state
		I2cState = I2C_SLAVE_RX;
		// prepare buffer
		I2cReceiveDataIndex = 0;
		// receive data byte and return ACK
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		break;
	case TW_SR_DATA_ACK:				// 0x80: data byte has been received, ACK has been returned
	case TW_SR_GCALL_DATA_ACK:			// 0x90: data byte has been received, ACK has been returned
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: SR->DATA_ACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// get previously received data byte
		I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
		// check receive buffer status
		if(I2cReceiveDataIndex < I2C_RECEIVE_DATA_BUFFER_SIZE)
		{
			// receive data byte and return ACK
			i2cReceiveByte(TRUE);
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		}
		else
		{
			// receive data byte and return NACK
			i2cReceiveByte(FALSE);
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		}
		break;
	case TW_SR_DATA_NACK:				// 0x88: data byte has been received, NACK has been returned
	case TW_SR_GCALL_DATA_NACK:			// 0x98: data byte has been received, NACK has been returned
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: SR->DATA_NACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// receive data byte and return NACK
		i2cReceiveByte(FALSE);
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		break;
	case TW_SR_STOP:					// 0xA0: STOP or REPEATED START has been received while addressed as slave
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: SR->SR_STOP\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// switch to SR mode with SLA ACK
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		// i2c receive is complete, call i2cSlaveReceive
		if(i2cSlaveReceive) i2cSlaveReceive(I2cReceiveDataIndex, I2cReceiveData);
		// set state
		I2cState = I2C_IDLE;
		break;

	// Slave Transmitter
	case TW_ST_SLA_ACK:					// 0xA8: own SLA+R has been received, ACK has been returned
	case TW_ST_ARB_LOST_SLA_ACK:		// 0xB0:     GCA+R has been received, ACK has been returned
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: ST->SLA_ACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// we are being addressed as slave for reading (data must be transmitted back to master)
		// set state
		I2cState = I2C_SLAVE_TX;
		// request data from application
		if(i2cSlaveTransmit) I2cSendDataLength = i2cSlaveTransmit(I2C_SEND_DATA_BUFFER_SIZE, I2cSendData);
		// reset data index
		I2cSendDataIndex = 0;
		// fall-through to transmit first data byte
	case TW_ST_DATA_ACK:				// 0xB8: data byte has been transmitted, ACK has been received
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: ST->DATA_ACK\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// transmit data byte
		outb(TWDR, I2cSendData[I2cSendDataIndex++]);
		if(I2cSendDataIndex < I2cSendDataLength)
			// expect ACK to data byte
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		else
			// expect NACK to data byte
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		break;
	case TW_ST_DATA_NACK:				// 0xC0: data byte has been transmitted, NACK has been received
	case TW_ST_LAST_DATA:				// 0xC8:
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: ST->DATA_NACK or LAST_DATA\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// all done
		// switch to open slave
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		// set state
		I2cState = I2C_IDLE;
		break;

	// Misc
	case TW_NO_INFO:					// 0xF8: No relevant state information
		// do nothing
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: NO_INFO\r\n");
		rprintfInit(uart1SendByte);
		#endif
		break;
	case TW_BUS_ERROR:					// 0x00: Bus error due to illegal start or stop condition
		#ifdef I2C_DEBUG
		rprintfInit(uart1AddToTxBuffer);
		rprintf("I2C: BUS_ERROR\r\n");
		rprintfInit(uart1SendByte);
		#endif
		// reset internal hardware and release bus
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
		// set state
		I2cState = I2C_IDLE;
		break;
	}
}

eI2cStateType i2cGetState(void)
{
	return I2cState;
}
