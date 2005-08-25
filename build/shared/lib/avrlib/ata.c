/*! \file ata.c \brief IDE-ATA hard disk interface driver. */
//*****************************************************************************
//
// File Name	: 'ata.c'
// Title		: IDE-ATA interface driver for hard disks
// Author		: Pascal Stang
// Date			: 11/22/2000
// Revised		: 4/19/2003
// Version		: 0.3
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef WIN32
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/signal.h>
	#include <avr/pgmspace.h>
//	#include <stdio.h>
#endif
#include "global.h"
#include "timer.h"
#include "rprintf.h"

#include "ata.h"

//#define DEBUG_ATA	1

// global variables

// drive information
typeDriveInfo ataDriveInfo;


void ataInit(void)
{

}

void ataDriveInit(void)
{
	u08 i;
	unsigned char* buffer = (unsigned char*) SECTOR_BUFFER_ADDR;

	// read drive identity
	rprintfProgStrM("\r\nScanning IDE interface...\r\n");
	// Wait for drive to be ready
	ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);
	// issue identify command
	ataWriteByte(ATA_REG_CMDSTATUS1, 0xEC);
	// wait for drive to request data transfer
	ataStatusWait(ATA_SR_DRQ, ATA_SR_DRQ);
	timerPause(200);
	// read in the data
	ataReadDataBuffer(buffer, 512);

	// set local drive info parameters
	ataDriveInfo.cylinders =		*( ((unsigned int*) buffer) + ATA_IDENT_CYLINDERS );
	ataDriveInfo.heads =			*( ((unsigned int*) buffer) + ATA_IDENT_HEADS );
	ataDriveInfo.sectors =			*( ((unsigned int*) buffer) + ATA_IDENT_SECTORS );
	ataDriveInfo.LBAsupport =		*( ((unsigned int*) buffer) + ATA_IDENT_FIELDVALID );
	ataDriveInfo.sizeinsectors =	*( (unsigned long*) (buffer + ATA_IDENT_LBASECTORS*2) );
	// copy model string
	for(i=0; i<40; i+=2)
	{
		// correct for byte order
		ataDriveInfo.model[i  ] = buffer[(ATA_IDENT_MODEL*2) + i + 1];
		ataDriveInfo.model[i+1] = buffer[(ATA_IDENT_MODEL*2) + i    ];
	}
	// terminate string
	ataDriveInfo.model[40] = 0;

	// process and print info
	if(ataDriveInfo.LBAsupport)
	{
		// LBA support
		rprintf("Drive 0: %dMB ", ataDriveInfo.sizeinsectors/(1000000/512) );
		rprintf("LBA mode -- MODEL: ");
	}
	else
	{
		// CHS, no LBA support
		// calculate drive size
		ataDriveInfo.sizeinsectors = (unsigned long) ataDriveInfo.cylinders*
												ataDriveInfo.heads*ataDriveInfo.sectors;
		rprintf("Drive 0: %dMB ", ataDriveInfo.sizeinsectors/(1000000/512) );
		rprintf("CHS mode C=%d H=%d S=%d -- MODEL: ", ataDriveInfo.cylinders, ataDriveInfo.heads, ataDriveInfo.sectors );
	}
	// print model information	
	rprintfStr(ataDriveInfo.model); rprintfCRLF();

	// initialize local disk parameters
	//ataDriveInfo.cylinders = ATA_DISKPARM_CLYS;
	//ataDriveInfo.heads = ATA_DISKPARM_HEADS;
	//ataDriveInfo.sectors = ATA_DISKPARM_SECTORS;

}

void ataDiskErr(void)
{
	unsigned char b;

	b = ataReadByte(ATA_REG_ERROR);	
	rprintfProgStrM("ATA Error: "); 
	rprintfu08(b); 
	rprintfCRLF();
}

