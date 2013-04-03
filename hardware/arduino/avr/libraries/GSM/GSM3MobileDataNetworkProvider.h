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
#ifndef _GSM3MOBILEDATANETWORKPROVIDER_
#define _GSM3MOBILEDATANETWORKPROVIDER_

#include <GSM3MobileAccessProvider.h>

// This class is not really useful, but serves as a guideline for programmers
// We keep it but it should never be linked
class GSM3MobileDataNetworkProvider
{
	public:
		
		/** Attach to GPRS/GSM network
			@param networkId 	APN GPRS
			@param user			Username
			@param pass			Password
			@return connection status
		 */
		virtual GSM3_NetworkStatus_t networkAttach(char* networId, char* user, char* pass)=0;
		
		/** Detach GPRS/GSM network
			@return connection status
		 */
		virtual GSM3_NetworkStatus_t networkDetach()=0;
				
};

extern GSM3MobileDataNetworkProvider* theGSM3MobileDataNetworkProvider;

#endif