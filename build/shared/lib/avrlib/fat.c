/*! \file fat.c \brief FAT16/32 file system driver. */
//*****************************************************************************
//
// File Name	: 'fat.c'
// Title		: FAT16/32 file system driver
// Author		: Pascal Stang
// Date			: 11/07/2000
// Revised		: 12/12/2000
// Version		: 0.3
// Target MCU	: ATmega103 (should work for Atmel AVR Series)
// Editor Tabs	: 4
//
// This code is based in part on work done by Jesper Hansen for his
//		YAMPP MP3 player project.
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "ata.h"
#include "rprintf.h"

#include "fat.h"
#include "fatconf.h"

// globals
unsigned char *SectorBuffer  =		(unsigned char *) SECTOR_BUFFER1_ADDR;
unsigned char *LongNameBuffer =		(unsigned char *) LONGNAME_BUFFER_ADDR;
unsigned char *DirNameBuffer =		(unsigned char *) DIRNAME_BUFFER_ADDR;

struct partrecord PartInfo;
unsigned char Fat32Enabled;
unsigned long FirstDataSector;
unsigned int  BytesPerSector;
unsigned int  SectorsPerCluster;
unsigned long FirstFATSector;
unsigned long FirstDirSector;
unsigned long FileSize;
unsigned long FatInCache = 0;

/*************************************************************************/
/*************************************************************************/


unsigned long fatClustToSect(unsigned long clust)
{
	return ((clust-2) * SectorsPerCluster) + FirstDataSector;
}

unsigned int fatClusterSize(void)
{
	// return the number of sectors in a disk cluster
	return SectorsPerCluster;
}

unsigned char fatInit( unsigned char device)
{
	//struct partrecord *pr;
	struct bpb710 *bpb;

	// read partition table
	// TODO.... error checking
	ataReadSectors(DRIVE0, 0, 1, SectorBuffer);
	// map first partition record	
	// save partition information to global PartInfo
	PartInfo = *((struct partrecord *) ((struct partsector *) SectorBuffer)->psPart);
//	PartInfo = *pr;
	
	// Read the Partition BootSector
	// **first sector of partition in PartInfo.prStartLBA
	ataReadSectors( DRIVE0, PartInfo.prStartLBA, 1, SectorBuffer );
	bpb = (struct bpb710 *) ((struct bootsector710 *) SectorBuffer)->bsBPB;

	// setup global disk constants
	FirstDataSector	= PartInfo.prStartLBA;
	if(bpb->bpbFATsecs)
	{
		// bpbFATsecs is non-zero and is therefore valid
		FirstDataSector	+= bpb->bpbResSectors + bpb->bpbFATs * bpb->bpbFATsecs;
	}
	else
	{
		// bpbFATsecs is zero, real value is in bpbBigFATsecs
		FirstDataSector	+= bpb->bpbResSectors + bpb->bpbFATs * bpb->bpbBigFATsecs;
	}
	SectorsPerCluster	= bpb->bpbSecPerClust;
	BytesPerSector		= bpb->bpbBytesPerSec;
	FirstFATSector		= bpb->bpbResSectors + PartInfo.prStartLBA;

	switch (PartInfo.prPartType)
	{
		case PART_TYPE_DOSFAT16:
		case PART_TYPE_FAT16:
		case PART_TYPE_FAT16LBA:
			// first directory cluster is 2 by default (clusters range 2->big)
			FirstDirSector	= CLUST_FIRST;
			// push data sector pointer to end of root directory area
			//FirstDataSector += (bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR;
			Fat32Enabled = FALSE;
			break;
		case PART_TYPE_FAT32LBA:
		case PART_TYPE_FAT32:
			// bpbRootClust field exists in FAT32 bpb710, but not in lesser bpb's
			FirstDirSector = bpb->bpbRootClust;
			// push data sector pointer to end of root directory area
			// need this? FirstDataSector += (bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR;
			Fat32Enabled = TRUE;
			break;
		default:
			rprintfProgStrM("Found: No Partition!\r\n");
			//return 1;
			break;
	}


#ifdef DEBUG_FAT
	switch (PartInfo.prPartType)
	{
		case PART_TYPE_DOSFAT16:
				rprintfProgStrM("Found: DOSFAT 16\r\n");
				break;
		case PART_TYPE_FAT16:
				rprintfProgStrM("Found: FAT16\r\n");
				break;
		case PART_TYPE_FAT16LBA:
				rprintfProgStrM("Found: FAT16 LBA\r\n");
				break;
		case PART_TYPE_FAT32LBA:
				rprintfProgStrM("Found: FAT32 LBA\r\n");
				break;
		case PART_TYPE_FAT32:
				rprintfProgStrM("Found: FAT32\r\n");
				//return 1;	
				break;
		default:
				rprintfProgStrM("Found: No Partition!\r\n");
				//return 1;
				break;
	}

	rprintfProgStrM("First sector    : ");	rprintfu32(PartInfo.prStartLBA);	rprintfCRLF();
	rprintfProgStrM("Size            : ");	rprintfu32(PartInfo.prSize);		rprintfCRLF();
	rprintfProgStrM("bytes/sector    : ");	rprintfu16(bpb->bpbBytesPerSec);	rprintfCRLF();
	rprintfProgStrM("sectors/cluster : ");	rprintfu08(bpb->bpbSecPerClust);	rprintfCRLF();
	rprintfProgStrM("reserved sectors: ");	rprintfu16(bpb->bpbResSectors);		rprintfCRLF();
	rprintfProgStrM("FatSectors      : ");	rprintfu16(bpb->bpbFATsecs);		rprintfCRLF();
	rprintfProgStrM("BigFatSectors   : ");	rprintfu32(bpb->bpbBigFATsecs);		rprintfCRLF();
	rprintfProgStrM("Number of Fats  : ");	rprintfu08(bpb->bpbFATs);			rprintfCRLF();
	rprintfProgStrM("First Fat Sector: ");	rprintfu32(FirstFATSector);			rprintfCRLF();
	rprintfProgStrM("First Data Sect : ");	rprintfu32(FirstDataSector);		rprintfCRLF();
	rprintfProgStrM("First Dir Clust : ");	rprintfu32(FirstDirSector);			rprintfCRLF();
#endif

	return 0;	
}

