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
#ifndef __GSM3_MOBILECLIENTPROVIDER__
#define __GSM3_MOBILECLIENTPROVIDER__

#include <Arduino.h>
#include <IPAddress.h>

class GSM3MobileClientProvider
{
	protected:

		uint8_t sockets;

	public:

		/** Constructor */
		GSM3MobileClientProvider(){};
		
		/** Minimum socket
			@return socket
		 */
		virtual inline int minSocket()=0;
		
		/** Maximum socket
			@return socket
		 */
		virtual inline int maxSocket()=0;

		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		*/
		virtual int ready()=0;
		
		/** Get status socket client
			@param socket		Socket
			@return 1 if connected
		 */
		virtual bool getStatusSocketClient(uint8_t socket)=0;

		// Socket management
		
		/** Get socket
			@param socket		Socket
			@return socket
		 */
		virtual int getSocket(int socket=-1)=0;
		
		/** Release socket
			@param socket		Socket
		 */
		virtual void releaseSocket(int socket)=0;

		// Client socket functions
		
		/** Connect to a server via TCP connection
			@param server		Server name or IP address in a String
			@param port			Port
			@param id_socket	Socket
			@return 0 if command running, 1 if success, otherwise error
		 */
		virtual int connectTCPClient(const char* server, int port, int id_socket)=0;
		
		/** Connect to a server (by IP address) via TCP connection
			@param add			IP address in IPAddress format
			@param port			Port
			@param id_socket	Socket
			@return 0 if command running, 1 if success, otherwise error
		 */
		virtual int connectTCPClient(IPAddress add, int port, int id_socket)=0;
		
		/** Begin writing through a socket
			@param client1Server0	1 if modem acts as client, 0 if acts as server
			@param id_socket		Local socket number
			@return 0 if command running, 1 if success, otherwise error
		 */
		virtual void beginWriteSocket(bool client1Server0, int id_socket)=0;
		
		/** Write through a socket. MUST go after beginWriteSocket()
			@param c 				character to be written
		 */
		virtual void writeSocket(uint8_t c)=0;
		
		/** Write through a socket. MUST go after beginWriteSocket()
			@param buf				characters to be written (final 0 will not be written)
		 */
		virtual void writeSocket(const char* buf)=0;
		
		/** Finish current writing
		 */	
		virtual void endWriteSocket()=0;
		
		/** Check if there are data to be read in socket.
			@param client1Server0	1 if modem acts as client, 0 if acts as server
			@param id_socket		Local socket number
			@return 0 if command running, 1 if there are data available, 4 if no data, otherwise error
		 */
		virtual int availableSocket(bool client, int id_socket)=0;
		
		/** Read data (get a character) available in socket
			@return character
		 */
		virtual int readSocket()=0;
		
		/** Flush socket
		 */
		virtual void flushSocket()=0;
		
		/** Get a character but will not advance the buffer head
			@return character
		 */
		virtual int peekSocket()=0;
		
		/** Close a socket
			@param client1Server0	1 if modem acts as client, 0 if acts as server
			@param id_socket		Socket
			@return 0 if command running, 1 if success, otherwise error
		 */
		virtual int disconnectTCP(bool client1Server0, int idsocket)=0;

};

extern GSM3MobileClientProvider* theGSM3MobileClientProvider;

#endif
