/*! \file stxetxconf.h \brief STX/ETX Packet Protocol Implementation Configuration. */
//*****************************************************************************
//
// File Name	: 'stxetx.h'
// Title		: STX/ETX Packet Protocol Implementation Configuration
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 10/9/2002
// Revised		: 02/10/2003
// Version		: 0.1
// Target MCU	: any
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef STXETXCONF_H
#define STXETXCONF_H

// STX/ETX Configuration Options

// This determines the size of the Packet Receive Buffer
// where whole verified packets are copied after being received
#define STXETX_MAXRXPACKETSIZE	20	// length of packet buffer


#endif
