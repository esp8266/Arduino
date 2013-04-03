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


