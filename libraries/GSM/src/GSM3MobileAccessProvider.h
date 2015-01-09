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
#ifndef _GSM3MOBILEACCESSPROVIDER_
#define _GSM3MOBILEACCESSPROVIDER_

enum GSM3_NetworkStatus_t { ERROR, IDLE, CONNECTING, GSM_READY, GPRS_READY, TRANSPARENT_CONNECTED, OFF};

class GSM3MobileAccessProvider
{
	public:
		// Access functions
		//Configuration functions.
		/** Establish GSM connection
			@param pin			PIN code
			@param restart		Determines if hardware restart
			@param synchronous	Determines sync mode
			@return If synchronous, GSM3_NetworkStatus_t. If asynchronous, returns 0.
		 */
		virtual inline GSM3_NetworkStatus_t begin(char* pin=0,bool restart=true, bool synchronous=true)=0;
		
		/** Check network access status
			@return 1 if Alive, 0 if down
		*/
		virtual inline int isAccessAlive()=0;
		
		/** Shutdown the modem (power off really)
			@return true if successful
		 */
		virtual inline bool shutdown()=0;
		
		/** Secure shutdown the modem (power off really)
			@return always true
		 */
		virtual inline bool secureShutdown()=0;
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		virtual int ready()=0;
};

#endif