//////////////////////////////////////////////////////////////


unsigned int baseentry = 0;
unsigned int entrycount = 0;


unsigned long fatGetDirEntry(unsigned int entry, unsigned int count)
{
	unsigned long sector;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	struct winentry *we;
	unsigned int hasBuffer;
	unsigned int b;
	int i,index;
	char *p;
	
	if(count == 0)
	{
		entrycount = 0;
		DirNameBuffer = 0;
	}
	
	// read dir data
	sector = fatClustToSect(FirstDirSector);

	hasBuffer = 0;

	index = 16;	// crank it up
	do 
	{
		if(index == 16)	// time for next sector ?
		{
			ataReadSectors( DRIVE0, sector++, 1, SectorBuffer);
			de = (struct direntry *) SectorBuffer;
			index = 0;
		}	
	
		if(*de->deName != 0xE5)
		{
			// if not a deleted entry
			if(de->deAttributes == ATTR_LONG_FILENAME)
			{
				// we have a long name entry
				we = (struct winentry *) de;
				b = 13 *( (we->weCnt-1) & 0x0f);				// index into string
				p = &LongNameBuffer[b];
				for (i=0;i<5;i++)	*p++ = we->wePart1[i*2];	// copy first part			
				for (i=0;i<6;i++)	*p++ = we->wePart2[i*2];	// second part
				for (i=0;i<2;i++)	*p++ = we->wePart3[i*2];	// and third part
				if (we->weCnt & 0x40) *p = 0;					// in case dirnamelength is multiple of 13
				if ((we->weCnt & 0x0f) == 1) hasBuffer = 1;		// mark that we have a long entry
			}
			else
			{
				// we have a short name entry
				// check if this is the end of a multi-part long name entry
				if(hasBuffer)
				{
					// a long entry name has been collected
					// is it a directory ?
					if(de->deAttributes == ATTR_DIRECTORY)
					{
						unsigned long save = FirstDirSector;
						unsigned int save2 = baseentry;
						unsigned long rval;
						
						strcpy(DirNameBuffer,LongNameBuffer);
						strcat(DirNameBuffer,"/");

//						rprintfStr(LongNameBuffer); rprintfProgStrM("/"); //EOL();

						// call recursively
						FirstDirSector = ((unsigned long)de->deHighClust << 16) + de->deStartCluster;
						rval = fatGetDirEntry(entry,1);
						FirstDirSector = save;
						baseentry = save2;
						if (rval)
							return rval;
						else	
						{
							// reload original sector
							ataReadSectors( DRIVE0,	sector-1, 1, SectorBuffer);
							entrycount--;			// decrement entry counter		
							*DirNameBuffer = 0;
                 	}
					}
					else // normal file entry
						if(entrycount == entry)		
							break;
					hasBuffer = 0;	// clear buffer	
					entrycount++;	// increment entry counter		
				}
				// else ignore short_name_only entries
			}
		}
		de++;
		index++;
	}	while (*de->deName || index == 16);	// 0 in de->deName[0] if no more entries

	if (hasBuffer == 0)		// end of entries
		return 0;
	
	FileSize = de->deFileSize;
	return (unsigned long) ((unsigned long)de->deHighClust << 16) + de->deStartCluster;
}


