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

#ifndef __GSM3DIRECTMODEMPROVIDER__
#define __GSM3DIRECTMODEMPROVIDER__

#include <GSM3SoftSerial.h>
#include <GSM3MobileNetworkProvider.h>
#include <GSM3ShieldV1BaseProvider.h>
#include <Stream.h>
#include <Arduino.h>

class GSM3ShieldV1DirectModemProvider : public GSM3ShieldV1BaseProvider, public Stream
{			
		private:
			
			bool trace;
			
        public:
			
			/** Constructor
				@param trace if true, dumps all AT dialogue to Serial
			*/
			GSM3ShieldV1DirectModemProvider(bool trace=false);
			
			/**
			 */
			void begin();
			
			/**
			 */
			void restartModem();
			
			/** Enable the debug process.
			 */
			void connect();
			
			/** Disable the debug process.
			 */
			void disconnect();
			
			/** Debug write to modem by means of SoftSerial.
				@param c			Character
				@return size
			 */
			size_t write(uint8_t c);
			
			/** Check for incoming bytes in buffer
				@return
			 */
			int available();
			
			/** Read from circular buffer
				@return character
			 */
			int read();
			
			/** Read from circular buffer, but do not delete it
				@return character
			 */
			int peek();
			
			/** Empty circular buffer
			 */
			void flush();
			
			/** Manages modem response
				@param from 		Initial byte of buffer
				@param to 			Final byte of buffer
			 */
			void manageResponse(byte from, byte to){};
			
			/** Recognize unsolicited event
				@param from		
				@return true if successful
			 */
			bool recognizeUnsolicitedEvent(byte from){return false;};
			
			/** Send AT command to modem
				@param command		AT command
				@param delay		Time to wait for response
				@return response from modem
			 */
			String writeModemCommand(String command, int delay);
};

#endif