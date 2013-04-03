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
#ifndef __GSM3_MOBILESERVERPROVIDER__
#define __GSM3_MOBILESERVERPROVIDER__


#include <GSM3MobileAccessProvider.h>
#include <inttypes.h>
#include <stddef.h>
		

class GSM3MobileServerProvider
{	
		/** Get socket status
			@param s			Socket
			@return modem status (true if connected)
		 */
		virtual bool getSocketAsServerModemStatus(int s)=0;
		
	public:
		
		/** minSocketAsServer
			@return socket
		 */
		virtual int minSocketAsServer()=0;
		
		/** maxSocketAsServer
			@return socket
		 */
		virtual int maxSocketAsServer()=0;
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		virtual int ready()=0;
		
		/** Constructor */
		GSM3MobileServerProvider(){};
						
		/** Connect server to TCP port
			@param port			TCP port
			@return	command error if exists
		 */
		virtual  int connectTCPServer(int port)=0;
		//virtual  int getIP(char* LocalIP, int LocalIPlength)=0;
				
		/** Get new occupied socket as server
			@return return -1 if no new socket has been occupied
		 */
		virtual int getNewOccupiedSocketAsServer()=0;
		
		/** Get socket status
			@param socket		Socket
			@return socket status (true if connected)
		 */
		virtual bool getStatusSocketAsServer(uint8_t socket)=0;
		
		// virtual int disconnectTCP(bool client1Server0, int idsocket)=0;

};

extern GSM3MobileServerProvider* theGSM3MobileServerProvider;

#endif
