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
#ifndef _GSM3SMSSERVICE_
#define _GSM3SMSSERVICE_

#include <GSM3MobileSMSProvider.h>
#include <Stream.h>

class GSM3SMSService : public Stream 
{
	private:
	
		uint8_t flags;
		
		/** Makes synchronous the functions, if needed
			@param returnvalue	Return value
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		int waitForAnswerIfNeeded(int returnvalue);
		
	public:
	
		/** Constructor
			@param synch		Determines sync mode
		 */
		GSM3SMSService(bool synch=true);
		
		/** Write a character in SMS message
			@param c			Character
			@return size
		 */
		size_t write(uint8_t c);
		
		/** Begin a SMS to send it
			@param to			Destination
			@return error command if it exists
		 */
		int beginSMS(const char* to);
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		int ready();
		
		/** End SMS
			@return error command if it exists
		 */
		int endSMS();
		
		/** Check if SMS available and prepare it to be read
			@return number of bytes in a received SMS
		 */
		int available();
		
		/** Read sender number phone
			@param number		Buffer for save number phone
			@param nlength		Buffer length
			@return 1 success, >1 error
		 */
		int remoteNumber(char* number, int nlength); 
		
		/** Read one char for SMS buffer (advance circular buffer)
			@return byte
		 */
		int read();
		
		/** Read a byte but do not advance the buffer header (circular buffer)
			@return byte
		 */
		int peek();
		
		/** Delete the SMS from Modem memory and proccess answer
		 */
		void flush();

};


#endif