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
#include <GSM3SMSService.h>
#include <GSM3MobileNetworkProvider.h>
#include <Arduino.h>

// While there is only a shield (ShieldV1) we will include it by default
#include <GSM3ShieldV1SMSProvider.h>
GSM3ShieldV1SMSProvider theShieldV1SMSProvider;

#define GSM3SMSSERVICE_SYNCH 0x01 // 1: synchronous 0: asynchronous
#define __TOUT__ 10000


GSM3SMSService::GSM3SMSService(bool synch)
{
	if(synch)
		flags |= GSM3SMSSERVICE_SYNCH;
}

// Returns 0 if last command is still executing
// 1 if success
// >1 if error 
int GSM3SMSService::ready()
{	
	return theGSM3SMSProvider->ready();
}

int GSM3SMSService::beginSMS(const char *number)
{	
	return waitForAnswerIfNeeded(theGSM3SMSProvider->beginSMS(number));
};

int GSM3SMSService::endSMS()
{
	return waitForAnswerIfNeeded(theGSM3SMSProvider->endSMS());
};

size_t GSM3SMSService::write(uint8_t c)
{
	theGSM3SMSProvider->writeSMS(c);
	return 1;
}

void GSM3SMSService::flush()
{		
	theGSM3SMSProvider->flushSMS();
	waitForAnswerIfNeeded(1);
};

int GSM3SMSService::available()
{
	return waitForAnswerIfNeeded(theGSM3SMSProvider->availableSMS());
};

int GSM3SMSService::remoteNumber(char* number, int nlength)
{
	return theGSM3SMSProvider->remoteSMSNumber(number, nlength);

}

int GSM3SMSService::read()
{
	return theGSM3SMSProvider->readSMS();
};
int GSM3SMSService::peek()
{		
	return theGSM3SMSProvider->peekSMS();
};

int GSM3SMSService::waitForAnswerIfNeeded(int returnvalue)
{
	// If synchronous
	if(flags & GSM3SMSSERVICE_SYNCH )
	{
		unsigned long m;
		m=millis();
		// Wait for __TOUT__
		while(((millis()-m)< __TOUT__ )&&(ready()==0)) 
			delay(100);
		// If everything was OK, return 1
		// else (timeout or error codes) return 0;
		if(ready()==1)
			return 1;
		else
			return 0;
	}
	// If not synchronous just kick ahead the coming result
	return ready();
}





