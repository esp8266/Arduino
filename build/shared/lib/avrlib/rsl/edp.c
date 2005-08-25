/*! \file edp.c \brief Emerald Data Protocol System. */
//*****************************************************************************
//
// File Name	: 'edp.c'
// Title		: Emerald Data Protocol System
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.07.01
// Revised		: 2003.07.21
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>	// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support
#include <avr/pgmspace.h>	// include program-space support

#include "global.h"		// include our global settings
#include "i2c.h"		// include I2C support
#include "rprintf.h"	// include printf function library

#include "edp.h"

// globals
// EDP master/command: response code and reply buffer
u08 EdpCommandResponseCode;
//u08 EdpCommandReplyLength;
u08 EdpCommandReplyBuffer[EDP_REPLY_BUFFER_SIZE];
u08 EdpCommandReplyChecksum;
// EDP slave: response code and reply buffer
u08 EdpSlaveResponseCode;
u08 EdpSlaveReplyLength;
u08 EdpSlaveReplyBuffer[EDP_REPLY_BUFFER_SIZE];
// EDP slave request handler function pointer
EdpSlaveHandlerFuncType edpSlaveHandlerFunc;

// functions
void edpInit(void)
{
	// initialize i2c interface and function library
	i2cInit();
	// set i2c bit rate to 30KHz
	i2cSetBitrate(30);
	// set the Slave Receive Handler function
	// (this function will run whenever a master somewhere else on the bus
	//  writes data to us as a slave)
	i2cSetSlaveReceiveHandler( edpSlaveReceiveService );
	// set the Slave Transmit Handler function
	// (this function will run whenever a master somewhere else on the bus
	//  attempts to read data from us as a slave)
	i2cSetSlaveTransmitHandler( edpSlaveTransmitService );
}

void edpSetSlaveHandler(EdpSlaveHandlerFuncType edpSlaveHandlerFunction)
{
	edpSlaveHandlerFunc = edpSlaveHandlerFunction;
}

