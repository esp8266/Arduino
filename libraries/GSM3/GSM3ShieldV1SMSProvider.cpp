#include <GSM3ShieldV1SMSProvider.h>
#include <Arduino.h>
	
GSM3ShieldV1SMSProvider::GSM3ShieldV1SMSProvider()
{
	theGSM3SMSProvider=this;
};

//Send SMS begin function.
int GSM3ShieldV1SMSProvider::beginSMS(const char* to)
{
	if((theGSM3ShieldV1ModemCore.getStatus() != GSM_READY)&&(theGSM3ShieldV1ModemCore.getStatus() != GPRS_READY))
	  return 2;

	theGSM3ShieldV1ModemCore.setPhoneNumber((char*)to);
	theGSM3ShieldV1ModemCore.openCommand(this,BEGINSMS);
	beginSMSContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Send SMS continue function.
void GSM3ShieldV1SMSProvider::beginSMSContinue()
{
	bool resp;
	// 1: Send AT
	// 2: wait for > and write text
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+CMGS=\""), false);
		theGSM3ShieldV1ModemCore.print(theGSM3ShieldV1ModemCore.getPhoneNumber());
		theGSM3ShieldV1ModemCore.print("\"\r");
		break;
	case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp, ">"))
		{
			if (resp) theGSM3ShieldV1ModemCore.closeCommand(1);
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
		break;
	}
}

//Send SMS write function.
void GSM3ShieldV1SMSProvider::writeSMS(char c)
{
	theGSM3ShieldV1ModemCore.write(c);
}

//Send SMS begin function.
int GSM3ShieldV1SMSProvider::endSMS()
{
	theGSM3ShieldV1ModemCore.openCommand(this,ENDSMS);
	endSMSContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Send SMS continue function.
void GSM3ShieldV1SMSProvider::endSMSContinue()
{
	bool resp;
	// 1: Send #26
	// 2: wait for OK
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		theGSM3ShieldV1ModemCore.write(26);
		theGSM3ShieldV1ModemCore.print("\r");
		break;
	case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			if (resp) 
				theGSM3ShieldV1ModemCore.closeCommand(1);
			else 
				theGSM3ShieldV1ModemCore.closeCommand(3);
		}
		break;
	}
}

//Available SMS main function.
int GSM3ShieldV1SMSProvider::availableSMS()
{
	flagReadingSMS = 0;
	theGSM3ShieldV1ModemCore.openCommand(this,AVAILABLESMS);
	availableSMSContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Available SMS continue function.
void GSM3ShieldV1SMSProvider::availableSMSContinue()
{
	// 1:  AT+CMGL="REC UNREAD",1
	// 2: Receive +CMGL: _id_ ... READ","_numero_" ... \n_mensaje_\nOK
	// 3: Send AT+CMGD= _id_
	// 4: Receive OK
	// 5: Remaining SMS text in case full buffer.
	// This implementation really does not care much if the modem aswers trash to CMGL
	bool resp;
	//int msglength_aux;
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:	
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+CMGL=\"REC UNREAD\",1"));
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(parseCMGL_available(resp))
			{
				if (!resp) theGSM3ShieldV1ModemCore.closeCommand(4);
				else theGSM3ShieldV1ModemCore.closeCommand(1);
			}
		break;
	}
	  
}	
		
//SMS available parse.
bool GSM3ShieldV1SMSProvider::parseCMGL_available(bool& rsp)
{
	fullBufferSMS = (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()<=4);
	if (!(theGSM3ShieldV1ModemCore.theBuffer().chopUntil("+CMGL:", true)))
		rsp = false;
	else 
		rsp = true;
	idSMS=theGSM3ShieldV1ModemCore.theBuffer().readInt();

	//If there are 2 SMS in buffer, response is ...CRLFCRLF+CMGL
	twoSMSinBuffer = theGSM3ShieldV1ModemCore.theBuffer().locate("\r\n\r\n+");

	checkSecondBuffer = 0;
	
	return true;
}

//remoteNumber SMS function.
int GSM3ShieldV1SMSProvider::remoteSMSNumber(char* number, int nlength)
{
	theGSM3ShieldV1ModemCore.theBuffer().extractSubstring("READ\",\"", "\"", number, nlength);	
	
	return 1;
}

