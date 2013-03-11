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
#ifndef _GSM3SHIELDV1DATANETWORKPROVIDER_
#define _GSM3SHIELDV1DATANETWORKPROVIDER_

#include <GSM3MobileDataNetworkProvider.h>
#include <GSM3ShieldV1BaseProvider.h>
#include <GSM3ShieldV1ModemCore.h>
#include <IPAddress.h>

class GSM3ShieldV1DataNetworkProvider : public GSM3MobileDataNetworkProvider, public GSM3ShieldV1BaseProvider
{
	private:
	
		char* user; // Username for GPRS
		char* passwd; // Password for GPRS
	
		/** Continue to attach GPRS function
		 */
		void attachGPRSContinue();
		
		/** Continue to detach GPRS function
		 */
		void detachGPRSContinue();
		
		/** Parse QILOCIP response
			@param LocalIP			Buffer for save local IP address
			@param LocalIPlength	Buffer size
			@param rsp				Returns true if expected response exists
			@return true if command executed correctly
		 */
		bool parseQILOCIP_rsp(char* LocalIP, int LocalIPlength, bool& rsp);
		
		/** Continue to get IP function
		 */
		void getIPContinue();
		
		/** Implementation of inet_aton standard function
			@param aIPAddrString	IP address in characters buffer
			@param aResult			IP address in IPAddress format
			@return 1 if the address is successfully converted, or 0 if the conversion failed
		 */
		int inet_aton(const char* aIPAddrString, IPAddress& aResult);
	
	public:
	
		/** Attach to GPRS/GSM network
			@param networkId 	APN GPRS
			@param user			Username
			@param pass			Password
			@return connection status
		 */
		GSM3_NetworkStatus_t networkAttach(char* networkId, char* user, char* pass)
		{
			return attachGPRS(networkId, user, pass);
		};
		
		/** Detach GPRS/GSM network
			@return connection status
		 */
		GSM3_NetworkStatus_t networkDetach(){ return detachGPRS();};
		
		/** Attach to GPRS service
			@param apn			APN GPRS
			@param user_name	Username
			@param password		Password
			@param synchronous	Sync mode
			@return connection status
		 */
		GSM3_NetworkStatus_t attachGPRS(char* apn, char* user_name, char* password, bool synchronous=true);
		
		/** Detach GPRS service
			@param synchronous	Sync mode
			@return connection status
		 */
		GSM3_NetworkStatus_t detachGPRS(bool synchronous=true);
		
		/** Returns 0 if last command is still executing
			@return 1 if success, >1 if error 
		 */
		int ready(){return GSM3ShieldV1BaseProvider::ready();};
		
		/** Get network status (connection)
			@return status
		 */
		inline GSM3_NetworkStatus_t getStatus(){return theGSM3ShieldV1ModemCore.getStatus();};

		/** Get actual assigned IP address
			@param LocalIP			Buffer for copy IP address
			@param LocalIPlength	Buffer length
			@return command error if exists
		 */
		int getIP(char* LocalIP, int LocalIPlength);
		
		/** Get actual assigned IP address in IPAddress format
			@return IP address in IPAddress format
		 */
		IPAddress getIPAddress();
		
		/** Manages modem response
			@param from 		Initial byte of buffer
			@param to 			Final byte of buffer
		 */
		void manageResponse(byte from, byte to);


};

#endif