// return the size of the last directory entry
unsigned long fatGetFilesize(void)
{
	return FileSize;
}


// return the long name of the last directory entry
char* fatGetFilename(void)
{	
	return LongNameBuffer;
}


// return the directory of the last directory entry
char* fatGetDirname(void)
{	
	return DirNameBuffer;
}


// load a clusterfull of data
void fatLoadCluster(unsigned long cluster, unsigned char *buffer)
{
	register unsigned char i;
	// read cluster
	//while ( ataReadSectors( DRIVE0, clust2sect(cluster), SectorsPerCluster, buffer) != 0);
	for(i=0; i<SectorsPerCluster; i++)
	{
//		ataReadSectors( DRIVE0, clust2sect(cluster)+i, 1, buffer+(i<<9) );
		// temporary fix for wierd misaligned cluster problem
		// (only when using FAT16?)
		ataReadSectors( DRIVE0, fatClustToSect(cluster+8)+i, 1, buffer+(i<<9) );
	}
}


// find next cluster in the FAT chain
unsigned long fatNextCluster(unsigned long cluster)
{
	unsigned long nextCluster;
	unsigned long fatMask;
	unsigned long fatOffset;
	unsigned long sector;
	unsigned int offset;
	
	// get fat offset in bytes
	if(Fat32Enabled)
	{
		// four FAT bytes (32 bits) for every cluster
		fatOffset = cluster << 2;
		// set the FAT bit mask
		fatMask = FAT32_MASK;
	}
	else
	{
		// two FAT bytes (16 bits) for every cluster
		fatOffset = cluster << 1;
		// set the FAT bit mask
		fatMask = FAT16_MASK;
	}
	
	// calculate the FAT sector that we're interested in
	sector = FirstFATSector + (fatOffset / BytesPerSector);
	// calculate offset of the our entry within that FAT sector
	offset = fatOffset % BytesPerSector;

	// if we don't already have this FAT chunk loaded, go get it
	if (sector != FatInCache)
	{
		// read sector of FAT table
		while (ataReadSectors( DRIVE0, sector, 1, (unsigned char*)FAT_CACHE_ADDR) != 0);
		FatInCache = sector;
	}

	// read the nextCluster value
	nextCluster = (*((unsigned long*) &((char*)FAT_CACHE_ADDR)[offset])) & fatMask;

	// check to see if we're at the end of the chain
	if (nextCluster == (CLUST_EOFE & fatMask))
		nextCluster = 0;

#ifdef DEBUG_FAT
	rprintfProgStrM(">");
	rprintfu32(nextCluster);
	rprintfCRLF();
#endif
	
	return nextCluster;
}
