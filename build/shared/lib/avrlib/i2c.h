/*! \file i2c.h \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.h'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.03
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef I2C_H
#define I2C_H

#include "global.h"

// include project-specific configuration
#include "i2cconf.h"

// TWSR values (not bits)
// (taken from avr-libc twi.h - thank you Marek Michalkiewicz)
// Master
#define TW_START					0x08
#define TW_REP_START				0x10
// Master Transmitter
#define TW_MT_SLA_ACK				0x18
#define TW_MT_SLA_NACK				0x20
#define TW_MT_DATA_ACK				0x28
#define TW_MT_DATA_NACK				0x30
#define TW_MT_ARB_LOST				0x38
// Master Receiver
#define TW_MR_ARB_LOST				0x38
#define TW_MR_SLA_ACK				0x40
#define TW_MR_SLA_NACK				0x48
#define TW_MR_DATA_ACK				0x50
#define TW_MR_DATA_NACK				0x58
// Slave Transmitter
#define TW_ST_SLA_ACK				0xA8
#define TW_ST_ARB_LOST_SLA_ACK		0xB0
#define TW_ST_DATA_ACK				0xB8
#define TW_ST_DATA_NACK				0xC0
#define TW_ST_LAST_DATA				0xC8
// Slave Receiver
#define TW_SR_SLA_ACK				0x60
#define TW_SR_ARB_LOST_SLA_ACK		0x68
#define TW_SR_GCALL_ACK				0x70
#define TW_SR_ARB_LOST_GCALL_ACK	0x78
#define TW_SR_DATA_ACK				0x80
#define TW_SR_DATA_NACK				0x88
#define TW_SR_GCALL_DATA_ACK		0x90
#define TW_SR_GCALL_DATA_NACK		0x98
#define TW_SR_STOP					0xA0
// Misc
#define TW_NO_INFO					0xF8
#define TW_BUS_ERROR				0x00

// defines and constants
#define TWCR_CMD_MASK		0x0F
#define TWSR_STATUS_MASK	0xF8

// return values
#define I2C_OK				0x00
#define I2C_ERROR_NODEV		0x01

// types
typedef enum
{
	I2C_IDLE = 0, I2C_BUSY = 1,
	I2C_MASTER_TX = 2, I2C_MASTER_RX = 3,
	I2C_SLAVE_TX = 4, I2C_SLAVE_RX = 5
} eI2cStateType;

// functions

//! Initialize I2C (TWI) interface
void i2cInit(void);

//! Set the I2C transaction bitrate (in KHz)
void i2cSetBitrate(u16 bitrateKHz);

// I2C setup and configurations commands
//! Set the local (AVR processor's) I2C device address
void i2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn);

//! Set the user function which handles receiving (incoming) data as a slave
void i2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(u08 receiveDataLength, u08* recieveData));
//! Set the user function which handles transmitting (outgoing) data as a slave
void i2cSetSlaveTransmitHandler(u08 (*i2cSlaveTx_func)(u08 transmitDataLengthMax, u08* transmitData));

// Low-level I2C transaction commands 
//! Send an I2C start condition in Master mode
void i2cSendStart(void);
//! Send an I2C stop condition in Master mode
void i2cSendStop(void);
//! Wait for current I2C operation to complete
void i2cWaitForComplete(void);
//! Send an (address|R/W) combination or a data byte over I2C
void i2cSendByte(u08 data);
//! Receive a data byte over I2C  
// ackFlag = TRUE if recevied data should be ACK'ed
// ackFlag = FALSE if recevied data should be NACK'ed
void i2cReceiveByte(u08 ackFlag);
//! Pick up the data that was received with i2cReceiveByte()
u08 i2cGetReceivedByte(void);
//! Get current I2c bus status from TWSR
u08 i2cGetStatus(void);

// high-level I2C transaction commands

//! send I2C data to a device on the bus
void i2cMasterSend(u08 deviceAddr, u08 length, u08 *data);
//! receive I2C data from a device on the bus
void i2cMasterReceive(u08 deviceAddr, u08 length, u08* data);

//! send I2C data to a device on the bus (non-interrupt based)
u08 i2cMasterSendNI(u08 deviceAddr, u08 length, u08* data);
//! receive I2C data from a device on the bus (non-interrupt based)
u08 i2cMasterReceiveNI(u08 deviceAddr, u08 length, u08 *data);

//! Get the current high-level state of the I2C interface
eI2cStateType i2cGetState(void);

#endif
