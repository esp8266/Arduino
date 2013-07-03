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
#include <GSM3VoiceCallService.h>
#include <Arduino.h>

#include <GSM3ShieldV1VoiceProvider.h>
GSM3ShieldV1VoiceProvider theShieldV1VoiceProvider;

// While there is only a shield (ShieldV1) we will include it by default

#define GSM3VOICECALLSERVICE_SYNCH 0x01 // 1: synchronous 0: asynchronous
#define __TOUT__ 10000




GSM3VoiceCallService::GSM3VoiceCallService(bool synch)
{
	if(synch)
		flags |= GSM3VOICECALLSERVICE_SYNCH;
	theGSM3MobileVoiceProvider->initialize();
}

GSM3_voiceCall_st GSM3VoiceCallService::getvoiceCallStatus()
{
	if(theGSM3MobileVoiceProvider==0)
		return IDLE_CALL;
	
	return theGSM3MobileVoiceProvider->getvoiceCallStatus();
}

int GSM3VoiceCallService::ready()
{
	if(theGSM3MobileVoiceProvider==0)
		return 0;
	
	return theGSM3MobileVoiceProvider->ready();
}

int GSM3VoiceCallService::voiceCall(const char* to, unsigned long timeout)
{
	if(theGSM3MobileVoiceProvider==0)
		return 0;
		
	if(flags & GSM3VOICECALLSERVICE_SYNCH )
	{
		theGSM3MobileVoiceProvider->voiceCall(to);
		unsigned long m;
		m=millis();
		// Wait an answer for timeout
		while(((millis()-m)< timeout )&&(getvoiceCallStatus()==CALLING))
			delay(100);
		
		if(getvoiceCallStatus()==TALKING)
			return 1;
		else
			return 0;
	}
	else
	{ 
		return theGSM3MobileVoiceProvider->voiceCall(to);
	}
		
}

int GSM3VoiceCallService::answerCall()
{
	if(theGSM3MobileVoiceProvider==0)
		return 0;
	
	return waitForAnswerIfNeeded(theGSM3MobileVoiceProvider->answerCall());
}

int GSM3VoiceCallService::hangCall()
{
	if(theGSM3MobileVoiceProvider==0)
		return 0;
	
	return waitForAnswerIfNeeded(theGSM3MobileVoiceProvider->hangCall());
}

int GSM3VoiceCallService::retrieveCallingNumber(char* buffer, int bufsize)
{
	if(theGSM3MobileVoiceProvider==0)
		return 0;
	
	return waitForAnswerIfNeeded(theGSM3MobileVoiceProvider->retrieveCallingNumber(buffer, bufsize));
}

int GSM3VoiceCallService::waitForAnswerIfNeeded(int returnvalue)
{
	// If synchronous
	if(flags & GSM3VOICECALLSERVICE_SYNCH )
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




