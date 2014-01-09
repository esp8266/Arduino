#include <GSM3ShieldV1MultiClientProvider.h>
#include <GSM3ShieldV1ModemCore.h>

char _command_MultiQISRVC[] PROGMEM = "AT+QISRVC=";

#define __TOUTFLUSH__ 10000

GSM3ShieldV1MultiClientProvider::GSM3ShieldV1MultiClientProvider()
{
	theGSM3MobileClientProvider=this;
	theGSM3ShieldV1ModemCore.registerUMProvider(this);
};

//Response management.
void GSM3ShieldV1MultiClientProvider::manageResponse(byte from, byte to)
{
	switch(theGSM3ShieldV1ModemCore.getOngoingCommand())
	{
		case XON:
			if (flagReadingSocket) 
				{
//					flagReadingSocket = 0;
					fullBufferSocket = (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()<3);
				}
			else theGSM3ShieldV1ModemCore.setOngoingCommand(NONE);
			break;
		case NONE:
			theGSM3ShieldV1ModemCore.gss.cb.deleteToTheEnd(from);
			break;
		case CONNECTTCPCLIENT:
			connectTCPClientContinue();
			break;
		case DISCONNECTTCP:
			disconnectTCPContinue();
			break;	
	 	case BEGINWRITESOCKET:
			beginWriteSocketContinue();
			break;
		case ENDWRITESOCKET:
			endWriteSocketContinue();
			break;	
		case AVAILABLESOCKET:
			availableSocketContinue();
			break;	
		case FLUSHSOCKET:
			fullBufferSocket = (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()<3);
			flushSocketContinue();
			break;	
	}
}

//Connect TCP main function.
int GSM3ShieldV1MultiClientProvider::connectTCPClient(const char* server, int port, int id_socket)
{
	theGSM3ShieldV1ModemCore.setPort(port);		
	idSocket = id_socket;
	
	theGSM3ShieldV1ModemCore.setPhoneNumber((char*)server);
	theGSM3ShieldV1ModemCore.openCommand(this,CONNECTTCPCLIENT);
	connectTCPClientContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();	
}

int GSM3ShieldV1MultiClientProvider::connectTCPClient(IPAddress add, int port, int id_socket)
{
	remoteIP=add;
	theGSM3ShieldV1ModemCore.setPhoneNumber(0);
	return connectTCPClient(0, port, id_socket);
}

//Connect TCP continue function.
void GSM3ShieldV1MultiClientProvider::connectTCPClientContinue()
{
	bool resp;
	// 0: Dot or DNS notation activation
	// 1: Disable SW flow control 
	// 2: Waiting for IFC OK
	// 3: Start-up TCP connection "AT+QIOPEN"
	// 4: Wait for connection OK
	// 5: Wait for CONNECT

	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QIDNSIP="), false);
		if ((theGSM3ShieldV1ModemCore.getPhoneNumber()!=0)&&
			((*(theGSM3ShieldV1ModemCore.getPhoneNumber())<'0')||((*(theGSM3ShieldV1ModemCore.getPhoneNumber())>'9'))))
		{
			theGSM3ShieldV1ModemCore.print('1');
			theGSM3ShieldV1ModemCore.print('\r');
		}
		else 
		{
			theGSM3ShieldV1ModemCore.print('0');
			theGSM3ShieldV1ModemCore.print('\r');
		}
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			//Response received
			if(resp)
			{				
				// AT+QIOPEN
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QIOPEN="),false);
				theGSM3ShieldV1ModemCore.print(idSocket);
				theGSM3ShieldV1ModemCore.print(",\"TCP\",\"");
				if(theGSM3ShieldV1ModemCore.getPhoneNumber()!=0)
				{
					theGSM3ShieldV1ModemCore.print(theGSM3ShieldV1ModemCore.getPhoneNumber());
				}
				else
				{
					remoteIP.printTo(theGSM3ShieldV1ModemCore);
				}
				theGSM3ShieldV1ModemCore.print('"');
				theGSM3ShieldV1ModemCore.print(',');
				theGSM3ShieldV1ModemCore.print(theGSM3ShieldV1ModemCore.getPort());
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
				// OK Received
				// Great. Go for the next step
				theGSM3ShieldV1ModemCore.setCommandCounter(4);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}	
		break;
	case 4:
		char auxLocate [12];
		prepareAuxLocate(PSTR("CONNECT OK"), auxLocate);
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp,auxLocate))
	    {
			// Response received
			if(resp)
			{
				// Received CONNECT OK
				// Great. We're done
				theGSM3ShieldV1ModemCore.closeCommand(1);
			}
			else 
				theGSM3ShieldV1ModemCore.closeCommand(3);
		}		
		break;
		
	}
}

