

/* Copyright (c) 2011, Peter Barrett  
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

extern const u8 Sector_0[] PROGMEM;
extern const u8 Sector_1[] PROGMEM;
extern const u8 Sector_2[] PROGMEM;
extern const u8 Sector_3[] PROGMEM;
extern const u8 Sector_Size[] PROGMEM;


const u8 Sector_0[27] =
{
	0xEB,0x3C,0x90,
	FAT_OEM_NAME,
	0x00,0x02,	// Bytes per sector 512
	0x40,		// Sectors per Cluster 32k per cluster
	0x01,0x00,	// Reserved sectors
	0x02,		// FATSs
	0x10,0x00,	// root entries
	4+64,0x00,	// number of sectors: 30k file
	0xF8,		// Media type fixed
	0x01,0x00,	// Sectors per FAT
	0x01,0x00,	// Sectors per head
	0x01,//0x00,	// Heads per cylinder
	//0x00,0x00,0x00,0x00,	// Hidden
	//0x00,0x00,0x00,0x00,	// long number of sectors
	//0x00,0x00,				// Ext flags
	// 38 bytes
};

//	FAT12 clusters for 32k file
const u8 Sector_1[] =
{
#if 0
	0xF8,0xFF,0xFF,
	0xFF,0x0F//,0x00
#endif
};

const u8 Sector_2[] =
{
};

const u8 Sector_3[] =
{
#if 0
	FAT_FILE_NAME, 0x00,0x00,0x00,0x00,0x00,	// Uppercase name please
	0x43,0x3E,0x43,0x3E,0x00,0x00,0xAB,0x8C,0x40,0x3E,0x02,0x00,0x00,0x80,//0x00,0x00
	// 0x00,0x00,
#endif

#ifdef FAT_DISK_LABEL
	 FAT_DISK_LABEL,0x28
#endif
};

#if 0
const u8 Sector_Size[] =
{
	0,
	sizeof(Sector_0),
	sizeof(Sector_0) + sizeof(Sector_1),
	sizeof(Sector_0) + sizeof(Sector_1) + sizeof(Sector_2),
	sizeof(Sector_0) + sizeof(Sector_1) + sizeof(Sector_2) + sizeof(Sector_3)
};
#endif

const u8* LBAToROM(u8 b)
{
	b -= 4;
	return (const u8*)(b << 9);
}

void SendSector(u8 b)
{
	if (b == 0)
	{
		u8 n = sizeof(Sector_0);
		Transfer(MSC_TX | TRANSFER_PGM,Sector_0,n);
		Transfer(MSC_TX | TRANSFER_ZERO,0,512-n);
		return;
	}
	
	if ((b < 4) || (b >= 64))
	{
		Transfer(MSC_TX | TRANSFER_ZERO,0,512);
		return;
	}
	
		//	Copy body of file from ROM
	Transfer(MSC_TX | TRANSFER_PGM,LBAToROM(b),512);
}

#define GENERATE_INVALID_ROM_ADDRESS 60

void RecvSector(u8 lba)
{
	if ((lba < 4) || (lba >= 64))
		lba = GENERATE_INVALID_ROM_ADDRESS;	// Only care about the first 30k

	//	Write sectors to flash
	u16 addr = (u16)LBAToROM(lba);		// may generate out of range addresses, Program will fix
	u8 i = 4;							// 4x128 is a sector
	while (i--)
	{
		Program(MSC_RX,addr,128);
		addr += 128;
	}
}

#endif