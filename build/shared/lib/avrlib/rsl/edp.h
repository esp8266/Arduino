/*! \file edp.h \brief Emerald Data Protocol System. */
//*****************************************************************************
//
// File Name	: 'edp.h'
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

#ifndef EDP_H
#define EDP_H

#include "edpdefs.h"

// defines
//#define EDP_DEBUG
// edp reply buffer size
#ifndef EDP_REPLY_BUFFER_SIZE
#define EDP_REPLY_BUFFER_SIZE	128
#endif
// edpSendCommand return values
#define EDP_COMMAND_OK		0
#define EDP_COMMAND_NODEV	1
// edpGetCommandReply return values
#define EDP_REPLY_OK		0
#define EDP_REPLY_BADCHKSUM	1

// structs and typedefs
typedef struct
{
	u08 SrcAddr;
	u08 Command;
	u08 Data[];
} EdpCommand;

typedef struct
{
	u08 Length;
	u08 Data[];
} EdpReply;

// typedefs
typedef u08 (*EdpSlaveHandlerFuncType)(u08 edpCmdLength, EdpCommand* edpCmd,
					u08 edpReplyLengthMax, EdpReply* edpReply);

// functions
void edpInit(void);
void edpSetSlaveHandler(EdpSlaveHandlerFuncType edpSlaveHandlerFunction);

// ************ EDP Master operations ************
u08 edpSendCommand(u08 deviceAddr, u08 cmdLength, EdpCommand* edpCommand);
u08 edpGetCommandReply(u08* responseCode, EdpReply** edpReply);
//u08 edpSendCommand(u08 deviceAddr, u08 sendLength, u08* sendData);
//u08 edpGetCommandReply(u08* responseCode, u08* replyLength, u08** replyData);

// ************ EDP Slave operations ************
void edpSlaveReceiveService(u08 receiveDataLength, u08* receiveData);
u08 edpSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData);

#endif
