/*! \file fat.h \brief FAT16/32 file system driver. */
//*****************************************************************************
//
// File Name	: 'fat.h'
// Title		: FAT16/32 file system driver
// Author		: Pascal Stang
// Date			: 11/07/2000
// Revised		: 12/12/2000
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

#ifndef FAT_H
#define FAT_H

#include "global.h"


// Some useful cluster numbers
#define MSDOSFSROOT     0               // cluster 0 means the root dir
#define CLUST_FREE      0               // cluster 0 also means a free cluster
#define MSDOSFSFREE     CLUST_FREE
#define CLUST_FIRST     2               // first legal cluster number
#define CLUST_RSRVD     0xfffffff6      // reserved cluster range
#define CLUST_BAD       0xfffffff7      // a cluster with a defect
#define CLUST_EOFS      0xfffffff8      // start of eof cluster range
#define CLUST_EOFE      0xffffffff      // end of eof cluster range

#define FAT12_MASK      0x00000fff      // mask for 12 bit cluster numbers
#define FAT16_MASK      0x0000ffff      // mask for 16 bit cluster numbers
#define FAT32_MASK      0x0fffffff      // mask for FAT32 cluster numbers


// Partition Type used in the partition record
#define PART_TYPE_UNKNOWN		0x00
#define PART_TYPE_FAT12			0x01
#define PART_TYPE_XENIX			0x02
#define PART_TYPE_DOSFAT16		0x04
#define PART_TYPE_EXTDOS		0x05
#define PART_TYPE_FAT16			0x06
#define PART_TYPE_NTFS			0x07
#define PART_TYPE_FAT32			0x0B
#define PART_TYPE_FAT32LBA		0x0C
#define PART_TYPE_FAT16LBA		0x0E
#define PART_TYPE_EXTDOSLBA		0x0F
#define PART_TYPE_ONTRACK		0x33
#define PART_TYPE_NOVELL		0x40
#define PART_TYPE_PCIX			0x4B
#define PART_TYPE_PHOENIXSAVE	0xA0
#define PART_TYPE_CPM			0xDB
#define PART_TYPE_DBFS			0xE0
#define PART_TYPE_BBT			0xFF

struct partrecord // length 16 bytes
{			
	BYTE	prIsActive;					// 0x80 indicates active partition
	BYTE	prStartHead;				// starting head for partition
	WORD	prStartCylSect;				// starting cylinder and sector
	BYTE	prPartType;					// partition type (see above)
	BYTE	prEndHead;					// ending head for this partition
	WORD	prEndCylSect;				// ending cylinder and sector
	DWORD	prStartLBA;					// first LBA sector for this partition
	DWORD	prSize;						// size of this partition (bytes or sectors ?)
};

        
struct partsector
{
	CHAR	psPartCode[512-64-2];		// pad so struct is 512b
	BYTE	psPart[64];					// four partition records (64 bytes)
	BYTE	psBootSectSig0;				// two signature bytes (2 bytes)
	BYTE	psBootSectSig1;
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
};



// Format of a boot sector.  This is the first sector on a DOS floppy disk
// or the first sector of a partition on a hard disk.  But, it is not the
// first sector of a partitioned hard disk.
struct bootsector33 {
	BYTE 	bsJump[3];					// jump inst E9xxxx or EBxx90
	CHAR	bsOemName[8];				// OEM name and version
	CHAR	bsBPB[19];					// BIOS parameter block
	CHAR	bsDriveNumber;				// drive number (0x80)
	CHAR	bsBootCode[479];			// pad so struct is 512b
	BYTE	bsBootSectSig0;				// boot sector signature byte 0x55
	BYTE	bsBootSectSig1;				// boot sector signature byte 0xAA
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
};

struct extboot {
	CHAR	exDriveNumber;				// drive number (0x80)
	CHAR	exReserved1;				// reserved
	CHAR	exBootSignature;			// ext. boot signature (0x29)
#define EXBOOTSIG       0x29
	CHAR	exVolumeID[4];				// volume ID number
	CHAR	exVolumeLabel[11];			// volume label
	CHAR	exFileSysType[8];			// fs type (FAT12 or FAT16)
};

