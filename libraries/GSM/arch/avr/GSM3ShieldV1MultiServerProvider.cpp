#include <GSM3ShieldV1MultiServerProvider.h>
#include <GSM3ShieldV1ModemCore.h>
#include <Arduino.h>

#define __NCLIENTS_MAX__ 3

char _command_QILOCIP[] PROGMEM = "AT+QILOCIP";

GSM3ShieldV1MultiServerProvider::GSM3ShieldV1MultiServerProvider()
{
	theGSM3MobileServerProvider=this;
	socketsAsServer=0;
	socketsAccepted=0;
	theGSM3ShieldV1ModemCore.registerUMProvider(this);
};

//Response management.
void GSM3ShieldV1MultiServerProvider::manageResponse(byte from, byte to)
{
	switch(theGSM3ShieldV1ModemCore.getOngoingCommand())
	{
		case NONE:
			theGSM3ShieldV1ModemCore.gss.cb.deleteToTheEnd(from);
			break;
		case CONNECTSERVER:
			connectTCPServerContinue();
			break;	
		case GETIP:
			getIPContinue();
			break;
	}
}

//Connect Server main function.
int GSM3ShieldV1MultiServerProvider::connectTCPServer(int port)
{
	// We forget about LocalIP as it has no real use, the modem does whatever it likes
	theGSM3ShieldV1ModemCore.setPort(port);			
	theGSM3ShieldV1ModemCore.openCommand(this,CONNECTSERVER);
	connectTCPServerContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Connect Server continue function.
void GSM3ShieldV1MultiServerProvider::connectTCPServerContinue()
{

	bool resp;
	// 1: Read Local IP "AT+QILOCIP"
	// 2: Waiting for IP and Set local port "AT+QILPORT"
	// 3: Waiting for QILPOR OK andConfigure as server "AT+QISERVER"
	// 4: Wait for SERVER OK

	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		//"AT+QILOCIP."
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_QILOCIP);
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
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QISERVER=0,"),false);
				theGSM3ShieldV1ModemCore.print(__NCLIENTS_MAX__);
				theGSM3ShieldV1ModemCore.print('\r');
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
			if (resp) theGSM3ShieldV1ModemCore.closeCommand(1);
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}		
		break;	
	}
}

//QILOCIP parse.
bool GSM3ShieldV1MultiServerProvider::parseQILOCIP_rsp(char* LocalIP, int LocalIPlength, bool& rsp)
{
	if (!(theGSM3ShieldV1ModemCore.theBuffer().extractSubstring("\r\n","\r\n", LocalIP, LocalIPlength)))
		rsp = false;
	else 
		rsp = true;
	return true;
}

//Get IP main function.
int GSM3ShieldV1MultiServerProvider::getIP(char* LocalIP, int LocalIPlength)
{
	theGSM3ShieldV1ModemCore.setPhoneNumber(LocalIP);
	theGSM3ShieldV1ModemCore.setPort(LocalIPlength);
	theGSM3ShieldV1ModemCore.openCommand(this,GETIP);
	getIPContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

void GSM3ShieldV1MultiServerProvider::getIPContinue()
{

	bool resp;
	// 1: Read Local IP "AT+QILOCIP"
	// 2: Waiting for IP.

	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		//AT+QILOCIP
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_QILOCIP);
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
		break;	
	}
}

bool GSM3ShieldV1MultiServerProvider::getSocketAsServerModemStatus(int s)
{
	if (socketsAccepted&(0x0001<<s)) 
		return true;
	else return false;
}


//URC recognize.
bool GSM3ShieldV1MultiServerProvider::recognizeUnsolicitedEvent(byte oldTail)
{

	int nlength;
	char auxLocate [15];
	
	
	//REMOTE SOCKET CLOSED.
	prepareAuxLocate(PSTR("0, CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		releaseSocket(0);
		socketsAccepted &= ~(0x0001);
		//Serial.println("JCR_DB REMOTE CLOSED");
	}
	
	//REMOTE SOCKET CLOSED.
	
	prepareAuxLocate(PSTR("1, CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		releaseSocket(1);
		socketsAccepted &= ~(0x0002);
	}
	
	//REMOTE SOCKET CLOSED.
	prepareAuxLocate(PSTR("2, CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		releaseSocket(2);
		socketsAccepted &= ~(0x0004);
	}
	
	//REMOTE SOCKET CLOSED.
	prepareAuxLocate(PSTR("3, CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		releaseSocket(3);
		socketsAccepted &= ~(0x0008);
	}
	
	//REMOTE SOCKET CLOSED.
	prepareAuxLocate(PSTR("4, CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		releaseSocket(4);
		socketsAccepted &= ~(0x0010);
	}
	
	//REMOTE SOCKET CLOSED.
	prepareAuxLocate(PSTR("5, CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		releaseSocket(5);
		socketsAccepted &= ~(0x0020);
	}
	
	//REMOTE SOCKET CLOSED.
	prepareAuxLocate(PSTR("6, CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		releaseSocket(6);
		socketsAccepted &= ~(0x0040);
	}
	
	//REMOTE SOCKET CLOSED.
	prepareAuxLocate(PSTR("7, CLOSED\r\n"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		releaseSocket(7);
		socketsAccepted &= ~(0x0080);
	}
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("0, REMOTE IP"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		socketsAccepted |= (0x0001);
		return true;
	}
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("1, REMOTE IP"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		socketsAccepted |= (0x0002);
		return true;
	}
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("2, REMOTE IP"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		socketsAccepted |= (0x0004);
		return true;
	}
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("3, REMOTE IP"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		socketsAccepted |= (0x0008);
		return true;
	}
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("4, REMOTE IP"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		socketsAccepted |= (0x0010);
		return true;
	}
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("5, REMOTE IP"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		socketsAccepted |= (0x0020);
		return true;
	}
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("6, REMOTE IP"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		socketsAccepted |= (0x0040);
		return true;
	}
	
	//REMOTE SOCKET ACCEPTED.
	prepareAuxLocate(PSTR("7, REMOTE IP"), auxLocate);
	if(theGSM3ShieldV1ModemCore.gss.cb.locate(auxLocate))
	{
		//To detect remote socket closed for example inside socket data.
		theGSM3ShieldV1ModemCore.gss.cb.flush();
		socketsAccepted |= (0x0080);
		return true;
	}
	
	
	return false;
}

bool GSM3ShieldV1MultiServerProvider::getStatusSocketAsServer(uint8_t socket)
{
	if(socketsAsServer&(0x0001<<socket))
		return 1;
	else
		return 0;
};

void GSM3ShieldV1MultiServerProvider::releaseSocket(int socket)
{
	if (socketsAsServer&((0x0001)<<socket))
		socketsAsServer^=((0x0001)<<socket);
}

int GSM3ShieldV1MultiServerProvider::getNewOccupiedSocketAsServer()
{
	int i;
	ready();
	for(i=minSocketAsServer(); i<=maxSocketAsServer(); i++)
	{
		if ((!(socketsAsServer&(0x0001<<i))) && getSocketAsServerModemStatus(i))
		{
			socketsAsServer|=((0x0001)<<i);	
			return i;
		}
	}
	// No new occupied
	return -1;
}