// ************ EDP Master operations ************
u08 edpSendCommand(u08 deviceAddr, u08 cmdLength, EdpCommand* edpCommand)
{
	EdpReply* edpCommandReply = (EdpReply*)EdpCommandReplyBuffer;
	u08* sendData;
	u08* replyData;
	u08 replyLength;
	u08 checksum;

	// initialize response variables
	edpCommandReply->Length = 0;
	EdpCommandReplyChecksum = 0;

	#ifdef EDP_DEBUG
	rprintf("\r\nBegin EdpSendCommand, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// disable TWI interrupt
	cbi(TWCR, TWIE);

	// clear TWI interface
	//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK));

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Start, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// send device address with write
	i2cSendByte( (deviceAddr&0xFE) );
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Device Address+Write, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// check if device is present and live
	if( i2cGetStatus() != TW_MT_SLA_ACK)
	{
		// device did not ACK it's address, command will not continue
		// transmit stop condition
		// leave with TWEA on for slave receiving
		i2cSendStop();
		while( !(inb(TWCR) & BV(TWSTO)) );
		#ifdef EDP_DEBUG
		rprintf("No Device!, Sent Stop, TWSR:0x%x\r\n",inb(TWSR));
		#endif
		// enable TWI interrupt
		sbi(TWCR, TWIE);
		// return error
		return EDP_COMMAND_NODEV;
	}

	// send data
	sendData = (u08*)edpCommand;
	checksum = 0;
	while(cmdLength)
	{
		i2cSendByte( *sendData );
		i2cWaitForComplete();
		#ifdef EDP_DEBUG
		rprintf("Sent Data, TWSR:0x%x\r\n",inb(TWSR));
		#endif
		checksum += *sendData++;
		cmdLength--;
	}

	// send the checksum
	i2cSendByte( ~checksum );
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Checksum, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// send repeated start condition
	i2cSendStart();
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Repeated Start, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// send device address with read
	i2cSendByte( deviceAddr|0x01 );
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Device Address+Read, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// read response code, return NACK
	i2cReceiveByte(FALSE);
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Read Data, TWSR:0x%x\r\n",inb(TWSR));
	#endif
	EdpCommandResponseCode = i2cGetReceivedByte();

	if(EdpCommandResponseCode==EDP_RESP_DATA_REPLY)
	{
		// a data reply is being sent

		// send repeated start condition
		i2cSendStart();
		i2cWaitForComplete();
		
		// send device address with read
		i2cSendByte( deviceAddr|0x01 );
		i2cWaitForComplete();

		// get length, return ACK
		i2cReceiveByte(TRUE);
		i2cWaitForComplete();
		edpCommandReply->Length = i2cGetReceivedByte();
		// set temp variables
		replyLength = edpCommandReply->Length;
		replyData = edpCommandReply->Data;

		// get data, return ACKs
		// preset checksum with the datalength byte
		checksum = replyLength;
		while(replyLength > 1)
		{
			i2cReceiveByte(TRUE);	// receive data byte and return ACK
			i2cWaitForComplete();
			*replyData = i2cGetReceivedByte();
			checksum += *replyData++;
			replyLength--;
		}

		// get last data (actually the checksum), return NACK (last-byte signal)
		i2cReceiveByte(FALSE);
		i2cWaitForComplete();
		*replyData = i2cGetReceivedByte();
		// add received checksum+1 to our checksum, the result should be zero
		checksum += (*replyData) + 1;
		// save the reply checksum
		EdpCommandReplyChecksum = checksum;
	}
	
	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();
	while( !(inb(TWCR) & BV(TWSTO)) );
	#ifdef EDP_DEBUG
	rprintf("Sent Stop, TWSR:0x%x\r\n",inb(TWSR));
	#endif
	
	// enable TWI interrupt
	sbi(TWCR, TWIE);

	return EDP_COMMAND_OK;
}

// get the response code and reply from last command
u08 edpGetCommandReply(u08* responseCode, EdpReply** edpReply)
{
	u08 retval=EDP_REPLY_OK;

	// get the response code from last command
	*responseCode = EdpCommandResponseCode;
	// get the reply from last command
	*edpReply = (EdpReply*)EdpCommandReplyBuffer;

	// check response code
	if(EdpCommandResponseCode == EDP_RESP_DATA_REPLY)
	{
		// there was a reply, check the checksum
		// if it's non-zero, data corruption is present
		if(EdpCommandReplyChecksum)
			retval = EDP_REPLY_BADCHKSUM;
	}
	return retval;
}

/*
u08 edpSendCommand(u08 deviceAddr, u08 sendLength, u08* sendData)
{
	u08* replyData = EdpCommandReplyBuffer;
	u08 replyLength;
	u08 checksum;

	// initialize response variables
	EdpCommandReplyLength = 0;
	EdpCommandReplyChecksum = 0;

	#ifdef EDP_DEBUG
	rprintf("\r\nBegin EdpSendCommand, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// disable TWI interrupt
	cbi(TWCR, TWIE);

	// clear TWI interface
	//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK));

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Start, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// send device address with write
	i2cSendByte( (deviceAddr&0xFE) );
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Device Address+Write, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// check if device is present and live
	if( i2cGetStatus() != TW_MT_SLA_ACK)
	{
		// device did not ACK it's address, command will not continue
		// transmit stop condition
		// leave with TWEA on for slave receiving
		i2cSendStop();
		while( !(inb(TWCR) & BV(TWSTO)) );
		#ifdef EDP_DEBUG
		rprintf("No Device!, Sent Stop, TWSR:0x%x\r\n",inb(TWSR));
		#endif
		// enable TWI interrupt
		sbi(TWCR, TWIE);
		// return error
		return EDP_COMMAND_NODEV;
	}

	// send data
	checksum = 0;
	while(sendLength)
	{
		i2cSendByte( *sendData );
		i2cWaitForComplete();
		#ifdef EDP_DEBUG
		rprintf("Sent Data, TWSR:0x%x\r\n",inb(TWSR));
		#endif
		checksum += *sendData++;
		sendLength--;
	}

	// send the checksum
	i2cSendByte( ~checksum );
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Checksum, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// send repeated start condition
	i2cSendStart();
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Repeated Start, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// send device address with read
	i2cSendByte( deviceAddr|0x01 );
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Sent Device Address+Read, TWSR:0x%x\r\n",inb(TWSR));
	#endif

	// read response code, return NACK
	i2cReceiveByte(FALSE);
	i2cWaitForComplete();
	#ifdef EDP_DEBUG
	rprintf("Read Data, TWSR:0x%x\r\n",inb(TWSR));
	#endif
	EdpCommandResponseCode = i2cGetReceivedByte();

	if(EdpCommandResponseCode==EDP_RESP_DATA_REPLY)
	{
		// a data reply is being sent

		// send repeated start condition
		i2cSendStart();
		i2cWaitForComplete();
		
		// send device address with read
		i2cSendByte( deviceAddr|0x01 );
		i2cWaitForComplete();

		// get length, return ACK
		i2cReceiveByte(TRUE);
		i2cWaitForComplete();
		replyLength = i2cGetReceivedByte();
		EdpCommandReplyLength = replyLength;

		// get data, return ACKs
		// preset checksum with the datalength byte
		checksum = replyLength;
		while(replyLength > 1)
		{
			i2cReceiveByte(TRUE);	// receive data byte and return ACK
			i2cWaitForComplete();
			*replyData = i2cGetReceivedByte();
			checksum += *replyData++;
			replyLength--;
		}

		// get last data (actually the checksum), return NACK (last-byte signal)
		i2cReceiveByte(FALSE);
		i2cWaitForComplete();
		*replyData = i2cGetReceivedByte();
		// add received checksum+1 to our checksum, the result should be zero
		checksum += (*replyData) + 1;
		// save the reply checksum
		EdpCommandReplyChecksum = checksum;
	}
	
	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();
	while( !(inb(TWCR) & BV(TWSTO)) );
	#ifdef EDP_DEBUG
	rprintf("Sent Stop, TWSR:0x%x\r\n",inb(TWSR));
	#endif
	
	// enable TWI interrupt
	sbi(TWCR, TWIE);

	return EDP_COMMAND_OK;
}

u08 edpGetCommandReply(u08* responseCode, u08* replyLength, u08** replyData)
{
	u08 retval=EDP_REPLY_OK;

	// get the response code and reply data from last command
	*responseCode = EdpCommandResponseCode;
	// get the reply length from last command
	*replyLength = EdpCommandReplyLength;
	// get the reply data from last command
	*replyData = EdpCommandReplyBuffer;

	// check response code
	if(EdpCommandResponseCode == EDP_RESP_DATA_REPLY)
	{
		// there was a reply, check the checksum
		// if it's non-zero, data corruption is present
		if(EdpCommandReplyChecksum)
			retval = EDP_REPLY_BADCHKSUM;
	}
	return retval;
}
*/

// ************ EDP Slave operations ************

// this function will run when a master somewhere else on the bus
// addresses us and wishes to write data to us
void edpSlaveReceiveService(u08 receiveDataLength, u08* receiveData)
{
	u08 i,checksum;

	// initialize the reply length from this command
	EdpSlaveReplyLength = 0;
	// verify the checksum
	// initialize the checksum with 1
	checksum = 0x01;
	// sum all the data in the packet and the data's checksum
	for(i=0; i<receiveDataLength; i++)
	{
		checksum += receiveData[i];
	}
	// if the checksum is non-zero, then the data is corrupt
	if(checksum)
	{
		// set reply code
		// [FIX] which should it be?
		EdpSlaveResponseCode = EDP_RESP_DATA_CHK_ERROR;
		//EdpSlaveResponseCode = EDP_RESP_CMD_CHK_ERROR;
		return;
	}

	// make an EDP command pointer to the received I2C data
	EdpCommand* edpCommand = (EdpCommand*)receiveData;

	// if a slave handler is defined
	if(edpSlaveHandlerFunc)
	{
		// then use it
		EdpSlaveResponseCode = edpSlaveHandlerFunc(
			receiveDataLength, edpCommand, 
			EDP_REPLY_BUFFER_SIZE, (EdpReply*)EdpSlaveReplyBuffer);
	}
	else
	{
		// otherwise reply with unknown command
		EdpSlaveResponseCode = EDP_RESP_UNKWN_CMD;
	}
}

// this function will run when a master somewhere else on the bus
// addresses us and wishes to read data from us
u08 edpSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData)
{
	u08 i;
	u08 checksum;
	u08 transmitDataLength = 0;

	EdpReply* edpReply = (EdpReply*)EdpSlaveReplyBuffer;

	if(EdpSlaveResponseCode)
	{
		// reply code is non-zero, we must send it
		*transmitData = EdpSlaveResponseCode;
		transmitDataLength = 1;
		// reset the reply code to flag that we've sent it
		EdpSlaveResponseCode = 0;
	}
	else
	{
		// reply code already sent, now send data (if any)
		// copy length of reply to transmit buffer (+1 for checksum)
		*transmitData++ = edpReply->Length+1;
		// initialize checksum
		checksum = edpReply->Length+1;
		// copy reply buffer to the transmit buffer
		for(i=0; i<edpReply->Length; i++)
		{
			*transmitData++ = edpReply->Data[i];
			checksum += edpReply->Data[i];
		}
		// copy checksum to transmit buffer
		*transmitData++ = ~checksum;
		// set number of bytes to transmit
		transmitDataLength = edpReply->Length+2;
	}

	// return number of bytes written to transmit buffer
	return transmitDataLength;
}
