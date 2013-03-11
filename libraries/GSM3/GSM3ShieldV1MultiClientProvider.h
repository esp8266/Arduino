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
#ifndef __GSM3_SHIELDV1CLIENTPROVIDER__
#define __GSM3_SHIELDV1CLIENTPROVIDER__

#include <GSM3MobileClientProvider.h>
#include <GSM3ShieldV1BaseProvider.h>

class GSM3ShieldV1MultiClientProvider : public GSM3MobileClientProvider,  public GSM3ShieldV1BaseProvider
{
	private:
	
		int remotePort;		// Current operation remote port
		int idSocket; 		// Remote ID socket
		IPAddress remoteIP; // Remote IP address
		
		uint16_t sockets;

		/** Continue to connect TCP client function
		 */
		void connectTCPClientContinue();
		
		/** Continue to disconnect TCP client function
		 */
		void disconnectTCPContinue();
		
		/** Continue to begin socket for write function
		 */
		void beginWriteSocketContinue();
		
		/** Continue to end write socket function
		 */
		void endWriteSocketContinue();
		
		/** Continue to available socket function
		 */
		void availableSocketContinue();
		
		/** Continue to flush socket function
		 */
		void flushSocketContinue();
		
		// GATHER!
		bool flagReadingSocket; //In case socket data being read, update fullBufferSocket in the next buffer.
		bool fullBufferSocket;	//To detect if the socket data being read needs another buffer.
		bool client1_server0;	//1 Client, 0 Server.
		
		/** Parse QIRD response
			@param rsp		Returns true if expected response exists
			@return true if command executed correctly
		 */
		bool parseQIRD_head(bool& rsp); 

	public:
	
		/** Constructor */
		GSM3ShieldV1MultiClientProvider();
		
		/** Minimum socket
			@return 0
		 */
		int minSocket(){return 0;};
		
		/** Maximum socket
			@return 5
		 */
		int maxSocket(){return 5;};
		
		/** Connect to a remote TCP server
			@param server		String with IP or server name
			@param port 		Remote port number
			@param id_socket	Local socket number
			@return 0 if command running, 1 if success, otherwise error
		*/
		int connectTCPClient(const char* server, int port, int id_socket);
		
		/** Connect to a remote TCP server
			@param add			Remote IP address
			@param port 		Remote port number
			@param id_socket	Local socket number
			@return 0 if command running, 1 if success, otherwise error
		*/
		int connectTCPClient(IPAddress add, int port, int id_socket);

		/** Begin writing through a socket
			@param client1Server0	1 if modem acts as client, 0 if acts as server
			@param id_socket		Local socket number
			@return 0 if command running, 1 if success, otherwise error
		*/
		void beginWriteSocket(bool client1Server0, int id_socket);

		/** Write through a socket. MUST go after beginWriteSocket()
			@param buf			characters to be written (final 0 will not be written)
		*/
		void writeSocket(const char* buf);

		/** Write through a socket. MUST go after beginWriteSocket()
			@param c character to be written
		*/		
		void writeSocket(char c);
		
		/** Finish current writing
		*/	
		void endWriteSocket();
		
		/** Check if there are data to be read in socket.
			@param client1Server0	1 if modem acts as client, 0 if acts as server
			@param id_socket		Local socket number
			@return 0 if command running, 1 if there are data available, 4 if no data, otherwise error
		*/
		int availableSocket(bool client, int id_socket); // With "available" and "readSocket" ask the modem for 1500 bytes.
		
		/** Read a character from socket
			@return socket
		 */
		int readSocket(); //If Read() gets to the end of the QIRD response, execute again QIRD SYNCHRONOUSLY 
		
		/** Flush socket
		 */
		void flushSocket();
		
		/** Get a character but will not advance the buffer head
			@return character
		 */
		int peekSocket();

		/** Close a socket
			@param client1Server0	1 if modem acts as client, 0 if acts as server
			@param id_socket		Local socket number
			@return 0 if command running, 1 if success, otherwise error
		*/
		int disconnectTCP(bool client1Server0, int id_socket);
		
		/** Recognize unsolicited event
			@param from		
			@return true if successful
		*/		
		bool recognizeUnsolicitedEvent(byte from);
	
		/** Manages modem response
			@param from 		Initial byte of buffer
			@param to 			Final byte of buffer
		 */
		void manageResponse(byte from, byte to);
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		*/
		int ready(){return GSM3ShieldV1BaseProvider::ready();};
		
		/** Get client socket
			@param socket		
			@return	socket
		 */
		int getSocket(int socket=-1);
		
		/** Release socket
			@param socket		Socket for release
		 */
		void releaseSocket(int socket);
		
		/** Get socket client status
			@param socket		Socket
			@return socket client status
		 */
		bool getStatusSocketClient(uint8_t socket);
		
};


#endif