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
#ifndef __GSM3_SHIELDV1CELLMANAGEMENT__
#define __GSM3_SHIELDV1CELLMANAGEMENT__

#include <GSM3ShieldV1ModemCore.h>
#include <GSM3MobileCellManagement.h>
#include <GSM3ShieldV1CellManagement.h>

class GSM3ShieldV1CellManagement : public GSM3MobileCellManagement, public GSM3ShieldV1BaseProvider
{		
	public:
	
		/** Constructor
		*/
		GSM3ShieldV1CellManagement();

		/** Manages modem response
			@param from 		Initial byte of buffer
			@param to 			Final byte of buffer
		 */
		void manageResponse(byte from, byte to);
		
		/** getLocation
		 @return current cell location
		*/		
		int getLocation(char *country, char *network, char *area, char *cell);
		
		/** getICCID
		*/
		int getICCID(char *iccid);
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		int ready(){return GSM3ShieldV1BaseProvider::ready();};

	private:	
	
		char *countryCode;
		char *networkCode;
		char *locationArea;
		char *cellId;
		
		char *bufferICCID;
	
		/** Continue to getLocation function
		 */
		void getLocationContinue();
		
		/** Continue to getICCID function
		 */
		void getICCIDContinue();
		
		bool parseQENG_available(bool& rsp);
		
		bool parseQCCID_available(bool& rsp);
		
};

#endif