struct bootsector50 {
	BYTE	bsJump[3];					// jump inst E9xxxx or EBxx90
	CHAR	bsOemName[8];				// OEM name and version
	CHAR	bsBPB[25];					// BIOS parameter block
	CHAR	bsExt[26];					// Bootsector Extension
	CHAR	bsBootCode[448];			// pad so structure is 512b
	BYTE	bsBootSectSig0;				// boot sector signature byte 0x55 
	BYTE	bsBootSectSig1;				// boot sector signature byte 0xAA
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
};

struct bootsector710 {
	BYTE	bsJump[3];					// jump inst E9xxxx or EBxx90
	CHAR	bsOEMName[8];				// OEM name and version
	CHAR	bsBPB[53];					// BIOS parameter block
	CHAR	bsExt[26];					// Bootsector Extension
	CHAR	bsBootCode[418];			// pad so structure is 512b
	BYTE	bsBootSectSig2;				// 2 & 3 are only defined for FAT32?
	BYTE	bsBootSectSig3;
	BYTE	bsBootSectSig0;				// boot sector signature byte 0x55
	BYTE	bsBootSectSig1;				// boot sector signature byte 0xAA
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
#define BOOTSIG2        0
#define BOOTSIG3        0
};


/***************************************************************/
/***************************************************************/

// BIOS Parameter Block (BPB) for DOS 3.3
struct bpb33 {
        WORD	bpbBytesPerSec; // bytes per sector
        BYTE    bpbSecPerClust;	// sectors per cluster
        WORD	bpbResSectors;	// number of reserved sectors
        BYTE	bpbFATs;		// number of FATs
        WORD	bpbRootDirEnts;	// number of root directory entries
        WORD	bpbSectors;		// total number of sectors
        BYTE	bpbMedia;		// media descriptor
        WORD	bpbFATsecs;     // number of sectors per FAT
        WORD	bpbSecPerTrack; // sectors per track
        WORD	bpbHeads;       // number of heads
        WORD	bpbHiddenSecs;  // number of hidden sectors
};

// BPB for DOS 5.0
// The difference is bpbHiddenSecs is a short for DOS 3.3,
// and bpbHugeSectors is not present in the DOS 3.3 bpb.
struct bpb50 {
        WORD	bpbBytesPerSec; // bytes per sector
        BYTE	bpbSecPerClust; // sectors per cluster
        WORD	bpbResSectors;  // number of reserved sectors
        BYTE	bpbFATs;        // number of FATs
        WORD	bpbRootDirEnts; // number of root directory entries
        WORD	bpbSectors;     // total number of sectors
        BYTE	bpbMedia;       // media descriptor
        WORD	bpbFATsecs;     // number of sectors per FAT
        WORD	bpbSecPerTrack; // sectors per track
        WORD	bpbHeads;       // number of heads
        DWORD	bpbHiddenSecs;  // # of hidden sectors
// 3.3 compat ends here
        DWORD	bpbHugeSectors; // # of sectors if bpbSectors == 0
};

// BPB for DOS 7.10 (FAT32)
// This one has a few extensions to bpb50.
struct bpb710 {
		WORD	bpbBytesPerSec;	// bytes per sector
		BYTE	bpbSecPerClust;	// sectors per cluster
		WORD	bpbResSectors;	// number of reserved sectors
		BYTE	bpbFATs;		// number of FATs
		WORD	bpbRootDirEnts;	// number of root directory entries
		WORD	bpbSectors;		// total number of sectors
		BYTE	bpbMedia;		// media descriptor
		WORD	bpbFATsecs;		// number of sectors per FAT
		WORD	bpbSecPerTrack;	// sectors per track
		WORD	bpbHeads;		// number of heads
		DWORD	bpbHiddenSecs;	// # of hidden sectors
// 3.3 compat ends here
		DWORD	bpbHugeSectors;	// # of sectors if bpbSectors == 0
// 5.0 compat ends here
		DWORD     bpbBigFATsecs;// like bpbFATsecs for FAT32
		WORD      bpbExtFlags;	// extended flags:
#define FATNUM    0xf			// mask for numbering active FAT
#define FATMIRROR 0x80			// FAT is mirrored (like it always was)
		WORD      bpbFSVers;	// filesystem version
#define FSVERS    0				// currently only 0 is understood
		DWORD     bpbRootClust;	// start cluster for root directory
		WORD      bpbFSInfo;	// filesystem info structure sector
		WORD      bpbBackup;	// backup boot sector
		// There is a 12 byte filler here, but we ignore it
};




