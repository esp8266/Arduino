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
#ifndef _GSM3MOBILENETWORKPROVIDER_
#define _GSM3MOBILENETWORKPROVIDER_

#include <GSM3MobileAccessProvider.h>
#include <inttypes.h>
#include <stddef.h>
#include <IPAddress.h>
		
class GSM3MobileNetworkProvider 
{
	private:
	
		/** Restart hardware
			@return 1 if successful
		 */
		int HWrestart();
		
		uint16_t socketsAsServer; // Server socket
		
		/** Get modem status
			@param s			Socket
			@return modem status (true if connected)
		 */
		virtual inline bool getSocketAsServerModemStatus(int s){return false;};
		
	public:
		
		/** minSocketAsServer
			@return 0
		 */
		virtual inline int minSocketAsServer(){return 0;};
		
		/** maxSocketAsServer
			@return 0
		 */
		virtual inline int maxSocketAsServer(){return 0;};
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		virtual int ready()=0;
		
		/** Constructor */
		GSM3MobileNetworkProvider();
		
		/** Get network status
			@return network status
		 */
		virtual inline GSM3_NetworkStatus_t getStatus(){return ERROR;};
		
		/** Get socket client status
			@param socket		Socket
			@return 1 if connected, 0 otherwise
		 */
		bool getStatusSocketClient(uint8_t socket);

		/** Close a AT command
			@param code			Close code
		 */
		virtual inline void closeCommand(int code){};
		
		/** Establish a TCP connection
			@param port				Port
			@param localIP			IP address
			@param localIPlength	IP address size in characters
			@return	command error if exists
		 */
		virtual inline int connectTCPServer(int port, char* localIP, int localIPlength){return 0;};
		
		/** Get local IP address
			@param LocalIP			Buffer for save IP address
			@param LocalIPlength	Buffer size
		 */
		virtual inline int getIP(char* LocalIP, int LocalIPlength){return 0;};
			
		/** Get new occupied socket
			@return -1 if no new socket has been occupied
		 */
		int getNewOccupiedSocketAsServer();
		
		/** Get socket status as server
			@param socket			Socket to get status
			@return socket status
		 */
		bool getStatusSocketAsServer(uint8_t socket);
		
		/** Close a socket
			@param client1Server0	1 if modem acts as client, 0 if acts as server
			@param id_socket		Local socket number
			@return 0 if command running, 1 if success, otherwise error
		 */
		int disconnectTCP(bool client1Server0, int idsocket){return 1;};
		
		/** Release socket
			@param socket		Socket
		 */
		void releaseSocket(int socket){};

};

extern GSM3MobileNetworkProvider* theProvider;

#endif
