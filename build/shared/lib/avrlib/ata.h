/*! \file ata.h \brief IDE-ATA hard disk interface driver. */
//*****************************************************************************
//
// File Name	: 'ata.h'
// Title		: IDE-ATA interface driver for hard disks
// Author		: Pascal Stang
// Date			: 11/22/2000
// Revised		: 12/29/2000
// Version		: 0.3
// Target MCU	: ATmega103 (should work for Atmel AVR Series)
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


#ifndef ATA_H
#define ATA_H

#include "global.h"
#include "ataconf.h"

// constants
#define DRIVE0		0

#define STANDBY		0
#define SLEEP		1
#define IDLE		2

// ATA status register bits
#define ATA_SR_BSY		0x80
#define ATA_SR_DRDY		0x40
#define ATA_SR_DF		0x20
#define ATA_SR_DSC		0x10
#define ATA_SR_DRQ		0x08
#define ATA_SR_CORR		0x04
#define ATA_SR_IDX		0x02
#define ATA_SR_ERR		0x01

// ATA error register bits
#define ATA_ER_UNC		0x40
#define ATA_ER_MC		0x20
#define ATA_ER_IDNF		0x10
#define ATA_ER_MCR		0x08
#define ATA_ER_ABRT		0x04
#define ATA_ER_TK0NF	0x02
#define ATA_ER_AMNF		0x01

// ATA head register bits
#define ATA_HEAD_USE_LBA	0x40
/*
// ATA registers
#define ATA_REG_BASE		0x8000
#define ATA_REG_DATAL		0x00
#define ATA_REG_ERROR		0x01
#define ATA_REG_SECCOUNT	0x02
#define ATA_REG_STARTSEC	0x03
#define ATA_REG_CYLLO		0x04
#define ATA_REG_CYLHI		0x05
#define ATA_REG_HDDEVSEL	0x06
#define ATA_REG_CMDSTATUS1	0x07
#define ATA_REG_CMDSTATUS2	0x08
#define ATA_REG_ACTSTATUS	0x09

#define ATA_REG_DATAH		0x10
*/
// ATA commands
#define ATA_CMD_READ			0x20
#define ATA_CMD_READNR			0x21
#define ATA_CMD_WRITE			0x30
#define ATA_CMD_WRITENR			0x31
#define ATA_CMD_IDENTIFY		0xEC
#define ATA_CMD_RECALIBRATE		0x10
#define ATA_CMD_SPINDOWN		0xE0	// spin down disk immediately
#define ATA_CMD_SPINUP			0xE1	// spin up disk immediately
#define ATA_CMD_STANDBY_5SU		0xE2	// spin down disk and set auto-power-down timer (sectorcount*5sec)
#define ATA_CMD_IDLE_5SU		0xE3	// keep disk spinning and set auto-power-down timer (sectorcount*5sec)
#define ATA_CMD_SLEEP			0xE6	// sleep disk (wakeup only on HW or SW reset)
#define ATA_CMD_STANDBY_01SU	0xF2	// spin down disk and set auto-power-down timer (sectorcount*0.1sec)
#define ATA_CMD_IDLE_01SU		0xF3	// keep disk spinning and set auto-power-down timer (sectorcount*0.1sec)


// ATA CHS disk parameters (examples, now we autodetect)
#define ATA_DISKPARM_CLYS		0x03A6	// number of cylinders per platter
#define ATA_DISKPARM_HEADS		0x10	// number of heads (usable plater sides)
#define ATA_DISKPARM_SECTORS	0x11	// number of sectors per head per cylinder

// ATA Identity fields
// all offsets refer to word offset (2 byte increments)
#define ATA_IDENT_DEVICETYPE	0		// specifies ATA/ATAPI, removable/non-removable
#define ATA_IDENT_CYLINDERS		1		// number of logical cylinders
#define ATA_IDENT_HEADS			3		// number of logical heads
#define ATA_IDENT_SECTORS		6		// number of sectors per track
#define ATA_IDENT_SERIAL		10		// drive model name (20 characters)
#define ATA_IDENT_MODEL			27		// drive model name (40 characters)
#define ATA_IDENT_FIELDVALID	53		// indicates field validity of higher words (bit0: words54-58, bit1: words 64-70)
#define ATA_IDENT_LBASECTORS	60		// number of sectors in LBA translation mode

// drive mode defines (for ataSetDrivePowerMode() )
#define ATA_DISKMODE_SPINDOWN	0
#define ATA_DISKMODE_SPINUP		1
#define ATA_DISKMODE_SETTIMEOUT	2
#define ATA_DISKMODE_SLEEP		3

// typedefs
// drive info structure
typedef struct 
{
	unsigned int  cylinders;
	unsigned char heads;
	unsigned char sectors;
	unsigned long sizeinsectors;
	unsigned char LBAsupport;
	char model[41];
} typeDriveInfo;


// Prototypes
void ataInit(void);
void ataDriveInit(void);
void ataDriveSelect(u08 DriveNo);
void ataSetDrivePowerMode(u08 DriveNo, u08 mode, u08 timeout);
u08  ataReadByte(u08 reg);
void ataWriteByte(u08 reg, u08 data);
void ataShowRegisters(unsigned char DriveNo);
u08  ataSWReset(void);
void ataDiskErr(void);
void ataPrintSector( u08 *Buffer);
void ataReadDataBuffer(u08 *Buffer, u16 numBytes);
void ataWriteDataBuffer(u08 *Buffer, u16 numBytes);
u08 ataStatusWait(u08 mask, u08 waitStatus);

// read and write routines for CHS based drives
unsigned char ataReadSectorsCHS(	unsigned char Drive, 
									unsigned char Head, 
									unsigned int Track,
									unsigned char Sector,
									unsigned int numsectors,
									unsigned char *Buffer);

unsigned char ataWriteSectorsCHS(	unsigned char Drive, 
									unsigned char Head, 
									unsigned int Track,
									unsigned char Sector,
									unsigned int numsectors,
									unsigned char *Buffer);

// read and write routines for LBA based drives
unsigned char ataReadSectorsLBA(	unsigned char Drive, 
									unsigned long lba,
									unsigned int numsectors,
                            		unsigned char *Buffer);

unsigned char ataWriteSectorsLBA(	unsigned char Drive, 
									unsigned long lba,
									unsigned int numsectors,
                            		unsigned char *Buffer);

// generic read and write routines using LBA
//   uses native or translated LBA addressing
//   given autodetected drive type
unsigned char ataReadSectors(	unsigned char Drive, 
								unsigned long lba,
								unsigned int numsectors,
                            	unsigned char *Buffer);

unsigned char ataWriteSectors(	unsigned char Drive, 
								unsigned long lba,
								unsigned int numsectors,
                            	unsigned char *Buffer);

//unsigned char IdentifyDrive(unsigned char DriveNo,  unsigned char *Buffer, tdefDriveInfo *DriveInfo);
//unsigned char SetMode(unsigned char DriveNo, unsigned char Mode, unsigned char PwrDown);
//unsigned char ATA_Idle(unsigned char Drive);

#endif
