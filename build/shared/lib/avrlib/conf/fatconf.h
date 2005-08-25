/*! \file fatconf.h \brief FAT16/32 file system driver configuration. */
//*****************************************************************************
//
// File Name	: 'fatconf.h'
// Title		: FAT16/32 file system driver configuration
// Author		: Pascal Stang
// Date			: 4/19/2003
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

#ifndef FATCONF_H
#define FATCONF_H

// debug on/off
#define DEBUG_FAT

#define SECTOR_BUFFER1_ADDR			0x0600+0x0600
#define SECTOR_BUFFER1_SIZE			0x0200

#define LONGNAME_BUFFER_ADDR		0x0200+0x0600
#define LONGNAME_BUFFER_SIZE		0x0100

#define DIRNAME_BUFFER_ADDR			0x0300+0x0600
#define DIRNAME_BUFFER_SIZE			0x0100

#define FAT_CACHE_ADDR				0x0400+0x0600
#define FAT_CACHE_SIZE				0x0200

#endif
