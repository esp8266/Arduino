/*
This file is part of the GSM3 communications library for Arduino
-- Multi-transport communications platform
-- Fully asynchronous
-- Includes code for the Arduino-Telefonica GSM/GPRS Shield V1
-- Voice calls
-- SMS
-- TCP/IP connections
-- HTTP basic clients

This library has been developed by Telefónica Digital - PDI -
- Physical Internet Lab, as part as its collaboration with
Arduino and the Open Hardware Community. 

September-December 2012

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
https://github.com/BlueVia/Official-Arduino
*/
#ifndef _GSM3SHIELDV1BASEPROVIDER_
#define _GSM3SHIELDV1BASEPROVIDER_

#include <GSM3SoftSerial.h>

enum GSM3_commandType_e { XON, NONE, MODEMCONFIG, ALIVETEST, BEGINSMS, ENDSMS, AVAILABLESMS, FLUSHSMS,
	VOICECALL, ANSWERCALL, HANGCALL, RETRIEVECALLINGNUMBER, 
	ATTACHGPRS, DETACHGPRS, CONNECTTCPCLIENT, DISCONNECTTCP, BEGINWRITESOCKET, ENDWRITESOCKET, 
	AVAILABLESOCKET, FLUSHSOCKET, CONNECTSERVER, GETIP, GETCONNECTSTATUS, GETLOCATION, GETICCID}; 

class GSM3ShieldV1BaseProvider
{
	public:

	/** Get last command status
		@return Returns 0 if last command is still executing, 1 success, >1 error
	*/
	int ready();

	/** This function locates strings from PROGMEM in the buffer
		@param str			PROGMEN
		@param auxLocate	Buffer where to locate strings
	*/
	void prepareAuxLocate(PGM_P str, char auxLocate[]);

	/** Manages modem response
		@param from 		Initial byte of buffer
		@param to 			Final byte of buffer
	*/
	virtual void manageResponse(byte from, byte to);
	
	/** Recognize URC
		@param from		
		@return true if successful (default: false)
	*/		
	virtual bool recognizeUnsolicitedEvent(byte from){return false;};

};

#endif
