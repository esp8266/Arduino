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
#include <GSM3ShieldV1ServerProvider.h>
#include <GSM3ShieldV1ModemCore.h>
#include <Arduino.h>

GSM3ShieldV1ServerProvider::GSM3ShieldV1ServerProvider()
{
	theGSM3MobileServerProvider=this;
};

//Response management.
void GSM3ShieldV1ServerProvider::manageResponse(byte from, byte to)
{
	switch(theGSM3ShieldV1ModemCore.getOngoingCommand())
	{
		case NONE:
			theGSM3ShieldV1ModemCore.gss.cb.deleteToTheEnd(from);
			break;
		case CONNECTSERVER:
			connectTCPServerContinue();
			break;	
		/*case GETIP:
			getIPContinue();
			break;*/
	}
}

//Connect Server main function.
int GSM3ShieldV1ServerProvider::connectTCPServer(int port)
{
	// We forget about LocalIP as it has no real use, the modem does whatever it likes
	theGSM3ShieldV1ModemCore.setPort(port);			
	theGSM3ShieldV1ModemCore.openCommand(this,CONNECTSERVER);
	// From this moment on we wait for a call
	connectTCPServerContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Connect Server continue function.
void GSM3ShieldV1ServerProvider::connectTCPServerContinue()
{

	bool resp;
	// 1: Read Local IP "AT+QILOCIP"
	// 2: Waiting for IP and Set local port "AT+QILPORT"
	// 3: Waiting for QILPOR OK andConfigure as server "AT+QISERVER"
	// 4: Wait for SERVER OK

	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		//"AT+QILOCIP."
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QILOCIP"));
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		//Not IP storing but the command is necessary.
		//if(parseQILOCIP_rsp(local_IP, local_IP_Length, resp))
		// This awful trick saves some RAM bytes
		char aux[3];
		aux[0]='\r';aux[1]='\n';aux[2]=0;
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp, aux))
	    {
			//Response received
			if(resp)
			{
				// Great. Go for the next step
				// AT+QILPORT
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QILPORT=\"TCP\","),false);
				theGSM3ShieldV1ModemCore.print(	theGSM3ShieldV1ModemCore.getPort());
				theGSM3ShieldV1ModemCore.print('\r');
				theGSM3ShieldV1ModemCore.setCommandCounter(3);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}	
		break;	
	case 3:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			// Response received
			if(resp)
			{
				// OK received
				// Great. Go for the next step
				// AT+QISERVER
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QISERVER"),true);
				theGSM3ShieldV1ModemCore.setCommandCounter(4);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}	
		break;	
	case 4:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			// Response received
			// OK received, kathapoon, chessespoon
			if (resp) 
			{
				theGSM3ShieldV1ModemCore.registerUMProvider(this);
				theGSM3ShieldV1ModemCore.closeCommand(1);
			}
			else 
				theGSM3ShieldV1ModemCore.closeCommand(3);
		}		
		break;	
	}
}

//QILOCIP parse.
/*bool GSM3ShieldV1ServerProvider::parseQILOCIP_rsp(char* LocalIP, int LocalIPlength, bool& rsp)
{
	if (!(theGSM3ShieldV1ModemCore.theBuffer().extractSubstring("\r\n","\r\n", LocalIP, LocalIPlength)))
		rsp = false;
	else 
		rsp = true;
	return true;
}

//Get IP main function.
int GSM3ShieldV1ServerProvider::getIP(char* LocalIP, int LocalIPlength)
{
	theGSM3ShieldV1ModemCore.setPhoneNumber(LocalIP);
	theGSM3ShieldV1ModemCore.setPort(LocalIPlength);
	theGSM3ShieldV1ModemCore.openCommand(this,GETIP);
	getIPContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

void GSM3ShieldV1ServerProvider::getIPContinue()
{

	bool resp;
	// 1: Read Local IP "AT+QILOCIP"
	// 2: Waiting for IP.

	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		//AT+QILOCIP
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_MonoQILOCIP);
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(parseQILOCIP_rsp(theGSM3ShieldV1ModemCore.getPhoneNumber(), theGSM3ShieldV1ModemCore.getPort(), resp))
	    {
			if (resp) 
				theGSM3ShieldV1ModemCore.closeCommand(1);
			else 
				theGSM3ShieldV1ModemCore.closeCommand(3);
		}
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		theGSM3ShieldV1ModemCore.gss.spaceAvailable();
		break;	
	}
}*/

bool GSM3ShieldV1ServerProvider::getSocketAsServerModemStatus(int s)
{
	if(theGSM3ShieldV1ModemCore.getStatus()==TRANSPARENT_CONNECTED)
		return true;
	else 
		return false;
}


//URC recognize.
bool GSM3ShieldV1ServerProvider::recognizeUnsolicitedEvent(byte oldTail)
{

	int nlength;
	char auxLocate [15];
	
	//REMOTE SOCKET CLOSED.
	prepareAuxLocate(PSTR("CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.setStatus(GPRS_READY);
	}
	
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("CONNECT\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.theBuffer().chopUntil(auxLocate, true);
		theGSM3ShieldV1ModemCore.gss.spaceAvailable();
		theGSM3ShieldV1ModemCore.setStatus(TRANSPARENT_CONNECTED);
		return true;
	}
	
	return false;
}

bool GSM3ShieldV1ServerProvider::getStatusSocketAsServer(uint8_t socket)
{
	return(theGSM3ShieldV1ModemCore.getStatus()==TRANSPARENT_CONNECTED);
};

void GSM3ShieldV1ServerProvider::releaseSocket(int socket)
{
}

int GSM3ShieldV1ServerProvider::getNewOccupiedSocketAsServer()
{
	return 0;
}