// ***************************************************************
// * byte versions of the above structs                          *
// ***************************************************************


// BIOS Parameter Block (BPB) for DOS 3.3
struct byte_bpb33 {
        CHAR bpbBytesPerSec[2];		// bytes per sector
        CHAR bpbSecPerClust;        // sectors per cluster
        CHAR bpbResSectors[2];      // number of reserved sectors
        CHAR bpbFATs;               // number of FATs
        CHAR bpbRootDirEnts[2];     // number of root directory entries
        CHAR bpbSectors[2];         // total number of sectors
        CHAR bpbMedia;              // media descriptor
        CHAR bpbFATsecs[2];         // number of sectors per FAT
        CHAR bpbSecPerTrack[2];     // sectors per track
        CHAR bpbHeads[2];           // number of heads
        CHAR bpbHiddenSecs[2];      // number of hidden sectors
};

// BPB for DOS 5.0
// The difference is bpbHiddenSecs is a short for DOS 3.3,
// and bpbHugeSectors is not in the 3.3 bpb.
struct byte_bpb50 {
        CHAR bpbBytesPerSec[2];     // bytes per sector
        CHAR bpbSecPerClust;        // sectors per cluster
        CHAR bpbResSectors[2];      // number of reserved sectors
        CHAR bpbFATs;               // number of FATs
        CHAR bpbRootDirEnts[2];     // number of root directory entries
        CHAR bpbSectors[2];         // total number of sectors
        CHAR bpbMedia;              // media descriptor
        CHAR bpbFATsecs[2];         // number of sectors per FAT
        CHAR bpbSecPerTrack[2];     // sectors per track
        CHAR bpbHeads[2];           // number of heads
        CHAR bpbHiddenSecs[4];      // number of hidden sectors
        CHAR bpbHugeSectors[4];		// # of sectors if bpbSectors == 0
};

// BPB for DOS 7.10 (FAT32).
// This one has a few extensions to bpb50.
struct byte_bpb710 {
        BYTE bpbBytesPerSec[2];     // bytes per sector
        BYTE bpbSecPerClust;        // sectors per cluster
        BYTE bpbResSectors[2];      // number of reserved sectors
        BYTE bpbFATs;               // number of FATs
        BYTE bpbRootDirEnts[2];     // number of root directory entries
        BYTE bpbSectors[2];         // total number of sectors
        BYTE bpbMedia;              // media descriptor
        BYTE bpbFATsecs[2];         // number of sectors per FAT
        BYTE bpbSecPerTrack[2];     // sectors per track
        BYTE bpbHeads[2];           // number of heads
        BYTE bpbHiddenSecs[4];      // # of hidden sectors
        BYTE bpbHugeSectors[4];     // # of sectors if bpbSectors == 0
        BYTE bpbBigFATsecs[4];      // like bpbFATsecs for FAT32
        BYTE bpbExtFlags[2];        // extended flags:
        BYTE bpbFSVers[2];          // filesystem version
        BYTE bpbRootClust[4];       // start cluster for root directory
        BYTE bpbFSInfo[2];          // filesystem info structure sector
        BYTE bpbBackup[2];          // backup boot sector
        // There is a 12 byte filler here, but we ignore it
};

