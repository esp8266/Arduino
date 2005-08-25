/*! \file satmb.c \brief Satellite Motherboard Driver Functions. */
//*****************************************************************************
//
// File Name	: 'satmb.c'
// Title		: Satellite Motherboard Driver Functions
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.10.13
// Revised		: 2004.10.13
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>	// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "dallas.h"
#include "ds2450.h"
#include "satmb.h"
#include "dallasids.h"

// globals

// functions
void satmbInit(void)
{
	// preset serial port power to on
	satmbSetSerialPortPower(1);
}

void satmbSetSerialPortPower(u08 on)
{
	// set I/O control line to output
	sbi(SATMB_SER_PWR_DDR, SATMB_SER_PWR_PIN);
	// set serial port power state
	if(on)
		sbi(SATMB_SER_PWR_PORT, SATMB_SER_PWR_PIN);
	else
		cbi(SATMB_SER_PWR_PORT, SATMB_SER_PWR_PIN);
}

void satmbSerialRtsCtsInit(void)
{
	// initialize RTS/CTS lines for operation
	// RTS is input, set pullup
	cbi(SATMB_SER_RTS_DDR, SATMB_SER_RTS_PIN);
	sbi(SATMB_SER_RTS_PORT, SATMB_SER_RTS_PIN);
	// CTS is output, init low
	sbi(SATMB_SER_CTS_DDR, SATMB_SER_CTS_PIN);
	cbi(SATMB_SER_CTS_PORT, SATMB_SER_CTS_PIN);
}

u08 satmbSerialRtsCheck(void)
{
	if(inb(SATMB_SER_RTS_PORTIN) & (1<<SATMB_SER_RTS_PIN))
		return TRUE;
	else
		return FALSE;
}

void satmbSerialCtsSet(u08 state)
{
	if(state)
		sbi(SATMB_SER_CTS_PORT, SATMB_SER_CTS_PIN);
	else
		cbi(SATMB_SER_CTS_PORT, SATMB_SER_CTS_PIN);
}

u16 satmbV1GetCurrent(DallasSubsysId* targetSubsysId)
{
	u16 value;
	// setup A/D for 5V 16-bit conversion
	ds2450SetupAll(&targetSubsysId->S, 16, DS2450_RANGE_5V);
	// read current-level A/D
	ds2450StartAndResult(&targetSubsysId->S, 'A', &value);
	// calculate milliamp value
	// ma = 1000*(((value/65536)*5.12V)/(50*R))
	// for R=0.47/2 ohms
	// return result
	return value/150;
}

u16 satmbV2GetCurrent(DallasSubsysId* targetSubsysId)
{
	u16 value;
	// setup A/D for 5V 16-bit conversion
	ds2450SetupAll(&targetSubsysId->S, 16, DS2450_RANGE_5V);
	// read current-level A/D
	ds2450StartAndResult(&targetSubsysId->S, 'C', &value);
	// calculate milliamp value
	// ma = 1000*(((value/65536)*5.12V)/(50*R))
	// for R=0.1/2 ohms
	// return result
	return value/32;
}

u08 satmbV1GetOverCurrent(DallasSubsysId* targetSubsysId)
{
	u16 value;
	// setup A/D for 5V 16-bit conversion
	ds2450SetupAll(&targetSubsysId->S, 16, DS2450_RANGE_5V);
	// read overcurrent state A/D
	ds2450StartAndResult(&targetSubsysId->S, 'B', &value);
	// return result
	return (value>0x8000);
}

u08 satmbV2GetOverCurrent(DallasSubsysId* targetSubsysId)
{
	u16 value;
	// setup A/D for 5V 16-bit conversion
	ds2450SetupAll(&targetSubsysId->S, 16, DS2450_RANGE_5V);
	// read overcurrent state A/D
	ds2450StartAndResult(&targetSubsysId->S, 'D', &value);
	// return result
	return (value>0x8000);
}

void satmbV1SetPowerState(DallasSubsysId* targetSubsysId, u08 state)
{
	satmbSetPowerState(&targetSubsysId->V1, state);
}

void satmbV2SetPowerState(DallasSubsysId* targetSubsysId, u08 state)
{
	satmbSetPowerState(&targetSubsysId->V2, state);
}

void satmbSetPowerState(dallas_rom_id_T* targetRomId, u08 state)
{
	if(state)
	{
		// reset overcurrent flag
		ds2450DigitalOut(targetRomId, 'B', DIGOUT_LOW);
		ds2450DigitalOut(targetRomId, 'B', DIGOUT_OC);
		// assert enable
		ds2450DigitalOut(targetRomId, 'A', DIGOUT_LOW);
	}
	// pulse clock line
	ds2450DigitalOut(targetRomId, 'C', DIGOUT_OC);
	ds2450DigitalOut(targetRomId, 'C', DIGOUT_LOW);
	ds2450DigitalOut(targetRomId, 'C', DIGOUT_OC);
	// release enable
	ds2450DigitalOut(targetRomId, 'A', DIGOUT_OC);
}