void ataSetDrivePowerMode(u08 DriveNo, u08 mode, u08 timeout)
{
	// select drive
	ataDriveSelect(DriveNo);
	// Wait for drive to be ready
	ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

	// set mode
	switch(mode)
	{
	case ATA_DISKMODE_SPINDOWN:		ataWriteByte(ATA_REG_CMDSTATUS1, ATA_CMD_SPINDOWN); break;
	case ATA_DISKMODE_SPINUP:		ataWriteByte(ATA_REG_CMDSTATUS1, ATA_CMD_SPINUP); break;
	case ATA_DISKMODE_SETTIMEOUT:
		ataWriteByte(ATA_REG_SECCOUNT, timeout);
		ataWriteByte(ATA_REG_CMDSTATUS1, ATA_CMD_IDLE_5SU);
		break;
	case ATA_DISKMODE_SLEEP:		ataWriteByte(ATA_REG_CMDSTATUS1, ATA_CMD_SLEEP); break;
	default:
		break;
	}
}

void ataPrintSector( u08 *Buffer)
{
	u08 i;
	u16 j;
	u08 *buf;
	u08 s;

	buf = Buffer;
	
	// print the low order address indicies
	rprintfProgStrM("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  0123456789ABCDEF\r\n");
	rprintfProgStrM("     -----------------------------------------------  ---- ASCII -----\r\n");
	
	// print the data
	for(j=0; j<0x20; j++)
	{
		// print the high order address index for this line
		rprintfu16(j<<4);
		rprintfProgStrM(" ");

		// print the hex data
		for(i=0; i<0x10; i++)
		{
			rprintfu08(buf[(j<<4)+i]);
			rprintfProgStrM(" ");
		}
		
		// leave some space
		rprintfProgStrM(" ");

		// print the ascii data
		for(i=0; i<0x10; i++)
		{
			s = buf[(j<<4)+i]; 
			// make sure character is printable
			if(s >= 0x20)
			{
				rprintfChar(s);
			}
			else
			{
				rprintfChar(0x20);
			}

		}
		rprintfCRLF();
	}
}

void ataReadDataBuffer(u08 *Buffer, u16 numBytes)
{
	unsigned int i;

	//sbi(MCUCR, SRW);			// enable RAM waitstate

	// read data from drive
	for (i=0; i<(numBytes/16); i++)
	{
		// optimize by reading 16 bytes in-line before looping
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL);
		*Buffer++ = *((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH);
	}
	//cbi(MCUCR, SRW);			// disable RAM waitstate
	
}

void ataWriteDataBuffer(u08 *Buffer, u16 numBytes)
{
	register unsigned char temp;
	unsigned int i;

	//sbi(MCUCR, SRW);			// enable RAM waitstate

	// write data to drive
	for (i=0; i<(numBytes/16); i++) 	
	{
		// optimize by writing 16 bytes in-line before looping
		// keep byte order correct by using temp register
		temp = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH) = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL) = temp;
		temp = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH) = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL) = temp;
		temp = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH) = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL) = temp;
		temp = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH) = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL) = temp;
		temp = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH) = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL) = temp;
		temp = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH) = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL) = temp;
		temp = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH) = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL) = temp;
		temp = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAH) = *Buffer++;
		*((volatile unsigned char*) ATA_REG_BASE + ATA_REG_DATAL) = temp;
	}
	//cbi(MCUCR, SRW);			// disable RAM waitstate

}

u08 ataStatusWait(u08 mask, u08 waitStatus)
{
	register u08 status;

	delay(100);

	// wait for desired status
	while( ((status = ataReadByte(ATA_REG_CMDSTATUS1)) & mask) == waitStatus );

	return status;
}


