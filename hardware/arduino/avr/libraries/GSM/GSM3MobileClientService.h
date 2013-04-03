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
#ifndef _GSM3MOBILECLIENTSERVICE_
#define _GSM3MOBILECLIENTSERVICE_

#include <GSM3MobileNetworkProvider.h>
#include <Client.h>


class GSM3MobileClientService : public Client
{
	private:
	
		uint8_t mySocket;
		uint8_t flags;
		
		/** Blocks waiting for an answer
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		int waitForAnswer();

	public:
	
		/** Constructor
			@param synch		Sync mode
		 */
		GSM3MobileClientService(bool synch=true);
		
		/** Constructor
			@param socket		Socket
			@param synch		Sync mode
		 */
		GSM3MobileClientService(int socket, bool synch);
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		*/
		int ready();
		
		// we take this function out as IPAddress is complex to bring to
		// version 1.
		/** Connect to server by IP address
			@param (IPAddress)
			@param (uint16_t)
			@return returns 0 if last command is still executing, 1 success, 2 if there are no resources
		 */
		inline int connect(IPAddress, uint16_t);

		/** Connect to server by hostname
			@param host			Hostname
			@param port			Port
			@return returns 0 if last command is still executing, 1 success, 2 if there are no resources
		 */
		int connect(const char *host, uint16_t port);
		
		/** Initialize write in request
			@param sync			Sync mode
		 */
		void beginWrite(bool sync=false);
		
		/** Write a character in request
			@param c			Character
			@return size
		 */
		size_t write(uint8_t c);
		
		/** Write a characters buffer in request
			@param buf			Buffer
			@return buffer size
		 */
		size_t write(const uint8_t *buf);
		
		/** Write a characters buffer with size in request
			@param (uint8_t*)	Buffer
			@param (size_t)		Buffer size
			@return buffer size
		 */
		size_t write(const uint8_t*, size_t);
		
		/** Finish write request
			@param sync			Sync mode
		 */
		void endWrite(bool sync=false);
		
		/** Check if connected to server
			@return 1 if connected
		 */
		uint8_t connected();
		
		operator bool();
		
		/** Read from response buffer and copy size specified to buffer
			@param buf			Buffer		
			@param size			Buffer size
			@return bytes read
		 */
		int read(uint8_t *buf, size_t size);
		
		/** Read a character from response buffer
			@return character
		 */
		int read();
		
		/** Check if exists a response available
			@return 1 if exists, 0 if not exists
		 */
		int available();
		
		/** Read a character from response buffer but does not move the pointer.
			@return character
		 */
		int peek();
		
		/** Flush response buffer
		 */
		void flush();
		
		/** Stop client
		 */
		void stop();
		
		/** Get socket
			@return socket
		 */
		inline int getSocket(){return (int)mySocket;};

		
};


#endif