//Disconnect TCP main function.
int GSM3ShieldV1MultiClientProvider::disconnectTCP(bool client1Server0, int id_socket)
{		
	idSocket = id_socket;
	
	// First of all, we will flush the socket synchronously
	unsigned long m;
	m=millis();
	flushSocket();
	while(((millis()-m)< __TOUTFLUSH__ )&&(ready()==0)) 
		delay(10);
		
	// Could not flush the communications... strange
	if(ready()==0)
	{
		theGSM3ShieldV1ModemCore.setCommandError(2);
		return theGSM3ShieldV1ModemCore.getCommandError();
	}
		
	// Set up the command
	client1_server0 = client1Server0;
	flagReadingSocket=0;
	theGSM3ShieldV1ModemCore.openCommand(this,DISCONNECTTCP);
	disconnectTCPContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Disconnect TCP continue function
void GSM3ShieldV1MultiClientProvider::disconnectTCPContinue()
{
	bool resp;
	// 1: Send AT+QISRVC
	// 2: "AT+QICLOSE"
	// 3: Wait for OK
	
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_MultiQISRVC, false);
		if (client1_server0) theGSM3ShieldV1ModemCore.print('1');
		else theGSM3ShieldV1ModemCore.print('2');
		theGSM3ShieldV1ModemCore.print('\r');
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		// Parse response to QISRVC
		theGSM3ShieldV1ModemCore.genericParse_rsp(resp);
		if(resp)
		{
			// Send QICLOSE command
			theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QICLOSE="),false);
			theGSM3ShieldV1ModemCore.print(idSocket);
			theGSM3ShieldV1ModemCore.print('\r');
			theGSM3ShieldV1ModemCore.setCommandCounter(3);
		}
		else 
			theGSM3ShieldV1ModemCore.closeCommand(3);
		break;
	case 3:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {		
			theGSM3ShieldV1ModemCore.setCommandCounter(0);
			if (resp) 
				theGSM3ShieldV1ModemCore.closeCommand(1);
			else 
				theGSM3ShieldV1ModemCore.closeCommand(3);
		}	
		break;
	}
}

//Write socket first chain main function.
void GSM3ShieldV1MultiClientProvider::beginWriteSocket(bool client1Server0, int id_socket)
{
	idSocket = id_socket;	
	client1_server0 = client1Server0;
	theGSM3ShieldV1ModemCore.openCommand(this,BEGINWRITESOCKET);
	beginWriteSocketContinue();
}

//Write socket first chain continue function.
void GSM3ShieldV1MultiClientProvider::beginWriteSocketContinue()
{
	bool resp;
	// 1: Send AT+QISRVC
	// 2: Send AT+QISEND
	// 3: wait for > and Write text
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		// AT+QISRVC
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_MultiQISRVC, false);
		if (client1_server0) 
			theGSM3ShieldV1ModemCore.print('1');
		else 
			theGSM3ShieldV1ModemCore.print('2');
		theGSM3ShieldV1ModemCore.print('\r');
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
    case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			// Response received
			if(resp)
			{
				// AT+QISEND
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QISEND="), false);
				theGSM3ShieldV1ModemCore.print(idSocket);
				theGSM3ShieldV1ModemCore.print('\r');
				theGSM3ShieldV1ModemCore.setCommandCounter(3);
			}
			else
			{
				theGSM3ShieldV1ModemCore.closeCommand(3);
			}
		}	
		break;
	case 3:
		char aux[2];
		aux[0]='>';
		aux[1]=0;
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp, aux))
		{
			if(resp)
			{
				// Received ">"
				theGSM3ShieldV1ModemCore.closeCommand(1);
			}
			else
			{
				theGSM3ShieldV1ModemCore.closeCommand(3);
			}
		}
		break;
	}
}

