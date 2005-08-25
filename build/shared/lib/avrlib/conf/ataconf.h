/*! \file ataconf.h \brief IDE-ATA interface driver configuration. */
//*****************************************************************************
//
// File Name	: 'ataconf.h'
// Title		: IDE-ATA interface driver for hard disks
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Date			: 11/22/2000
// Revised		: 12/29/2000
// Version		: 0.3
// Target MCU	: Atmel AVR Series)
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

#ifndef ATA_IFCONF_H
#define ATA_IFCONF_H

// constants
#define SECTOR_BUFFER_ADDR			0x1E00

// ATA register base address
#define ATA_REG_BASE		0x8000
// ATA register offset
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

#endif