unsigned char ataReadSectorsCHS(	unsigned char Drive, 
											unsigned char Head, 
											unsigned int Track,
											unsigned char Sector,
											unsigned int numsectors,
											unsigned char *Buffer)
{
  	unsigned char temp;

	// Wait for drive to be ready
	temp = ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

  	// Prepare parameters...
	ataWriteByte(ATA_REG_HDDEVSEL, 0xA0+(Drive ? 0x10:00)+Head); // CHS mode/Drive/Head
	ataWriteByte(ATA_REG_CYLHI, Track>>8);  		// MSB of track
	ataWriteByte(ATA_REG_CYLLO, Track);     		// LSB of track
  	ataWriteByte(ATA_REG_STARTSEC, Sector);    	// sector
	ataWriteByte(ATA_REG_SECCOUNT, numsectors);	// # of sectors

	// Issue read sector command...
  	ataWriteByte(ATA_REG_CMDSTATUS1, 0x21);

	// Wait for drive to be ready
	temp = ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

	if (temp & ATA_SR_ERR)
	{
		rprintfProgStrM("RD ERR\r\n");
		return 1;
	}

	// Wait for drive to request data transfer
	ataStatusWait(ATA_SR_DRQ, 0);

	// read data from drive
	ataReadDataBuffer(Buffer, 512*numsectors);

	// Return the error bit from the status register...
	temp = ataReadByte(ATA_REG_CMDSTATUS1);	// read status register

	return (temp & ATA_SR_ERR) ? 1:0;
}


unsigned char ataWriteSectorsCHS(unsigned char Drive, 
											unsigned char Head, 
											unsigned int Track,
											unsigned char Sector,
											unsigned int numsectors,
											unsigned char *Buffer)
{
  	unsigned char temp;

	// Wait for drive to be ready
	temp = ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

  	// Prepare parameters...
	ataWriteByte(ATA_REG_HDDEVSEL, 0xA0+(Drive ? 0x10:00)+Head); // CHS mode/Drive/Head
	ataWriteByte(ATA_REG_CYLHI, Track>>8);  		// MSB of track
	ataWriteByte(ATA_REG_CYLLO, Track);     		// LSB of track
  	ataWriteByte(ATA_REG_STARTSEC, Sector);    	// sector
	ataWriteByte(ATA_REG_SECCOUNT, numsectors);	// # of sectors

	// Issue write sector command
  	ataWriteByte(ATA_REG_CMDSTATUS1, 0x31);

	//delay(100);

	// Wait for drive to request data transfer
	ataStatusWait(ATA_SR_DRQ, 0);

	// write data to drive
	ataWriteDataBuffer(Buffer, 512*numsectors);
	
	// Wait for drive to finish write
	temp = ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

	// check for errors
	if (temp & ATA_SR_ERR)
	{
		rprintfProgStrM("WR ERR\r\n");
		return 1;
	}

	// Return the error bit from the status register...
	return (temp & ATA_SR_ERR) ? 1:0;
}

unsigned char ataReadSectorsLBA(	unsigned char Drive, 
											unsigned long lba,
											unsigned int numsectors,
                            		unsigned char *Buffer)
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;

#ifdef DEBUG_ATA
	rprintfProgStrM("ATA LBA read ");
	rprintfu32(lba); rprintfProgStrM(" ");
	rprintfu16(numsectors); rprintfProgStrM(" ");
	rprintfu16((unsigned int)Buffer); 
	rprintfCRLF();
#endif

	sect = (int) ( lba & 0x000000ffL );
	lba = lba >> 8;
	cyl = (int) ( lba & 0x0000ffff );
	lba = lba >> 16;
	head = ( (int) ( lba & 0x0fL ) ) | ATA_HEAD_USE_LBA;

	temp = ataReadSectorsCHS( Drive, head, cyl, sect, numsectors, Buffer );

	if(temp)
		ataDiskErr();
	return temp;
}

