#include <GSM3ShieldV1AccessProvider.h>
#include <Arduino.h>
#include "GSM3IO.h"

#define __TOUTSHUTDOWN__ 5000
#define __TOUTMODEMCONFIGURATION__ 5000//equivalent to 30000 because of time in interrupt routine.
#define __TOUTAT__ 1000

const char _command_AT[] PROGMEM = "AT";
const char _command_CGREG[] PROGMEM = "AT+CGREG?";


GSM3ShieldV1AccessProvider::GSM3ShieldV1AccessProvider(bool debug)
{
	theGSM3ShieldV1ModemCore.setDebug(debug);

}

void GSM3ShieldV1AccessProvider::manageResponse(byte from, byte to)
{
	switch(theGSM3ShieldV1ModemCore.getOngoingCommand())
	{
		case MODEMCONFIG:
			ModemConfigurationContinue();
			break;
		case ALIVETEST:
			isModemAliveContinue();
			break;
	}
}

///////////////////////////////////////////////////////CONFIGURATION FUNCTIONS///////////////////////////////////////////////////////////////////

// Begin
// Restart or start the modem
// May be synchronous
GSM3_NetworkStatus_t GSM3ShieldV1AccessProvider::begin(char* pin, bool restart, bool synchronous)
{	
	pinMode(__RESETPIN__, OUTPUT);

	#ifdef TTOPEN_V1
	pinMode(__POWERPIN__, OUTPUT);
	digitalWrite(__POWERPIN__, HIGH);
	#endif

	// If asked for modem restart, restart
	if (restart) 
		HWrestart();
	else 
 		HWstart();
  
	theGSM3ShieldV1ModemCore.gss.begin(9600);
	// Launch modem configuration commands
	ModemConfiguration(pin);
	// If synchronous, wait till ModemConfiguration is over
	if(synchronous)
	{
		// if we shorten this delay, the command fails
		while(ready()==0) 
			delay(1000); 
	}
	return getStatus();
}

//HWrestart.
int GSM3ShieldV1AccessProvider::HWrestart()
{
	#ifdef TTOPEN_V1
	digitalWrite(__POWERPIN__, HIGH);
	delay(1000);
	#endif
	
	theGSM3ShieldV1ModemCore.setStatus(IDLE);
	digitalWrite(__RESETPIN__, HIGH);
	delay(12000);
	digitalWrite(__RESETPIN__, LOW);
	delay(1000);
	return 1; //configandwait(pin);
}

//HWrestart.
int GSM3ShieldV1AccessProvider::HWstart()
{

	theGSM3ShieldV1ModemCore.setStatus(IDLE);
	digitalWrite(__RESETPIN__, HIGH);
	delay(2000);
	digitalWrite(__RESETPIN__, LOW);
	//delay(1000);

	return 1; //configandwait(pin);
}

