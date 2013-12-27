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
#ifndef __GSM3_SHIELDV1SERVERPROVIDER__
#define __GSM3_SHIELDV1SERVERPROVIDER__

#include <GSM3MobileServerProvider.h>
#include <GSM3ShieldV1BaseProvider.h>

class GSM3ShieldV1MultiServerProvider : public GSM3MobileServerProvider,  public GSM3ShieldV1BaseProvider
{
	private:
		
		// Used sockets
		uint8_t socketsAsServer;
		uint8_t socketsAccepted;

		/** Continue to connect TCP server function
		 */
		void connectTCPServerContinue();

		/** Continue to get IP function
		 */
		void getIPContinue();
		
		/** Release socket
			@param socket		Socket
		 */
		void releaseSocket(int socket);
		
		/** Parse QILOCIP response
			@param LocalIP			Buffer for save local IP address
			@param LocalIPlength	Buffer size
			@param rsp				Returns if expected response exists
			@return true if command executed correctly
		 */
		bool parseQILOCIP_rsp(char* LocalIP, int LocalIPlength, bool& rsp);
		
	public:
	
		/** Constructor */
		GSM3ShieldV1MultiServerProvider();
		
		/** minSocketAsServer
			@return 0
		 */
		int minSocketAsServer(){return 0;};
		
		/** maxSocketAsServer
			@return 0
		 */
		int maxSocketAsServer(){return 4;};
		
		/** Get modem status
			@param s
			@return modem status (true if connected)
		 */
		bool getSocketAsServerModemStatus(int s);
		
		/** Get new occupied socket as server
			@return command error if exists
		 */
		int getNewOccupiedSocketAsServer();

		/** Connect server to TCP port
			@param port			TCP port
			@return	command error if exists
		 */
		int connectTCPServer(int port);
		
		/** Get server IP address
			@param LocalIP			Buffer for copy IP address
			@param LocalIPlength	Length of buffer
			@return command error if exists
		 */
		int getIP(char* LocalIP, int LocalIPlength);
		
//		int disconnectTCP(bool client1Server0, int id_socket);
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		 */
		int ready(){return GSM3ShieldV1BaseProvider::ready();};
		
		/** Get socket status as server
			@param socket		Socket to get status
			@return socket status
		 */
		bool getStatusSocketAsServer(uint8_t socket);
		
		/** Manages modem response
			@param from 		Initial byte of buffer
			@param to 			Final byte of buffer
		 */
		void manageResponse(byte from, byte to);
		
		/** Recognize unsolicited event
			@param oldTail		
			@return true if successful
		 */		
		bool recognizeUnsolicitedEvent(byte oldTail);

	
};

#endif
