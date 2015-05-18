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
#include <GSM3ShieldV1VoiceProvider.h>
#include <Arduino.h>

GSM3ShieldV1VoiceProvider::GSM3ShieldV1VoiceProvider()
 {
	phonelength=0;
	theGSM3MobileVoiceProvider=this;
 }
 
 void GSM3ShieldV1VoiceProvider::initialize()
 {
 	theGSM3ShieldV1ModemCore.registerUMProvider(this);
 }
 
//Voice Call main function.
int GSM3ShieldV1VoiceProvider::voiceCall(const char* to)
{
	theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("ATD"),false);
	theGSM3ShieldV1ModemCore.print(to);
	theGSM3ShieldV1ModemCore.print(";\r");
	setvoiceCallStatus(CALLING);
	return 1;
}

//Retrieve calling number main function.
int GSM3ShieldV1VoiceProvider::retrieveCallingNumber (char* buffer, int bufsize)
{
	theGSM3ShieldV1ModemCore.setPhoneNumber(buffer);
	phonelength = bufsize;
	theGSM3ShieldV1ModemCore.setCommandError(0);
	theGSM3ShieldV1ModemCore.setCommandCounter(1);
	theGSM3ShieldV1ModemCore.openCommand(this,RETRIEVECALLINGNUMBER);
	retrieveCallingNumberContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Retrieve calling number Continue function.
void GSM3ShieldV1VoiceProvider::retrieveCallingNumberContinue()
{
	// 1:  AT+CLCC
	// 2: Receive +CLCC: 1,1,4,0,0,"num",129,""
	// This implementation really does not care much if the modem aswers trash to CMGL
	bool resp;
	//int msglength_aux;
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:	
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+CLCC"));
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(parseCLCC(theGSM3ShieldV1ModemCore.getPhoneNumber(), phonelength))
		{
			theGSM3ShieldV1ModemCore.closeCommand(1);
		}
		break;
	}	
}	

//CLCC parse.	
bool GSM3ShieldV1VoiceProvider::parseCLCC(char* number, int nlength)
{
	theGSM3ShieldV1ModemCore.theBuffer().extractSubstring("+CLCC: 1,1,4,0,0,\"","\"", number, nlength);
	theGSM3ShieldV1ModemCore.theBuffer().flush();
	return true;
}	

//Answer Call main function.
int GSM3ShieldV1VoiceProvider::answerCall()
{
	theGSM3ShieldV1ModemCore.setCommandError(0);
	theGSM3ShieldV1ModemCore.setCommandCounter(1);
	theGSM3ShieldV1ModemCore.openCommand(this,ANSWERCALL);
	answerCallContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Answer Call continue function.
void GSM3ShieldV1VoiceProvider::answerCallContinue()
{
	// 1: ATA
	// 2: Waiting for OK
	
	// This implementation really does not care much if the modem aswers trash to CMGL
	bool resp;
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:
		// ATA ;
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("ATA"));
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		   {
			   setvoiceCallStatus(TALKING);
			   if (resp) theGSM3ShieldV1ModemCore.closeCommand(1);
			   else theGSM3ShieldV1ModemCore.closeCommand(3);
			}
		break;
	}
}
		
//Hang Call main function.		
int GSM3ShieldV1VoiceProvider::hangCall()
{
	theGSM3ShieldV1ModemCore.setCommandError(0);
	theGSM3ShieldV1ModemCore.setCommandCounter(1);
	theGSM3ShieldV1ModemCore.openCommand(this,HANGCALL);
	hangCallContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Hang Call continue function.
void GSM3ShieldV1VoiceProvider::hangCallContinue()
{
	// 1: ATH
	// 2: Waiting for OK
	
	bool resp;
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:
		//ATH
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("ATH"));
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
		   setvoiceCallStatus(IDLE_CALL);
		   if (resp) theGSM3ShieldV1ModemCore.closeCommand(1);
		   else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
		break;
	}
}		

//Response management.
void GSM3ShieldV1VoiceProvider::manageResponse(byte from, byte to)
{
	switch(theGSM3ShieldV1ModemCore.getOngoingCommand())
	{
		case ANSWERCALL:
			answerCallContinue();
			break;
		case HANGCALL:
			hangCallContinue();
			break;
		case RETRIEVECALLINGNUMBER:
			retrieveCallingNumberContinue();
			break;	

	}
}

//URC recognize.
bool GSM3ShieldV1VoiceProvider::recognizeUnsolicitedEvent(byte oldTail)
{

	int nlength;
	char auxLocate [15];
	//RING.
	prepareAuxLocate(PSTR("RING"), auxLocate);
	if(theGSM3ShieldV1ModemCore.theBuffer().locate(auxLocate))
	{
		// RING
		setvoiceCallStatus(RECEIVINGCALL);
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		return true;
	}
	
	//CALL ACEPTED.
	prepareAuxLocate(PSTR("+COLP:"), auxLocate);
	if(theGSM3ShieldV1ModemCore.theBuffer().locate(auxLocate))
	{
		//DEBUG
		//Serial.println("Call Accepted.");
		setvoiceCallStatus(TALKING);
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		return true;
	}	
	
	//NO CARRIER.
	prepareAuxLocate(PSTR("NO CARRIER"), auxLocate);
	if(theGSM3ShieldV1ModemCore.theBuffer().locate(auxLocate))
	{
		//DEBUG
		//Serial.println("NO CARRIER received.");
		setvoiceCallStatus(IDLE_CALL);
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		return true;
	}
	
	//BUSY.
	prepareAuxLocate(PSTR("BUSY"), auxLocate);
	if(theGSM3ShieldV1ModemCore.theBuffer().locate(auxLocate))
	{
		//DEBUG	
		//Serial.println("BUSY received.");
		setvoiceCallStatus(IDLE_CALL);
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		return true;
	}	
	
	//CALL RECEPTION.
	prepareAuxLocate(PSTR("+CLIP:"), auxLocate);
	if(theGSM3ShieldV1ModemCore.theBuffer().locate(auxLocate))
	{
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		setvoiceCallStatus(RECEIVINGCALL);
		return true;
	}
	
	return false;
}


