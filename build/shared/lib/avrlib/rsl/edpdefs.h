/*! \file edpdefs.h \brief Emerald Data Protocol Defines and Constants. */
//*****************************************************************************
//
// File Name	: 'edpdefs.h'
// Title		: Emerald Data Protocol Defines and Constants
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 09/08/2003
// Revised		: 09/08/2003
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef EDPDEFS_H
#define EDPDEFS_H

// **** Constant Definitions *****
// ---- Message Size, etc ----
// Standard defines for various message format parameters
#define I2C_MAX_COMMAND_LENGTH		127	// [param1]...[param127]  (does not include Command, From 
										// or Chk) Note that this an absolute maximum, 
										// subsystems/nodes are free to set lower maximum lengths
										// For versions pre rev5, this was fixed at 5.
#define I2C_MAX_DATA_PACKET_LENGTH	127	// Raw Data: [data1]...[data126][Chk]   (includes Chk for 


// ---- Communication OK (value all uppercase ('A'-'Z')
#define EDP_RESP_ACK				'A'	// indicates the command (to_send) was sent but has no return value.
#define EDP_RESP_DATA_REPLY			'R'	// command valid and has return, examine reply and length for details

// ---- Communication Error (values all lowercase ('a'-'z'))
#define EDP_RESP_UNKWN_CMD			'u'	// given command is unrecognized by the subsystem at that address
#define EDP_RESP_CMD_CHK_ERROR		'c'	// checksum error sending command
#define EDP_RESP_DATA_CHK_ERROR		'd'	// checksum error receiving data
#define EDP_RESP_SEQUENCE_ERROR		's'	// read/write out of order
#define EDP_RESP_READ_BEFORE_WRITE	'b'	// requested read before writting associated Command 
#define EDP_RESP_TOO_LONG         	'l'	// The command sent exceeds the maximum command length for node
#define EDP_RESP_TOO_FEW_PARAMS		'p'	// The command sent has too few parameters
#define EDP_RESP_INCORRECT_PARAM	'i'	// Parameters are incorrect (but there are the right number)
#define EDP_RESP_BUSY				'b'	// The subsystem is still alive, but too busy to reply (AVOID USING)
#define EDP_RESP_NOT_ALLOWED		'n'	// The command is recognized, but not allowed in the current
										// operating mode. Try another command, or try again later
#define EDP_RESP_OTHER_ERROR		'e'	// unspecified EDP/I2C error

// ---- Check Sum ----
/* The Checksum that is used is a rolling 8-bit sum from the [From] to the last parameter byte of a command
   packet and from the [Length] to the last data byte of a Data packet. This sum is then 1-complemented
   (~, not) and passed as [Chk]. This prevents a series of 0x00 replys from passing the correct check sum. 
   Because of the inversion, a packet with all zeros should have a checksum of 0xFF.
   
   The other nice feature of this checksum, is that no matter what the data is, if you add the checksum
   ([Chk]) to the final sum, you should get 0xFF.
   
   To make it even cleaner, you can start the rolling checksum at 0x01 such that when you add in all of the
   data bytes and the [Chk] byte, you get 0x00. This effectively makes the whole operation a twos complement
*/
#define EDP_CHECKSUM_INIT 0x01

// -------- Reserved I2C commands ---------
// Define a short list of reserved commands. Subsystems can choose whether or
// not to implement these commands, but if they are implemented, they must
// function as described below.

//Reserved Commands
#define	EDP_CMD_SET_TIME	':'	//0x3A  Set the subsystem time, realtime.h format
#define	EDP_CMD_RESERVED_1	';'	//0x3B  Reserved for future command
#define	EDP_CMD_ROM_WRITE	'<'	//0x3C  Write to program ROM (uploadable code)
#define	EDP_CMD_RESERVED_2	'='	//0x3D  Reserved for future command
#define	EDP_CMD_MEM_READ	'>'	//0x3E  Read from program ROM (check program)
#define	EDP_CMD_HELP		'?'	//0x3F  Return human readable help string(s)
#define	EDP_CMD_STATUS		'@'	//0x40  Get subsystem status


#define I2C_DATA_CONTINUE_MASK	   0x80 // If MSB of length is set, then the data continues beyond
                              	    	// this data packet


#endif
