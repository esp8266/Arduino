/*! \file satmb.h \brief Satellite Motherboard Driver Functions. */
//*****************************************************************************
//
// File Name	: 'satmb.h'
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

#ifndef SATMB_H
#define SATMB_H

#include "dallas.h"
#include "dallasids.h"


// defines and typedefs

// SAT-MB serial port control
// CTS is an output signal
#define SATMB_SER_CTS_PORT			PORTB
#define SATMB_SER_CTS_DDR			DDRB
#define SATMB_SER_CTS_PORTIN		PINB
#define SATMB_SER_CTS_PIN			PB5
// RTS is an input signal
#define SATMB_SER_RTS_PORT			PORTB
#define SATMB_SER_RTS_DDR			DDRB
#define SATMB_SER_RTS_PORTIN		PINB
#define SATMB_SER_RTS_PIN			PB6
// Serial Port Power Control (set low to turn off)
#define SATMB_SER_PWR_PORT			PORTD
#define SATMB_SER_PWR_DDR			DDRD
#define SATMB_SER_PWR_PORTIN		PIND
#define SATMB_SER_PWR_PIN			PD5

// SAT-MB Linx Radio Transceiver
// Non-UART RX line (receive)
#define SATMB_LINX_IO_RX_PORT		PORTD
#define SATMB_LINX_IO_RX_DDR		DDRD
#define SATMB_LINX_IO_RX_PORTIN		PIND
#define SATMB_LINX_IO_RX_PIN		PD4
// Non-UART TX line (transmit)
#define SATMB_LINX_IO_TX_PORT		PORTB
#define SATMB_LINX_IO_TX_DDR		DDRB
#define SATMB_LINX_IO_TX_PORTIN		PINB
#define SATMB_LINX_IO_TX_PIN		PB7
// Linx Radio Power Control (set low to turn off)
#define SATMB_LINX_PWR_PORT			PORTD
#define SATMB_LINX_PWR_DDR			DDRD
#define SATMB_LINX_PWR_PORTIN		PIND
#define SATMB_LINX_PWR_PIN			PD5
// Radio Receive Signal Strength Indicator (RSSI)
// this is an analog output
#define SATMB_LINX_RSSI_PORT		PORTF
#define SATMB_LINX_RSSI_DDR			DDRF
#define SATMB_LINX_RSSI_PORTIN		PINF
#define SATMB_LINX_RSSI_PIN			PF7

// SAT-MB Direct Dallas Bus Driver
// Dallas Line Pin
#define SATMB_DALLAS_LINE_PORT		PORTE
#define SATMB_DALLAS_LINE_DDR		DDRE
#define SATMB_DALLAS_LINE_PORTIN	PINE
#define SATMB_DALLAS_LINE_PIN		PE7
// Dallas OC-Tx Pin
#define SATMB_DALLAS_TX_PORT		PORTE
#define SATMB_DALLAS_TX_DDR			DDRE
#define SATMB_DALLAS_TX_PORTIN		PINE
#define SATMB_DALLAS_TX_PIN			PE3
// Dallas Strong-Pullup Pin
#define SATMB_DALLAS_SPU_PORT		PORTE
#define SATMB_DALLAS_SPU_DDR		DDRE
#define SATMB_DALLAS_SPU_PORTIN		PINE
#define SATMB_DALLAS_SPU_PIN		PE4

// functions

//! Initializes SAT-MB hardware
void satmbInit(void);

//! Controls power to the SAT-MB serial port
//  TRUE = on
//  FALSE = off
void satmbSetSerialPortPower(u08 on);

//! Initializes the SAT-MB serial port RTS/CTS lines
void satmbSerialRtsCtsInit(void);

//! Returns the current state of the SAT-MB serial port RTS line
u08 satmbSerialRtsCheck(void);

//! Sets the current state of the SAT-MB serial port CTS line
void satmbSerialCtsSet(u08 state);

// Power control commands (dallas bus)
u16 satmbV1GetCurrent(DallasSubsysId* targetSubsysId);
u16 satmbV2GetCurrent(DallasSubsysId* targetSubsysId);
u08 satmbV1GetOverCurrent(DallasSubsysId* targetSubsysId);
u08 satmbV2GetOverCurrent(DallasSubsysId* targetSubsysId);
void satmbV1SetPowerState(DallasSubsysId* targetSubsysId, u08 state);
void satmbV2SetPower(DallasSubsysId* targetSubsysId, u08 state);

void satmbSetPowerState(dallas_rom_id_T* targetRomId, u08 state);

#endif
