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
#include <GSM3ShieldV1BaseProvider.h>
#include <GSM3ShieldV1ModemCore.h>
#include <Arduino.h>

// Returns 0 if last command is still executing
// 1 if success
// >1 if error 
int GSM3ShieldV1BaseProvider::ready() 
{
	theGSM3ShieldV1ModemCore.manageReceivedData();

	return theGSM3ShieldV1ModemCore.getCommandError();
};

void GSM3ShieldV1BaseProvider::prepareAuxLocate(PGM_P str, char auxLocate[])
{
	int i=0;
	char c;

	do
	{	
		c=pgm_read_byte_near(str + i); 
		auxLocate[i]=c;
		i++;
	} while (c!=0);
}