//Initial configuration main function.
int GSM3ShieldV1AccessProvider::ModemConfiguration(char* pin)
{
	theGSM3ShieldV1ModemCore.setPhoneNumber(pin);
	theGSM3ShieldV1ModemCore.openCommand(this,MODEMCONFIG);
	theGSM3ShieldV1ModemCore.setStatus(CONNECTING);
	ModemConfigurationContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Initial configuration continue function.
void GSM3ShieldV1AccessProvider::ModemConfigurationContinue()
{
	bool resp;

	// 1: Send AT
	// 2: Wait AT OK and SetPin or CGREG
	// 3: Wait Pin OK and CGREG
	// 4: Wait CGREG and Flow SW control or CGREG
	// 5: Wait IFC OK and SMS Text Mode
	// 6: Wait SMS text Mode OK and Calling line identification
	// 7: Wait Calling Line Id OK and Echo off
	// 8: Wait for OK and COLP command for connecting line identification.
	// 9: Wait for OK.
	int ct=theGSM3ShieldV1ModemCore.getCommandCounter();
	if(ct==1)
	{
		// Launch AT	
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_AT);
	}
	else if(ct==2)
	{
		// Wait for AT - OK.
	   if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	   {
			if(resp)
			{ 
				// OK received
				if(theGSM3ShieldV1ModemCore.getPhoneNumber() && (theGSM3ShieldV1ModemCore.getPhoneNumber()[0]!=0)) 
					{
						theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+CPIN="), false);
						theGSM3ShieldV1ModemCore.setCommandCounter(3);
						theGSM3ShieldV1ModemCore.genericCommand_rqc(theGSM3ShieldV1ModemCore.getPhoneNumber());
					}
				else 
					{
						//DEBUG	
						//Serial.println("AT+CGREG?");	
						theGSM3ShieldV1ModemCore.setCommandCounter(4);
						theGSM3ShieldV1ModemCore.takeMilliseconds();
						theGSM3ShieldV1ModemCore.genericCommand_rq(_command_CGREG);
					}
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
	else if(ct==3)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if(resp)
			{
				theGSM3ShieldV1ModemCore.setCommandCounter(4);
				theGSM3ShieldV1ModemCore.takeMilliseconds();
				theGSM3ShieldV1ModemCore.delayInsideInterrupt(2000);
				theGSM3ShieldV1ModemCore.genericCommand_rq(_command_CGREG);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
	    }
	}
	else if(ct==4)
	{
		char auxLocate1 [12];
		char auxLocate2 [12];
		prepareAuxLocate(PSTR("+CGREG: 0,1"), auxLocate1);
		prepareAuxLocate(PSTR("+CGREG: 0,5"), auxLocate2);
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp, auxLocate1, auxLocate2))
		{
			if(resp)
			{
				theGSM3ShieldV1ModemCore.setCommandCounter(5);
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+IFC=1,1"));
			}
			else
			{
				// If not, launch command again
				if(theGSM3ShieldV1ModemCore.takeMilliseconds() > __TOUTMODEMCONFIGURATION__)
				{
					theGSM3ShieldV1ModemCore.closeCommand(3);
				}
				else 
				{
					theGSM3ShieldV1ModemCore.delayInsideInterrupt(2000);
					theGSM3ShieldV1ModemCore.genericCommand_rq(_command_CGREG);
				}
			}
		}	
	}
	else if(ct==5)
	{
		// 5: Wait IFC OK
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			//Delay for SW flow control being active.
			theGSM3ShieldV1ModemCore.delayInsideInterrupt(2000);
			// 9: SMS Text Mode
			theGSM3ShieldV1ModemCore.setCommandCounter(6);
			theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+CMGF=1"));
		}
	}
	else if(ct==6)
	{
		// 6: Wait SMS text Mode OK
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			//Calling line identification
			theGSM3ShieldV1ModemCore.setCommandCounter(7);			
			theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+CLIP=1"));
		}
	}
	else if(ct==7)
	{
		// 7: Wait Calling Line Id OK
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			// Echo off
			theGSM3ShieldV1ModemCore.setCommandCounter(8);			
			theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("ATE0"));
		}
	}
	else if(ct==8)
	{
		// 8: Wait ATEO OK, send COLP
		// In Arduino Mega, attention, take away the COLP step
		// It looks as we can only have 8 steps
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			theGSM3ShieldV1ModemCore.setCommandCounter(9);
			theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+COLP=1"));
		}
	}
	else if(ct==9)
	{
		// 9: Wait ATCOLP OK
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
		{
			if (resp) 
				{
					theGSM3ShieldV1ModemCore.setStatus(GSM_READY);
					theGSM3ShieldV1ModemCore.closeCommand(1);
				}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
 	}
}

//Alive Test main function.
int GSM3ShieldV1AccessProvider::isAccessAlive()
{
	theGSM3ShieldV1ModemCore.setCommandError(0);
	theGSM3ShieldV1ModemCore.setCommandCounter(1);
	theGSM3ShieldV1ModemCore.openCommand(this,ALIVETEST);
	isModemAliveContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

//Alive Test continue function.
void GSM3ShieldV1AccessProvider::isModemAliveContinue()
{
bool rsp;
switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
    case 1:
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_AT);
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
      break;
	case 2:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(rsp))
		{
			if (rsp) theGSM3ShieldV1ModemCore.closeCommand(1);
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
      break;
	}
}

//Shutdown.
bool GSM3ShieldV1AccessProvider::shutdown()
{
	unsigned long m;
	bool resp;
	char auxLocate [18];
	
	// It makes no sense to have an asynchronous shutdown
	pinMode(__RESETPIN__, OUTPUT);
	digitalWrite(__RESETPIN__, HIGH);
	delay(1500);
	digitalWrite(__RESETPIN__, LOW);
	theGSM3ShieldV1ModemCore.setStatus(IDLE);
	theGSM3ShieldV1ModemCore.gss.close();
	
	m=millis();
	prepareAuxLocate(PSTR("POWER DOWN"), auxLocate);
	while((millis()-m) < __TOUTSHUTDOWN__)
	{
		delay(1);
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp, auxLocate))
			return resp;
	}
	return false;
}

//Secure shutdown.
bool GSM3ShieldV1AccessProvider::secureShutdown()
{
	// It makes no sense to have an asynchronous shutdown
	pinMode(__RESETPIN__, OUTPUT);
	digitalWrite(__RESETPIN__, HIGH);
	delay(900);
	digitalWrite(__RESETPIN__, LOW);
	theGSM3ShieldV1ModemCore.setStatus(OFF);
	theGSM3ShieldV1ModemCore.gss.close();

#ifdef TTOPEN_V1
	_delay_ms(12000);
	digitalWrite(__POWERPIN__, LOW);
#endif
	
	return true;
}