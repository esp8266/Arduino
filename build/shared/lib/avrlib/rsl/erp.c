/*! \file erp.c \brief Emerald Radio Protocol System. */
//*****************************************************************************
//
// File Name	: 'erp.c'
// Title		: Emerald Radio Protocol System
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.09.10
// Revised		: 2003.09.10
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

//----- Include Files ---------------------------------------------------------
#include "global.h"		// include our global settings
#include "debug.h"		// include debug function library
#include "rprintf.h"	// include printf function library

#include "erp.h"
#include "edpdebug.h"

// globals

// functions
void erpDisplayHeader(ErpPacket* erpPacket)
{
	// show ERP packet header
	rprintf("ERP Header: Callsign=");
	rprintfStrLen(erpPacket->CallSign,0,CALLSIGN_FIELD_LEN);
	rprintf(", Trg=0x%x, Src=0x%x, Seq#=%d, Type=",
		erpPacket->ToAddress,
		erpPacket->FromAddress,
		erpPacket->SequenceNum);
	// try to decode packet type
	switch(erpPacket->Type)
	{
	case ERP_ECHO:			rprintf("ECHO"); break;
	case ERP_ECHOREPLY:		rprintf("ECHOREPLY"); break;
	case ERP_TEST:			rprintf("TEST"); break;
	case ERP_EDPCOMMAND:	rprintf("EDPCOMMAND"); break;
	case ERP_EDPREPLY:		rprintf("EDPREPLY"); break;
	case ERP_EDPREPLYNODEV:	rprintf("EDPREPLYNODEV"); break;
	default:				rprintf("0x%x", erpPacket->Type); break;
	}
	rprintfCRLF();
}

void erpDisplayPacket(ErpPacket* erpPacket, u08 pktLength)
{
	u08 i;
	u08 flag;

	// show ERP packet header
	erpDisplayHeader(erpPacket);

	// dump complete raw packet data
	if(pktLength)
	{
		// check if all characters are printable
		flag = TRUE;
		for(i=0; i<pktLength; i++)
		{
			if( ((u08*)erpPacket)[i] < 0x20 ) flag = FALSE;
		}

		// print packet data
		rprintf("Data:\r\n");
		if(flag)
		{
			// print as string
			rprintfStrLen(((u08*)erpPacket), 0, pktLength);
		}
		else
		{
			// print as hex
			debugPrintHexTable(pktLength, ((u08*)erpPacket));
		}
		rprintfCRLF();
	}
}

void erpDisplayEdpCommand(u08 length, ErpEdpCommand* erpEdpCommand)
{
	// print ERP-specific fields
	rprintf("EDP DestAddr: 0x%x\r\n", erpEdpCommand->EdpDestAddr);
	// print embedded EDP command
	edpDisplayCommand(length-1, &erpEdpCommand->EdpCommand);
}

void erpDisplayEdpReply(u08 length, ErpEdpReply* erpEdpReply)
{
	// print embedded EDP reply
	edpDisplayReply(erpEdpReply->EdpResponse, &erpEdpReply->EdpReply);
}


/*
void ErpPacketCreate(u08 targetI2cAddr, u08 pktType, u08 datalength, u08* data)
{
	// make packet structure in TxPacket memory
	struct ErpPacket* erpPacket
		= (struct ErpPacket*)TxPacket;

	// prepare Emerald packet header
	memcpy(erpPacket->CallSign, MYCALLSIGN, CALLSIGN_FIELD_LEN);
	erpPacket->ToAddress = targetI2cAddr;
	erpPacket->FromAddress = LocalI2cAddr;
	erpPacket->SequenceNum = SequenceNum++;
	erpPacket->Type = pktType;
	// copy data
	for(i=0; i<datalength; i++)
	{
		erpPacket->Data[i] = *data++;
	}
}

void ErpPacketTx(void)
{
	// STX/ETX header
	u08 stxetxStatus = 0x5A;
	u08 stxetxType   = 0xA5;
	rprintf("Sending Packet: Status: 0x%x  Type: 0x%x\r\n", stxetxStatus, stxetxType);
	radioSend(stxetxStatus, stxetxType, EMRADIOHEADER_LEN+(len/2), packet);
}
*/
