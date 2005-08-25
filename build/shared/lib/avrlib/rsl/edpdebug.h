/*! \file edpdebug.h \brief Emerald Data Protocol Debug Functions. */
//*****************************************************************************
//
// File Name	: 'edpdebug.h'
// Title		: Emerald Data Protocol Debug Functions
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.09.20
// Revised		: 2003.09.20
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef EDPDEBUG_H
#define EDPDEBUG_H

#include "edp.h"

// functions
u08 edpComposeCommand(u08 srcEdpAddr, u08* cmdBuffer);
void edpRunCommand(u08 destEdpAddr, u08 cmdLength, u08* cmdBuffer);

// display functions
void edpDisplayCommand(u08 length, EdpCommand* edpCommand);
void edpDisplayReply(u08 response, EdpReply* edpReply);
//void edpDisplayReplyOld(u08 response, u08 replyLength, u08* replyData);

#endif