//Write socket next chain function.
void GSM3ShieldV1MultiClientProvider::writeSocket(const char* buf)
{
	theGSM3ShieldV1ModemCore.print(buf);
}

//Write socket character function.
void GSM3ShieldV1MultiClientProvider::writeSocket(char c)
{
	theGSM3ShieldV1ModemCore.print(c);
}

//Write socket last chain main function.
void GSM3ShieldV1MultiClientProvider::endWriteSocket()
{		
	theGSM3ShieldV1ModemCore.openCommand(this,ENDWRITESOCKET);
	endWriteSocketContinue();
}

//Write socket last chain continue function.
void GSM3ShieldV1MultiClientProvider::endWriteSocketContinue()
{
	bool resp;
	// 1: Write text (ctrl-Z)
	// 2: Wait for OK
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		theGSM3ShieldV1ModemCore.write(26); // Ctrl-Z
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			// OK received
			if (resp) theGSM3ShieldV1ModemCore.closeCommand(1);
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
		break;
	}
}

//Available socket main function.
int GSM3ShieldV1MultiClientProvider::availableSocket(bool client1Server0, int id_socket)
{
	if(flagReadingSocket==1)
	{
		theGSM3ShieldV1ModemCore.setCommandError(1);
		return 1;
	}
	client1_server0 = client1Server0;
	idSocket = id_socket;	
	theGSM3ShieldV1ModemCore.openCommand(this,AVAILABLESOCKET);
	availableSocketContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Available socket continue function.
void GSM3ShieldV1MultiClientProvider::availableSocketContinue()
{
	bool resp;
	// 1: AT+QIRD
	// 2: Wait for OK and Next necessary AT+QIRD

	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QIRD=0,"),false);
		if (client1_server0) 
			theGSM3ShieldV1ModemCore.print('1');
		else 
			theGSM3ShieldV1ModemCore.print('2');
		theGSM3ShieldV1ModemCore.print(',');
		theGSM3ShieldV1ModemCore.print(idSocket);
		theGSM3ShieldV1ModemCore.print(",1500");
		// theGSM3ShieldV1ModemCore.print(",120");
		theGSM3ShieldV1ModemCore.print('\r');
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(parseQIRD_head(resp))
		{
			if (!resp)
			{
				theGSM3ShieldV1ModemCore.closeCommand(4);
			}
			else 
			{
				flagReadingSocket=1;
				theGSM3ShieldV1ModemCore.closeCommand(1);
			}
		}
		else 
		{
			theGSM3ShieldV1ModemCore.closeCommand(3);	
		}
		break;
	}
}
	