unsigned char ataWriteSectorsLBA(	unsigned char Drive, 
												unsigned long lba,
												unsigned int numsectors,
                            			unsigned char *Buffer)
{
	unsigned int cyl, head, sect;
	unsigned char temp;

#ifdef DEBUG_ATA
	rprintfProgStrM("ATA LBA write ");
	rprintfu32(lba); rprintfProgStrM(" ");
	rprintfu16(numsectors); rprintfProgStrM(" ");
	rprintfu16((unsigned int)Buffer); 
	rprintfCRLF();
#endif

	sect = (int) ( lba & 0x000000ffL );
	lba = lba >> 8;
	cyl = (int) ( lba & 0x0000ffff );
	lba = lba >> 16;
	head = ( (int) ( lba & 0x0fL ) ) | ATA_HEAD_USE_LBA;

	temp = ataWriteSectorsCHS( Drive, head, cyl, sect, numsectors, Buffer );

	if(temp)
		ataDiskErr();
	return temp;
}                            		


unsigned char ataReadSectors(	unsigned char Drive, 
										unsigned long lba,
										unsigned int numsectors,
                            	unsigned char *Buffer)
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;

	// check if drive supports native LBA mode
	if(ataDriveInfo.LBAsupport)
	{
		// drive supports using native LBA
		temp = ataReadSectorsLBA(Drive, lba, numsectors, Buffer);
	}
	else
	{
		// drive required CHS access
		#ifdef DEBUG_ATA
			// do this defore destroying lba
			rprintfProgStrM("ATA LBA for CHS read: ");
			rprintfProgStrM("LBA="); rprintfu32(lba); rprintfProgStrM(" ");
		#endif

		// convert LBA to pseudo CHS
		// remember to offset the sector count by one
		sect = (u08) (lba % ataDriveInfo.sectors)+1;
		lba = lba / ataDriveInfo.sectors;
		head = (u08) (lba % ataDriveInfo.heads);
		lba = lba / ataDriveInfo.heads;
		cyl = (u16) lba;

		#ifdef DEBUG_ATA
			rprintfProgStrM("C:H:S=");
			rprintfu16(cyl); rprintfProgStrM(":");
			rprintfu08(head); rprintfProgStrM(":");
			rprintfu08(sect); rprintfCRLF();
		#endif

		temp = ataReadSectorsCHS( Drive, head, cyl, sect, numsectors, Buffer );
	}

	if(temp)
		ataDiskErr();
	return temp;
}


unsigned char ataWriteSectors(unsigned char Drive, 
										unsigned long lba,
										unsigned int numsectors,
                            	unsigned char *Buffer)
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;

	// check if drive supports native LBA mode
	if(ataDriveInfo.LBAsupport)
	{
		// drive supports using native LBA
		temp = ataWriteSectorsLBA(Drive, lba, numsectors, Buffer);
	}
	else
	{
		// drive required CHS access
		#ifdef DEBUG_ATA
			// do this defore destroying lba
			rprintfProgStrM("ATA LBA for CHS write: ");
			rprintfProgStrM("LBA="); rprintfu32(lba); rprintfProgStrM(" ");
		#endif

		// convert LBA to pseudo CHS
		// remember to offset the sector count by one
		sect = (u08) (lba % ataDriveInfo.sectors)+1;
		lba = lba / ataDriveInfo.sectors;
		head = (u08) (lba % ataDriveInfo.heads);
		lba = lba / ataDriveInfo.heads;
		cyl = (u16) lba;

		#ifdef DEBUG_ATA
			rprintfProgStrM("C:H:S=");
			rprintfu16(cyl); rprintfProgStrM(":");
			rprintfu08(head); rprintfProgStrM(":");
			rprintfu08(sect); rprintfCRLF();
		#endif

		temp = ataWriteSectorsCHS( Drive, head, cyl, sect, numsectors, Buffer );
	}

	if(temp)
		ataDiskErr();
	return temp;
}                            		

void ataDriveSelect(u08 DriveNo)
{
	ataWriteByte(ATA_REG_HDDEVSEL, 0xA0+(DriveNo ? 0x10:00)); // Drive selection
}
 
//----------------------------------------------------------------------------
// Set drive mode (STANDBY, IDLE)
//----------------------------------------------------------------------------
/*#define STANDBY 0
#define IDLE    1
#define SLEEP   2 
*/ 

