

/* Copyright (c) 2010, Peter Barrett  
**  
** Permission to use, copy, modify, and/or distribute this software for  
** any purpose with or without fee is hereby granted, provided that the  
** above copyright notice and this permission notice appear in all copies.  
** 
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
** SOFTWARE.  
*/

#include "Platform.h"

#ifdef MSC_ENABLED

//	SCSI Commands
#define TEST_UNIT_READY            0x00
#define REQUEST_SENSE              0x03
#define FORMAT_UNIT                0x04
#define INQUIRY                    0x12
#define MODE_SENSE				   0x1A
#define START_STOP_UNIT            0x1B
#define MEDIA_REMOVAL              0x1E
#define READ_FORMAT_CAPACITIES     0x23
#define READ_CAPACITY              0x25
#define READ10                     0x28
#define WRITE10                    0x2A
#define VERIFY10                   0x2F


//	In FakeFAT
void SendSector(u8 b);
void RecvSector(u8 b);

//	In USBCore
u8 RecvPacket(u8 ep, u8* dst, u8 len);
u8 SendPacket(u8 ep, const u8* src, u8 len);
void Reboot();

#define MSC_TX MSC_ENDPOINT_IN
#define MSC_RX MSC_ENDPOINT_OUT

extern const u8 P_Inquiry[36] PROGMEM;
extern const u8 P_RequestSense[18] PROGMEM;
extern const u8 P_ReadCapacity[8-3] PROGMEM; 
extern const u8 P_ModeSense[4] PROGMEM; 

typedef struct
{
	u8 signature[4];
	u8 tag[4];
	u32 transferLength;
	u8 flags;
	u8 lun;
	u8 cblength;
	u8 cb[16];
} CBW;

const u8 P_Inquiry[36] =
{
	0x00,	// Direct Access Device
	0x80,	// Removable Medium
	0x00,	// Version
	0x01,

	36-4,	// Additional Length
	0x80,	// Storage Controller Component
	0x00,
	0x00,

	'r','o','s','s','u','m','u','r',	// Vendor

	'm','i','c','r','o','t','o','u',	// Product
	'c','h',' ',' ',' ',' ',' ',' ',

	'2','.','4',' '						// Revision
};

const u8 P_RequestSense[18] = 
{
	0x70,		// Response Code 
	0x00,
//	0x02,		// Sense Key 
	0x00,
	0x00,

	0x00,
	0x00,
	0x00,
	0x0A,       //	Additional Length

	0x00,
	0x00,
	0x00,
	0x00,

	0x30,        //	ASC
	0x01,        //	ASCQ

	0x00,
	0x00,
	0x00,
	0x00
};

//	Save 3 bytes if merging
const u8 P_ReadCapacity[8-3] = 
{
//	0x00,
//	0x00,
//	0x00,	// Steal 3 zero bytes from end of P_RequestSense
	64+4,	// block count 64 + 4
	0x00,
	0x00,
	0x02,	// block size = 512
	0x00
};

const u8 P_ModeSense[4] = 
{
	0x03,
	0x00,
	0x00,
	0x00
};

extern volatile u8 _ejected;

void SCSI(CBW& cbw)
{
	const u8* pgm = 0;
	u8 status = 0;
	u8 blen = 0;

	u8 cmd = cbw.cb[0];
	if (START_STOP_UNIT == cmd)
	{
		_ejected = 1;		// Stopped; going away
	}
	else if (TEST_UNIT_READY == cmd || MEDIA_REMOVAL == cmd)
	{
#ifdef MSC_NO_MEDIA
		status = 1;
#else
		status = _ejected;
#endif
	}
	else if (REQUEST_SENSE == cmd)
	{
		pgm = P_RequestSense;
		blen = sizeof(P_RequestSense);
	}
	else if (INQUIRY == cmd)
	{
		pgm = P_Inquiry;
		blen = sizeof(P_Inquiry);
	}
	else if (WRITE10 == cmd || READ10 == cmd)
	{
		// Write 10 byte form
		// Read 10 byte form
		{
			u8 lba = cbw.cb[5];
			u8 count = cbw.cb[8];
			u8 read = READ10 == cbw.cb[0];
			while (count--)
			{
				if (read)
					SendSector(lba);
				else
					RecvSector(lba);
				lba++;
				cbw.transferLength -= 512;	// 24 bytes
			}
		}
	}
	else if (MODE_SENSE == cmd)
	{
		pgm = P_ModeSense;
		blen = 4;
	}
	else
	{
		pgm = P_ReadCapacity-3;
		blen = 8;
		if (READ_CAPACITY != cmd)
			status = 1;	// Faking a data phase for unknown commands
	}

	if (pgm)
		Transfer(MSC_TX | TRANSFER_PGM | TRANSFER_RELEASE,pgm,blen);

	cbw.flags = status;
	cbw.transferLength -= blen;
	cbw.signature[3] = 'S';
	Transfer(MSC_TX | TRANSFER_RELEASE,(u8*)&cbw,13);
}

CBW cbw;	// 38 byte savings by making this global
void SCSITask()
{
	Recv(MSC_RX,(u8*)&cbw,sizeof(CBW));
	SCSI(cbw);
}

#endif