//Read Socket Parse head.
bool GSM3ShieldV1MultiClientProvider::parseQIRD_head(bool& rsp)
{
	char _qird [8];
	prepareAuxLocate(PSTR("+QIRD:"), _qird);
	fullBufferSocket = (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()<3);
	if(theGSM3ShieldV1ModemCore.theBuffer().locate(_qird)) 
	{		
		theGSM3ShieldV1ModemCore.theBuffer().chopUntil(_qird, true);
		// Saving more memory, reuse _qird
		_qird[0]='\n';
		_qird[1]=0;
		theGSM3ShieldV1ModemCore.theBuffer().chopUntil(_qird, true);
		rsp = true;			
		return true;
	}
	else if(theGSM3ShieldV1ModemCore.theBuffer().locate("OK")) 
	{
		rsp = false;
		return true;
	}
	else
	{
		rsp = false;
		return false;
	}
}
/*		
//Read socket main function.
int GSM3ShieldV1MultiClientProvider::readSocket()
{
	char charSocket;
	charSocket = theGSM3ShieldV1ModemCore.theBuffer().read(); 
	//Case buffer not full
	if (!fullBufferSocket)
	{	
		//The last part of the buffer after data is CRLFOKCRLF
		if (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()==125)
		{
			//Start again availableSocket function.
			flagReadingSocket=0;
			theGSM3ShieldV1ModemCore.openCommand(this,AVAILABLESOCKET);
			availableSocketContinue();					
		}
	}
	else if (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()==127)
	{
		// The buffer is full, no more action is possible until we have read()
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		flagReadingSocket = 1;
		theGSM3ShieldV1ModemCore.openCommand(this,XON);
		theGSM3ShieldV1ModemCore.gss.spaceAvailable();
		//A small delay to assure data received after xon.
		delay(10);
	}
	//To distinguish the case no more available data in socket.			
	if (ready()==1)	
		return charSocket;
	else 
		return 0;
}	
*/
int GSM3ShieldV1MultiClientProvider::readSocket()
{
	char charSocket;
	
	if(theGSM3ShieldV1ModemCore.theBuffer().availableBytes()==0)
	{
		Serial.println();Serial.println("*");
		return 0;
	}
		
	charSocket = theGSM3ShieldV1ModemCore.theBuffer().read(); 
	//Case buffer not full
	if (!fullBufferSocket)
	{	
		//The last part of the buffer after data is CRLFOKCRLF
		if (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()==125)
		{
			//Start again availableSocket function.
			flagReadingSocket=0;
			theGSM3ShieldV1ModemCore.openCommand(this,AVAILABLESOCKET);
			availableSocketContinue();					
		}
	}
	else if (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()>=100)
	{
		// The buffer was full, we have to let the data flow again
		// theGSM3ShieldV1ModemCore.theBuffer().flush();
		flagReadingSocket = 1;
		theGSM3ShieldV1ModemCore.openCommand(this,XON);
		theGSM3ShieldV1ModemCore.gss.spaceAvailable();
		//A small delay to assure data received after xon.
		delay(100);
		if(theGSM3ShieldV1ModemCore.theBuffer().availableBytes() >=6)
			fullBufferSocket=false;
	}

	return charSocket;

}

//Read socket main function.
int GSM3ShieldV1MultiClientProvider::peekSocket()
{
	return theGSM3ShieldV1ModemCore.theBuffer().peek(0); 
}


//Flush SMS main function.
void GSM3ShieldV1MultiClientProvider::flushSocket()
{
	flagReadingSocket=0;
	theGSM3ShieldV1ModemCore.openCommand(this,FLUSHSOCKET);
	flushSocketContinue();
}

//Send SMS continue function.
void GSM3ShieldV1MultiClientProvider::flushSocketContinue()
{
	bool resp;
	// 1: Deleting SMS
	// 2: wait for OK
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:
		//DEBUG
		//Serial.println("Flushing Socket.");	
			theGSM3ShieldV1ModemCore.theBuffer().flush();
			if (fullBufferSocket) 
				{
					//Serial.println("Buffer flushed.");
					theGSM3ShieldV1ModemCore.gss.spaceAvailable();
				}
			else 
				{
					//Serial.println("Socket flushed completely.");
					theGSM3ShieldV1ModemCore.closeCommand(1);
				}
		break;
	}
}

//URC recognize.
// Momentarily, we will not recognize "closes" in client mode
bool GSM3ShieldV1MultiClientProvider::recognizeUnsolicitedEvent(byte oldTail)
{
	return false;
}

int GSM3ShieldV1MultiClientProvider::getSocket(int socket)
{
	if(socket==-1)
	{
		int i;
		for(i=minSocket(); i<=maxSocket(); i++)
		{
			if (!(sockets&(0x0001<<i)))
			{
				sockets|=((0x0001)<<i);
				return i;
			}	
		}
	}
	else
	{
		if (!(sockets&(0x0001<<socket)))
		{
			sockets|=((0x0001)<<socket);
			return socket;
		}	
	}
	return -1;
}

void GSM3ShieldV1MultiClientProvider::releaseSocket(int socket)
{
	if (sockets&((0x0001)<<socket))
		sockets^=((0x0001)<<socket);
}

bool GSM3ShieldV1MultiClientProvider::getStatusSocketClient(uint8_t socket)
{
	if(socket>8)
		return 0;
	if(sockets&(0x0001<<socket))
		return 1;
	else
		return 0;
};