// FAT32 FSInfo block.
struct fsinfo {
        BYTE fsisig1[4];
        BYTE fsifill1[480];
        BYTE fsisig2[4];
        BYTE fsinfree[4];
        BYTE fsinxtfree[4];
        BYTE fsifill2[12];
        BYTE fsisig3[4];
        BYTE fsifill3[508];
        BYTE fsisig4[4];
};


/***************************************************************/
/***************************************************************/


// Structure of a dos directory entry.
struct direntry {
		BYTE		deName[8];      // filename, blank filled
#define SLOT_EMPTY      0x00            // slot has never been used
#define SLOT_E5         0x05            // the real value is 0xe5
#define SLOT_DELETED    0xe5            // file in this slot deleted
		BYTE		deExtension[3]; // extension, blank filled
		BYTE		deAttributes;   // file attributes
#define ATTR_NORMAL     0x00            // normal file
#define ATTR_READONLY   0x01            // file is readonly
#define ATTR_HIDDEN     0x02            // file is hidden
#define ATTR_SYSTEM     0x04            // file is a system file
#define ATTR_VOLUME     0x08            // entry is a volume label
#define ATTR_LONG_FILENAME	0x0f		// this is a long filename entry			    
#define ATTR_DIRECTORY  0x10            // entry is a directory name
#define ATTR_ARCHIVE    0x20            // file is new or modified
		BYTE        deLowerCase;    // NT VFAT lower case flags
#define LCASE_BASE      0x08            // filename base in lower case
#define LCASE_EXT       0x10            // filename extension in lower case
		BYTE        deCHundredth;   // hundredth of seconds in CTime
		BYTE        deCTime[2];     // create time
		BYTE        deCDate[2];     // create date
		BYTE        deADate[2];     // access date
		WORD        deHighClust; 	// high bytes of cluster number
		BYTE        deMTime[2];     // last update time
		BYTE        deMDate[2];     // last update date
		WORD        deStartCluster; // starting cluster of file
		DWORD       deFileSize;  	// size of file in bytes
};

// number of directory entries in one sector
#define DIRENTRIES_PER_SECTOR	0x10

// Structure of a Win95 long name directory entry
struct winentry {
		BYTE		weCnt;
#define WIN_LAST        0x40
#define WIN_CNT         0x3f
		BYTE		wePart1[10];
		BYTE		weAttributes;
#define ATTR_WIN95      0x0f
		BYTE		weReserved1;
		BYTE		weChksum;
		BYTE		wePart2[12];
		WORD       	weReserved2;
		BYTE		wePart3[4];
};

#define WIN_CHARS	13      // Number of chars per winentry

// Maximum filename length in Win95
// Note: Must be < sizeof(dirent.d_name)
#define WIN_MAXLEN      255

// This is the format of the contents of the deTime field in the direntry
// structure.
// We don't use bitfields because we don't know how compilers for
// arbitrary machines will lay them out.
#define DT_2SECONDS_MASK        0x1F    // seconds divided by 2
#define DT_2SECONDS_SHIFT       0
#define DT_MINUTES_MASK         0x7E0   // minutes
#define DT_MINUTES_SHIFT        5
#define DT_HOURS_MASK           0xF800  // hours
#define DT_HOURS_SHIFT          11

// This is the format of the contents of the deDate field in the direntry
// structure.
#define DD_DAY_MASK             0x1F    // day of month
#define DD_DAY_SHIFT            0
#define DD_MONTH_MASK           0x1E0   // month
#define DD_MONTH_SHIFT          5
#define DD_YEAR_MASK            0xFE00  // year - 1980
#define DD_YEAR_SHIFT           9

// Prototypes
unsigned char fatInit( unsigned char device);
unsigned int fatClusterSize(void);
unsigned long fatGetDirEntry(unsigned int entry, unsigned int count);
unsigned long fatGetFilesize(void);
char* fatGetFilename(void);
char* fatGetDirname(void);
void fatLoadCluster(unsigned long cluster, unsigned char *buffer);
unsigned long fatNextCluster(unsigned long cluster);

#endif
