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
#ifndef _GSM3MOBILESERVERSERVICE_
#define _GSM3MOBILESERVERSERVICE_

#include <GSM3MobileNetworkProvider.h>
#include <GSM3MobileClientService.h>
#include <Server.h>

class GSM3MobileServerService : public Server
{
	private:
	
		uint8_t _port; // Port
		uint8_t mySocket; // Actual socket
		uint8_t flags;
		bool local1Remote0;
	
		/** Internal utility, used in synchronous calls
			@return operation result, 1 if success, 0 otherwise
		 */
		int waitForAnswer();

	public:
		
		/** Constructor
			@param port			Port
			@param synch		True if the server acts synchronously
		 */
		GSM3MobileServerService(uint8_t port, bool synch=true);
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		int ready();
		
		/** Initialize server
		 */
		void begin();
		
		/** Check if there is an incoming client request
		  @param synch			If true, the returned client is synchronous or
								blocking.
		  @return Client if successful, else error
		*/
		GSM3MobileClientService available(bool synch=true);
		
		// Just to keep in line with Ethernet.
		// Write to every open socket...
		//void write(uint8_t);
		//void write(const uint8_t *buf, size_t size);
		
		/** Begin write in socket
		 */
		void beginWrite();
		
		/** Write character in socket
			@param c			Character
			@return size
		 */
		size_t write(uint8_t c);
		
		/** Write buffer in socket
			@param buf			Buffer
			@return size
		 */
		size_t write(const uint8_t *buf);
		
		/** Write buffer in socket with size
			@param buf			Buffer		
			@param sz			Buffer size
			@return size
		 */
		size_t write(const uint8_t *buf, size_t sz);
		
		/** End write in socket
		 */
		void endWrite();

		/** Stop server
		 */
		void stop();
		
		// we take this function out as IPAddress is complex to bring to
		// version 1.
		// inline int connect(IPAddress ip, uint16_t port){return 0;};
		// Returns 2 if there are no resources
		//int getIP(char* LocalIP, int LocalIPlength);
		
};


#endif