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
#ifndef _GSM3VOICECALLSERVICE_
#define _GSM3VOICECALLSERVICE_

#include <GSM3MobileNetworkProvider.h>
#include <GSM3MobileVoiceProvider.h>

class GSM3VoiceCallService
{
	private:
		uint8_t flags;
		
		/** Make synchronous the functions, if needed
			@param returnvalue	Return value
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		int waitForAnswerIfNeeded(int returnvalue);
		
	public:
		/** Service creation
			@param synch		If true, the service calls are synchronois
		*/
		GSM3VoiceCallService(bool synch=true);

		/** Voice call status
			@return Status of the voice call, as described in GSM3MobileVoiceProvider.h
		          { IDLE_CALL, CALLING, RECEIVINGCALL, TALKING};
		*/
		GSM3_voiceCall_st getvoiceCallStatus();

		/** Get last command status
			@return Returns 0 if last command is still executing, 1 success, >1 error
		*/
		int ready();
		
		/** Place a voice call. If asynchronous, returns while ringing. If synchronous
			returns if the call is stablished or cancelled.
			@param to			Receiver number. Country extension can be used or not.
								Char buffer should not be released or used until command is over
			@param timeout		In millisecods. Time ringing before closing the call. 
								Only used in synchronous mode.
								If zero, ring undefinitely
			@return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
					In synchronous mode returns 1 if the call is placed, 0 if not.
		*/		
		int voiceCall(const char* to, unsigned long timeout=30000);
		
		/** Accept an incoming voice call
			@return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
					In synchronous mode returns 1 if the call is answered, 0 if not.
		*/		
		int answerCall();
		
		/** Hang a stablished call or an incoming ring
			@return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
					In synchronous mode returns 1 if the call is answered, 0 if not.
		*/		
		int hangCall();

		/** Retrieve the calling number, put it in buffer
			@param buffer		pointer to the buffer memory
			@param bufsize		size of available memory area, at least should be 10 characters
			@return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
					In synchronous mode returns 1 if the number is correcty taken 0 if not
		*/		
		int retrieveCallingNumber(char* buffer, int bufsize);
};


#endif