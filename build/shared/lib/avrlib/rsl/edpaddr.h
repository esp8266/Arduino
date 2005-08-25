/*! \file edpaddr.h \brief Emerald Satellite EDP/I2C Bus Addresses. */
//*****************************************************************************
//
// File Name	: 'edpaddr.h'
// Title		: Emerald Satellite EDP/I2C Bus Addresses
// Author		: Bryan Palmintier & Pascal Stang - Copyright (C) 2003
// Created		: 09/08/2003 by PS
// Revised		: 11/10/2003 by BP
// Version		: 0.9
// Target MCU	: Any
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef EDPADDR_H
#define EDPADDR_H

// The 8 bits of an EDP address byte breakdown as follows:
//  b0: R/W bit (1=read, 0=write)
//  b1-4: subsystem address (16 availible)
//  b5-7: satellite network mask (up to 7 unique networks with 1 reserved by I2C)
//  The ground station is a special case which uses 1110xxxx
#define EDPNET_MASK   		0xE0	// mask for satellite/ground networks
#define EDPADDR_MASK		0x1E	// mask for subsystem addresses

// Satellite network addresses
#define EDPNET_SAT_A		0x00	// EM-1
#define EDPNET_SAT_B		0x20	// EM-2
#define EDPNET_SAT_C		0x40	// EM-3
#define EDPNET_SAT_D		0x60	// EM-4
#define EDPNET_SAT_E		0x80	// OK-1
#define EDPNET_SAT_F		0xA0	// OK-2
#define EDPNET_SAT_G		0xC0

// Ground Station network address
// NOTE: all devices on this network must maintain b4=0
//  or risk problems with 10bit I2C addressing
#define EDPNET_GROUND		0xE0

// Test subsystem address
// Note: it is OK to use these susbsystem addresses with the EDPADDR_GROUND mask
#define EDPADDR_TEST		0x02	// generic test address (LEDs etc)
#define EDPADDR_GROUND		0x04	// address for ground testing

// Subsystem addresses
// "Core" subsystems, those found on all satellites, DO NOT EDIT
// Note: it is OK to use these subsystem addresses with the EDPADDR_GROUND mask
#define EDPADDR_COMM		0x06
#define EDPADDR_DALMAST		0x08
#define EDPADDR_SCHED		0x0A
#define EDPADDR_EXPSYS		0x0C
#define EDPADDR_ISCOMM		0x0E

// "Common" subsystems, those found on many satellites, DO NOT EDIT
// Note: it is NOT OK to use these subsystem addresses with the EDPADDR_GROUND mask
#define EDPADDR_GPS   		0x10
#define EDPADDR_TORQUER		0x12
#define EDPADDR_MECH		0x14

// Mission Specific subsystems, EDIT AS NEEDED
// Note: it is NOT OK to use these subsystem addresses with the EDPADDR_GROUND mask
#define EDPADDR_ODDSS		0x16
#define EDPADDR_ULTRAWB		0x18
#define EDPADDR_TETHER		0x1A


// As part of the I2C protocol 000000000 is reserved for general calls and
// all 1111xxxx are reserved for 10 bit addressing
#define EDPADDR_RESERVED_GENCALL	0x00	// reserved by I2C for general call address
#define EDPADDR_RESERVED_10BIT		0xF0	// reserved by I2C for 10bit addressing

#endif
