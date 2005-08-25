/*! \file gps.h \brief GPS position storage and processing library. */
//*****************************************************************************
//
// File Name	: 'gps.h'
// Title		: GPS position storage and processing function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002.08.29
// Revised		: 2002.08.29
// Version		: 0.1
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

#ifndef GPS_H
#define GPS_H

#include "global.h"

// constants/macros/typdefs
typedef union union_float_u32
{
	float f;
	unsigned long i;
	unsigned char b[4];
} float_u32;

typedef union union_double_u64
{
	double f;
	unsigned long long i;
	unsigned char b[8];
} double_u64;

struct PositionLLA
{
	float_u32 lat;
	float_u32 lon;
	float_u32 alt;
	float_u32 TimeOfFix;
	u16 updates;
};

struct VelocityENU
{
	float_u32 east;
	float_u32 north;
	float_u32 up;
	float_u32 TimeOfFix;
	u16 updates;
};

struct VelocityHS
{
	float_u32 heading;
	float_u32 speed;
	float_u32 TimeOfFix;
	u16 updates;
};

struct PositionECEF
{
	float_u32 x;
	float_u32 y;
	float_u32 z;
	float_u32 TimeOfFix;
	u16 updates;
};

struct VelocityECEF
{
	float_u32 x;
	float_u32 y;
	float_u32 z;
	float_u32 TimeOfFix;
	u16 updates;
};

typedef struct struct_GpsInfo
{	
	float_u32 TimeOfWeek;
	u16 WeekNum;
	float_u32 UtcOffset;
	u08 numSVs;
	
	struct PositionLLA PosLLA;
	struct PositionECEF PosECEF;
	struct VelocityECEF VelECEF;
	struct VelocityENU VelENU;
	struct VelocityHS VelHS;

} GpsInfoType;

// functions
void gpsInit(void);
GpsInfoType* gpsGetInfo(void);
void gpsInfoPrint(void);

#endif