/*
unsigned char SetMode(unsigned char DriveNo, unsigned char Mode, unsigned char PwrDown) 
{
  WriteBYTE(CMD, 6, 0xA0 + (DriveNo ? 0x10:0x00)); // Select drive
  WriteBYTE(CMD, 2, (PwrDown ? 0x01:0x00)); // Enable automatic power down
  switch (Mode) 
  {
    case STANDBY: WriteBYTE(CMD,7, 0xE2); break;
    case IDLE:    WriteBYTE(CMD,7, 0xE3); break;
    // NOTE: To recover from sleep, either issue a soft or hardware reset !
    // (But not on all drives, f.ex seagate ST3655A it's not nessecary to reset
    // but only to go in Idle mode, But on a Conner CFA170A it's nessecary with
    // a reset)
    case SLEEP:   WriteBYTE(CMD,7, 0xE6); break;
  }
  Timer10mSec=10000;
  while ((ReadBYTE(CMD,7) & 0xC0)!=0x40 && Timer10mSec); // Wait for DRDY & NOT BUSY 
  if (Timer10mSec==0) return 0xFF;                       //   or timeout
 
  // Return the error register...
  return ReadBYTE(CMD, 1);
}

*/

u08 ataReadByte(u08 reg)
{
	register u08 ret;
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	ret = *((volatile unsigned char*) ATA_REG_BASE + reg);
	//cbi(MCUCR, SRW);			// disable RAM waitstate
	return ret;
}

void ataWriteByte(u08 reg, u08 data)
{
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	*((volatile unsigned char*) ATA_REG_BASE + reg) = data;
	//cbi(MCUCR, SRW);			// disable RAM waitstate
}

 
void ataShowRegisters(unsigned char DriveNo) 
{ 
	ataWriteByte(ATA_REG_HDDEVSEL, 0xA0 + (DriveNo ? 0x10:0x00)); // Select drive
	
	rprintfProgStrM("R0: DATALOW  = 0x");	rprintfu08(ataReadByte(ATA_REG_DATAL	));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R1: ERROR    = 0x");	rprintfu08(ataReadByte(ATA_REG_ERROR	));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R2: SECT CNT = 0x");	rprintfu08(ataReadByte(ATA_REG_SECCOUNT));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R3: SECT NUM = 0x");	rprintfu08(ataReadByte(ATA_REG_STARTSEC));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R4: CYL LOW  = 0x");	rprintfu08(ataReadByte(ATA_REG_CYLLO	));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R5: CYL HIGH = 0x");	rprintfu08(ataReadByte(ATA_REG_CYLHI	));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R6: HEAD/DEV = 0x");	rprintfu08(ataReadByte(ATA_REG_HDDEVSEL));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R7: CMD/STA  = 0x");	rprintfu08(ataReadByte(ATA_REG_CMDSTATUS1));	rprintfProgStrM("\r\n");
} 

unsigned char ataSWReset(void)
{
	ataWriteByte(ATA_REG_HDDEVSEL, 0x06);	// SRST and nIEN bits
	delay(10);	// 10uS delay
	ataWriteByte(ATA_REG_HDDEVSEL, 0x02);	// nIEN bits
	delay(10);	// 10 uS delay
   
   while( (ataReadByte(ATA_REG_CMDSTATUS1) & 0xC0) != 0x40 ); // Wait for DRDY and not BSY
    
  	return ataReadByte(ATA_REG_CMDSTATUS1) + ataReadByte(ATA_REG_ERROR);
}

/*
unsigned char ATA_Idle(unsigned char Drive)
{

  WriteBYTE(CMD, 6, 0xA0 + (Drive ? 0x10:0x00)); // Select drive
  WriteBYTE(CMD,7, 0xE1);

  while ((ReadBYTE(CMD,7) & 0xC0)!=0x40); // Wait for DRDY & NOT BUSY 

  // Return the error register...
  return ReadBYTE(CMD, 1);
}
*/
