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
#include <GSM3ShieldV1.h>
#include <HardwareSerial.h>
#include <Arduino.h>

#define __RESETPIN__ 7
#define __TOUTLOCALCOMS__ 500
#define __TOUTSHUTDOWN__ 5000
#define __TOUTMODEMCONFIGURATION__ 5000//equivalent to 30000 because of time in interrupt routine.
#define __TOUTAT__ 1000
#define __TOUTSMS__ 7000
#define __TOUTCALL__ 15000
#define __TOUTGPRS__ 10000
#define __NCLIENTS_MAX__ 3

//Constructor.
GSM3ShieldV1::GSM3ShieldV1(bool db) 
{
	theGSM3ShieldV1ModemCore.setCommandCounter(1);
	socketsAccepted=0;
	theGSM3ShieldV1ModemCore.registerUMProvider(this);
	theProvider=this;
}

//Response management.
void GSM3ShieldV1::manageResponse(byte from, byte to)
{
	switch(theGSM3ShieldV1ModemCore.getOngoingCommand())
	{
		case NONE:
			theGSM3ShieldV1ModemCore.gss.cb.deleteToTheEnd(from);
			break;

	}
}

//Function for 2 sec delay inside an interruption.
void GSM3ShieldV1::delayInsideInterrupt2seg()
{
	for (int k=0;k<40;k++)  theGSM3ShieldV1ModemCore.gss.tunedDelay(50000); 
} 

///////////////////////////////////////////////////////UNSOLICITED RESULT CODE (URC) FUNCTIONS///////////////////////////////////////////////////////////////////

//URC recognize.
bool GSM3ShieldV1::recognizeUnsolicitedEvent(byte oldTail)
{

int nlength;
char auxLocate [15];
	//POWER DOWN.
	prepareAuxLocate(PSTR("POWER DOWN"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		return true;
	}
	
	
	return false;
}



