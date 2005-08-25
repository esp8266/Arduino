/*! \file cmdline.h \brief Command-Line Interface Library. */
//*****************************************************************************
//
// File Name	: 'cmdline.h'
// Title		: Command-Line Interface Library
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.07.16
// Revised		: 2003.07.16
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// Description	:
//		This Command-Line interface library is meant to provide a reusable
//	terminal-like user interface much like a DOS command line or UNIX terminal.
//
//	The cmdline library does the following things for you:
//		-Prints command prompts
//		-Gathers a command string from the user (with editing features)
//		-Parses the command string when the user presses [ENTER]
//		-Compares the entered command to the command database
//		  \-Executes the corresponding function if a match is found
//		  \-Reports an error if no match is found
//		-Provides functions to retrieve the command arguments:
//		  \-as strings
//		  \-as decimal integers
//		  \-as hex integers
//
//	Supported editing features include:
//		-Backspace support
//		-Mid-line editing, inserting and deleting (left/right-arrows)
//		-Command History (up-arrow) (currently only one command deep)
//
//	To use the cmdline system, you will need to associate command strings
//	(commands the user will be typing) with your function that you wish to have
//	called when the user enters that command.  This is done by using the
//	cmdlineAddCommand() function.
//
//	To setup the cmdline system, you must do these things:
//		-Initialize it: cmdlineInit()
//		-Add one or more commands to the database: cmdlineAddCommand()
//		-Set an output function for your terminal: cmdlineSetOutputFunc()
//
//	To operate the cmdline system, you must do these things repeatedly:
//		-Pass user input from the terminal to: cmdlineSetOutputFunc()
//		-Call cmdlineMainLoop() from your program's main loop
//
//	The cmdline library does not assume an input or output device, but can be
//	configured to use any user function for output using cmdlineSetOutputFunc()
//	and accepts input by calling cmdlineInputFunc().  This means the cmdline
//	library can operate over any interface including UART (serial port),
//	I2c, ethernet, etc.
//
//	***** FOR MORE INFORMATION ABOUT USING cmdline SEE THE AVRLIB EXAMPLE *****
//	***** CODE IN THE avrlib/examples DIRECTORY                           *****
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef CMDLINE_H
#define CMDLINE_H

#include "global.h"

// constants/macros/typdefs
typedef void (*CmdlineFuncPtrType)(void);

// functions

//! initalize the command line system
void cmdlineInit(void);

//! add a new command to the database of known commands
// newCmdString should be a null-terminated command string with no whitespace
// newCmdFuncPtr should be a pointer to the function to execute when
//   the user enters the corresponding command tring
void cmdlineAddCommand(u08* newCmdString, CmdlineFuncPtrType newCmdFuncPtr);

//! sets the function used for sending characters to the user terminal
void cmdlineSetOutputFunc(void (*output_func)(unsigned char c));

//! call this function to pass input charaters from the user terminal
void cmdlineInputFunc(unsigned char c);

//! call this function in your program's main loop
void cmdlineMainLoop(void);

// internal commands
void cmdlineRepaint(void);
void cmdlineDoHistory(u08 action);
void cmdlineProcessInputString(void);
void cmdlinePrintPrompt(void);
void cmdlinePrintError(void);

// argument retrieval commands
//! returns a string pointer to argument number [argnum] on the command line
u08* cmdlineGetArgStr(u08 argnum);
//! returns the decimal integer interpretation of argument number [argnum]
long cmdlineGetArgInt(u08 argnum);
//! returns the hex integer interpretation of argument number [argnum]
long cmdlineGetArgHex(u08 argnum);

#endif