//remoteNumber SMS function.
int GSM3ShieldV1SMSProvider::readSMS()
{
	char charSMS;
	//First char.
	if (!flagReadingSMS) 
	{
		flagReadingSMS = 1;
		theGSM3ShieldV1ModemCore.theBuffer().chopUntil("\n", true);
	}
	charSMS = theGSM3ShieldV1ModemCore.theBuffer().read(); 
	
	//Second Buffer.
	if (checkSecondBuffer)
	{
		checkSecondBuffer = 0;
		twoSMSinBuffer = theGSM3ShieldV1ModemCore.theBuffer().locate("\r\n\r\n+");
	}

	//Case the last char in buffer.
	if ((!twoSMSinBuffer)&&fullBufferSMS&&(theGSM3ShieldV1ModemCore.theBuffer().availableBytes()==127))
	{
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		fullBufferSMS = 0;
		checkSecondBuffer = 1;
		theGSM3ShieldV1ModemCore.openCommand(this,XON);
		theGSM3ShieldV1ModemCore.gss.spaceAvailable();
		delay(10);
		
		return charSMS;
	}
	//Case two SMS in buffer
	else if (twoSMSinBuffer)
	{
		if (theGSM3ShieldV1ModemCore.theBuffer().locate("\r\n\r\n+")) 
		{
					return charSMS;
		}
		else 
		{
			theGSM3ShieldV1ModemCore.theBuffer().flush();
			theGSM3ShieldV1ModemCore.openCommand(this,XON);
			theGSM3ShieldV1ModemCore.gss.spaceAvailable();
			delay(10);
			return 0;
		}
	}
	//Case 1 SMS and buffer not full
	else if (!fullBufferSMS)
	{
		if (theGSM3ShieldV1ModemCore.theBuffer().locate("\r\n\r\nOK")) 
		{
			return charSMS;
		}
		else 
		{
			theGSM3ShieldV1ModemCore.theBuffer().flush();
			theGSM3ShieldV1ModemCore.openCommand(this,XON);
			theGSM3ShieldV1ModemCore.gss.spaceAvailable();
			delay(10);
			return 0;
		}
	}
	//Case to read all the chars in buffer to the end.
	else 
	{
		return charSMS;		
	}
}	

//Read socket main function.
int GSM3ShieldV1SMSProvider::peekSMS()
{
	if (!flagReadingSMS) 
	{
		flagReadingSMS = 1;
		theGSM3ShieldV1ModemCore.theBuffer().chopUntil("\n", true);
	}

	return theGSM3ShieldV1ModemCore.theBuffer().peek(0); 
}
	
//Flush SMS main function.
void GSM3ShieldV1SMSProvider::flushSMS()
{

	//With this, sms data can fill up to 2x128+5x128 bytes.
	for (int aux = 0;aux<5;aux++)
	{
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		theGSM3ShieldV1ModemCore.gss.spaceAvailable();
		delay(10);
	}
		
	theGSM3ShieldV1ModemCore.openCommand(this,FLUSHSMS);
	flushSMSContinue();
}

//Send SMS continue function.
void GSM3ShieldV1SMSProvider::flushSMSContinue()
{
	bool resp;
	// 1: Deleting SMS
	// 2: wait for OK
	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+CMGD="), false);
		theGSM3ShieldV1ModemCore.print(idSMS);
		theGSM3ShieldV1ModemCore.print("\r");
		break;
	case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			if (resp) theGSM3ShieldV1ModemCore.closeCommand(1);
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
		break;
	}
}

void GSM3ShieldV1SMSProvider::manageResponse(byte from, byte to)
{
	switch(theGSM3ShieldV1ModemCore.getOngoingCommand())
	{
/*		case XON:
			if (flagReadingSocket) 
				{
//					flagReadingSocket = 0;
					fullBufferSocket = (theGSM3ShieldV1ModemCore.theBuffer().availableBytes()<3);
				}
			else theGSM3ShieldV1ModemCore.openCommand(this,NONE);
			break;
*/		case NONE:
			theGSM3ShieldV1ModemCore.gss.cb.deleteToTheEnd(from);
			break;
		case BEGINSMS:
			beginSMSContinue();
			break;
		case ENDSMS:
			endSMSContinue();
			break;
		case AVAILABLESMS:
			availableSMSContinue();
			break;
		case FLUSHSMS:
			flushSMSContinue();
			break;
	}
}
