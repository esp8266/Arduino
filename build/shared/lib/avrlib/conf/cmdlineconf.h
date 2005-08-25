/*! \file cmdlineconf.h \brief Command-Line Interface Library Configuration. */
//*****************************************************************************
//
// File Name	: 'cmdlineconf.h'
// Title		: Command-Line Interface Library Configuration
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.07.16
// Revised		: 2003.07.21
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

#ifndef CMDLINECONF_H
#define CMDLINECONF_H

#include "global.h"

// constants/macros/typdefs

// size of command database
// (maximum number of commands the cmdline system can handle)
#define CMDLINE_MAX_COMMANDS	10

// maximum length (number of characters) of each command string
// (quantity must include one additional byte for a null terminator)
#define CMDLINE_MAX_CMD_LENGTH	15

// allotted buffer size for command entry
// (must be enough chars for typed commands and the arguments that follow)
#define CMDLINE_BUFFERSIZE		80

// number of lines of command history to keep
// (each history buffer is CMDLINE_BUFFERSIZE in size)
// ***** ONLY ONE LINE OF COMMAND HISTORY IS CURRENTLY SUPPORTED
#define CMDLINE_HISTORYSIZE		1

#endif
