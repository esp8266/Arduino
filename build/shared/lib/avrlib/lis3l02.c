/*! \file lis3l02.c \brief ST LIS3L02 3-axis I2C Accelerometer Library. */
//*****************************************************************************
//
// File Name	: 'lis3l02.c'
// Title		: ST LIS3L02 3-axis I2C Accelerometer Library
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.10.23
// Revised		: 2004.12.14
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

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "global.h"
#include "i2c.h"
#include "lis3l02.h"

#include "rprintf.h"
#include "timer.h"

// global variables

// Functions
u08 lis3l02Init(void)
{
	// reset LIS3L02 chip
	return lis3l02Reset();
}

u08 lis3l02Reset(void)
{
	// turn on device and enable X,Y,Z
	lis3l02WriteReg(LIS3L02_REG_CTRLREG1,
		LIS3L02_CTRLREG1_XEN |
		LIS3L02_CTRLREG1_YEN |
		LIS3L02_CTRLREG1_ZEN |
		LIS3L02_CTRLREG1_PD0);

	// scale and justification options
	lis3l02WriteReg(LIS3L02_REG_CTRLREG2,
		LIS3L02_CTRLREG2_BOOT | 
		LIS3L02_CTRLREG2_DAS );

	return 0;
}

u08 lis3l02ReadReg(u08 reg)
{
	u08 data;
	u08 i2cStat;

	// set register
	i2cStat = i2cMasterSendNI(LIS3L02_I2C_ADDR, 1, &reg);
	if(i2cStat == I2C_ERROR_NODEV)
	{
		rprintf("No I2C Device\r\n");
		return i2cStat;
	}
	// read register
	i2cStat = i2cMasterReceiveNI(LIS3L02_I2C_ADDR, 1, &data);

	//rprintf("READ: Reg=0x%x  Data=0x%x\r\n", reg, data);

	return data;
}

u08 lis3l02WriteReg(u08 reg, u08 data)
{
	u08 packet[2];
	u08 i2cStat;
	
	// prepare packet
	packet[0] = reg;
	packet[1] = data;
	// write register
	i2cStat = i2cMasterSendNI(LIS3L02_I2C_ADDR, 2, packet);
	if(i2cStat == I2C_ERROR_NODEV)
	{
		rprintf("No I2C Device\r\n");
		return i2cStat;
	}

	//rprintf("WRITE: Reg=0x%x  Data=0x%x\r\n", reg, data);

	return (i2cStat == I2C_OK);
}

s16 lis3l02GetAccel(u08 chxyz)
{
	s16 value;
	
	value  = lis3l02ReadReg(LIS3L02_REG_OUTXL + (chxyz<<1));
	value |= lis3l02ReadReg(LIS3L02_REG_OUTXH + (chxyz<<1))<<8;

	return value;
}



