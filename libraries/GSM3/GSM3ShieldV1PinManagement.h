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
#ifndef _GSM3SHIELDV1PINMANAGEMENT_
#define _GSM3SHIELDV1PINMANAGEMENT_

#include <GSM3ShieldV1AccessProvider.h>
#include <GSM3ShieldV1DirectModemProvider.h>

class GSM3ShieldV1PinManagement
{

	private:
	
		GSM3ShieldV1AccessProvider gsm; // GSM access required for network register with PIN code     
		GSM3ShieldV1DirectModemProvider modemAccess; 
		bool pin_used; // determines if pin lock is activated 
		
	public:

		/** Constructor */
		GSM3ShieldV1PinManagement();
		
		/** Check modem response and restart it
		 */
		void begin();
	
		/** Check if PIN lock or PUK lock is activated
			@return 0 if PIN lock is off, 1 if PIN lock is on, -1 if PUK lock is on, -2 if error exists
		 */
		int isPIN();
		
		/** Check if PIN code is correct and valid
			@param pin			PIN code
			@return 0 if is correct, -1 if is incorrect
		 */
		int checkPIN(String pin);
		
		/** Check if PUK code is correct and establish new PIN code
			@param puk			PUK code
			@param pin			New PIN code
			@return 0 if successful, otherwise return -1
		 */
		int checkPUK(String puk, String pin);
		
		/** Change PIN code
			@param old			Old PIN code
			@param pin			New PIN code
		 */
		void changePIN(String old, String pin);
		
		/** Change PIN lock status
			@param pin			PIN code
		 */
		void switchPIN(String pin);
		
		/** Check if modem was registered in GSM/GPRS network
			@return 0 if modem was registered, 1 if modem was registered in roaming, -1 if error exists
		 */
		int checkReg();
		
		/** Return if PIN lock is used
			@return true if PIN lock is used, otherwise, return false
		 */
		bool getPINUsed();
		
		/** Set PIN lock status
			@param used			New PIN lock status
		 */
		void setPINUsed(bool used);
};

#endif;