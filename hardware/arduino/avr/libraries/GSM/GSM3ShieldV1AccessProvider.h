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
#ifndef _GSM3SHIELDV1ACCESSPROVIDER_
#define _GSM3SHIELDV1ACCESSPROVIDER_

#include <GSM3MobileAccessProvider.h>
#include <GSM3ShieldV1ModemCore.h>
#include <GSM3ShieldV1BaseProvider.h>

class GSM3ShieldV1AccessProvider : public GSM3MobileAccessProvider, public GSM3ShieldV1BaseProvider
{
	private:

		/** Initialize main modem configuration
			@param pin			PIN code
			@return command error if exists
		 */
		int ModemConfiguration(char* pin);
		
		/** Continue to modem configuration function
		 */
		void ModemConfigurationContinue();
		
		/** Continue to check if modem alive function
		 */
		void isModemAliveContinue();
		

	public:
	
		/** Constructor
			@param debug		Determines debug mode
		 */
		
		GSM3ShieldV1AccessProvider(bool debug=false);
		
		/** Start the GSM/GPRS modem, attaching to the GSM network
			@param pin 			SIM PIN number (4 digits in a string, example: "1234"). If 
								NULL the SIM has no configured PIN.
			@param restart		Restart the modem. Default is TRUE. The modem receives
								a signal through the Ctrl/D7 pin. If it is shut down, it will
								start-up. If it is running, it will restart. Takes up to 10 
								seconds
			@param synchronous	If TRUE the call only returns after the Start is complete 
								or fails. If FALSE the call will return inmediately. You have
								to call repeatedly ready() until you get a result. Default is TRUE.
			@return If synchronous, GSM3_NetworkStatus_t. If asynchronous, returns 0.
		*/
		GSM3_NetworkStatus_t begin(char* pin=0,bool restart=true, bool synchronous=true);
		
		/** Check network access status
			@return 1 if Alive, 0 if down
		*/
		int isAccessAlive();
		
		/** Shutdown the modem (power off really)
			@return true if successful
		*/
		bool shutdown();
		
		/** Returns 0 if last command is still executing
			@return 1 if success, >1 if error 
		*/
		int ready(){return GSM3ShieldV1BaseProvider::ready();};
		
		/** Returns modem status
			@return modem network status 
		*/
		inline GSM3_NetworkStatus_t getStatus(){return theGSM3ShieldV1ModemCore.getStatus();};

		void manageResponse(byte from, byte to);

		/** Restart the modem (will shut down if running)
			@return 1 if success, >1 if error 
		*/		
		int HWrestart();
		
		/** Start the modem (will not shut down if running)
			@return 1 if success, >1 if error 
		*/		
		int HWstart();

};

#endif