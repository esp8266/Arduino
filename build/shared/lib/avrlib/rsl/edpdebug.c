/*! \file edpdebug.c \brief Emerald Data Protocol Debug Functions. */
//*****************************************************************************
//
// File Name	: 'edpdebug.c'
// Title		: Emerald Data Protocol Debug Functions
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.09.20
// Revised		: 2003.09.20
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
#include "rprintf.h"	// include printf function library
#include "debug.h"		// include debug helper library
#include "input.h"		// include user-input functions

#include "edp.h"
#include "edpdebug.h"

// globals

// functions

void edpDisplayCommand(u08 length, EdpCommand* edpCommand)
{
	// print source and command char
	rprintf("EDP SrcAddr: 0x%x  Cmd: 0x%x '%c'\r\n",
		edpCommand->SrcAddr,
		edpCommand->Command, edpCommand->Command);
	if(length-2)
	{
		// print data
		rprintf("Data:\r\n");
		debugPrintHexTable(length-2, edpCommand->Data);
	}
}

void edpDisplayReply(u08 response, EdpReply* edpReply)
{
	u08 i;
	u08 checksum;

	// display response
	rprintf("EDP Response: 0x%x '%c'\r\n",response,response);

	// if data was received
	if(response==EDP_RESP_DATA_REPLY)
	{
		// do checksum on reply
		checksum = edpReply->Length;
		for(i=0; i<(edpReply->Length-1); i++)
		{
			checksum += edpReply->Data[i];
		}
		checksum = ~checksum;
		// print message
		rprintf("EDP Reply: ");
		// show data received
		rprintf("Length: 0x%x ",edpReply->Length);
		rprintf("RxChksum=0x%x MyChksum=0x%x",edpReply->Data[edpReply->Length-1], checksum);
		rprintfCRLF();
		rprintf("Data:\r\n");
		debugPrintHexTable((edpReply->Length-1), edpReply->Data);
		rprintfCRLF();
	}
}
/*
void edpDisplayReplyOld(u08 response, u08 replyLength, u08* replyData)
{
	u08 i;
	u08 checksum;

	// display response
	rprintf("EDP Response: 0x%x '%c'\r\n",response,response);

	// if data was received
	if(response==EDP_RESP_DATA_REPLY)
	{
		// do checksum on reply
		checksum = replyLength;
		for(i=0; i<(replyLength-1); i++)
		{
			checksum += replyData[i];
		}
		checksum = ~checksum;
		// print message
		rprintf("EDP Reply: ");
		// show data received
		rprintf("Length: 0x%x ",replyLength);
		rprintf("RxChksum=0x%x MyChksum=0x%x",replyData[replyLength-1], checksum);
		rprintfCRLF();
		rprintf("Data:\r\n");
		debugPrintHexTable((replyLength-1), replyData);
		rprintfCRLF();
	}
}
*/

u08 edpComposeCommand(u08 srcEdpAddr, u08* cmdBuffer)
{
	u08 string[80];
	u08 len;
	u08 i;

	// instructions
	rprintfProgStrM("Enter EDP Command, format [c][p1][p2][p3]...[pN]\r\n");
	rprintfProgStrM("[c] is command char, [px] parameters in 2-digit hex\r\n");

	// get user input
	rprintfProgStrM("EDP Command>");
	len = inputString(0x0D, 80, string);
	rprintfCRLF();

	// check for null user input
	if(!len)
	{
		rprintfProgStrM("ERROR: No command\r\n");
		// return immediately with zero command length
		return 0;
	}

	// prepare command
	cmdBuffer[0] = srcEdpAddr;
	cmdBuffer[1] = string[0];
	for(i=0; i<len/2; i++)
	{
		cmdBuffer[i+2] = asciiHexToByte(&string[1+(i*2)]);
	}

	// return command length
	return 2+(len/2);
}

void edpRunCommand(u08 destEdpAddr, u08 cmdLength, u08* cmdBuffer)
{
	u08 response;
	EdpReply* edpReply;

	u08 retval;
	
	EdpCommand* edpCommand = (EdpCommand*)cmdBuffer;

	// send command
	rprintf("Sending Command: 0x%x '%c' ->",edpCommand->Command,edpCommand->Command);
	retval = edpSendCommand(destEdpAddr, cmdLength, edpCommand);
	// handle result values
	if(retval == EDP_COMMAND_OK)
	{
		// command sent successfully
		rprintfProgStrM("Send Success!\r\n");
	}
	else if(retval == EDP_COMMAND_NODEV)
	{
		// device did not exist
		rprintfProgStrM("Send Failed->NO DEVICE!\r\n");
		rprintf("No EDP device could be contacted at address 0x%x.\r\n", destEdpAddr);
		rprintfProgStrM("The device may be busy or not responding.\r\n");
		rprintfProgStrM("Check target device and I2C bus cabling.\r\n");
		// return immediately
		return;
	}
	else
	{
		// other error
		rprintfProgStrM("Send Failed->Unspecified Error!\r\n");
		// return immediately
		return;
	}

	// get the reply, if any, from the command
	retval = edpGetCommandReply(&response, &edpReply);
	// handle result values
	if(retval == EDP_REPLY_BADCHKSUM)
	{
		rprintf("**** Reply has bad checksum ****\r\n");
	}
	// display the reply
	edpDisplayReply(response, edpReply);
}
