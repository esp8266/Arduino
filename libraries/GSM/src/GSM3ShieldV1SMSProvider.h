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
#ifndef __GSM3_SHIELDV1SMSPROVIDER__
#define __GSM3_SHIELDV1SMSPROVIDER__

#include <GSM3ShieldV1ModemCore.h>
#include <GSM3MobileSMSProvider.h>
#include <GSM3ShieldV1SMSProvider.h>


class GSM3ShieldV1SMSProvider : public GSM3MobileSMSProvider, public GSM3ShieldV1BaseProvider
{		
	public:
		GSM3ShieldV1SMSProvider();
	
		/** Manages modem response
			@param from 		Initial byte of buffer
			@param to 			Final byte of buffer
		 */
		void manageResponse(byte from, byte to);
	
		/** Begin a SMS to send it
			@param to			Destination
			@return error command if it exists
		 */
		inline int beginSMS(const char* to);
		
		/** Write a SMS character by character
			@param c			Character
		 */
		inline void writeSMS(char c);
		
		/** End SMS
			@return error command if it exists
		 */
		inline int endSMS();
		
		/** Check if SMS available and prepare it to be read
			@return number of bytes in a received SMS
		 */
		int availableSMS();
		
		/** Read a byte but do not advance the buffer header (circular buffer)
			@return character
		 */
		int peekSMS();
		
		/** Delete the SMS from Modem memory and proccess answer
		 */
		void flushSMS();
		
		/** Read sender number phone
			@param number		Buffer for save number phone
			@param nlength		Buffer length
			@return 1 success, >1 error
		 */
		int remoteSMSNumber(char* number, int nlength); //Before reading the SMS, read the phone number.
		
		/** Read one char for SMS buffer (advance circular buffer)
			@return character
		 */
		int readSMS();
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		int ready(){return GSM3ShieldV1BaseProvider::ready();};

	private:	
	
		int idSMS; // Id from current SMS being read.
		bool flagReadingSMS; // To detect first SMS char if not yet reading.
		bool fullBufferSMS; // To detect if the SMS being read needs another buffer. 
		bool twoSMSinBuffer; // To detect if the buffer has more than 1 SMS.
		bool checkSecondBuffer; // Pending to detect if the second buffer has more than 1 SMS.

		/** Continue to begin SMS function
		 */
		void beginSMSContinue();
		
		/** Continue to end SMS function
		 */
		void endSMSContinue();
		
		/** Continue to available SMS function
		 */
		void availableSMSContinue();
		
		/** Continue to flush SMS function
		 */
		void flushSMSContinue();
		
		/** Parse CMGL response
			@param rsp			Returns true if expected response exists
			@return	true if command executed correctly
		 */
		bool parseCMGL_available(bool& rsp);
};
#endif