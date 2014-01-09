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
#ifndef _GSM3MOBILEMOCKUPPROVIDER_
#define _GSM3MOBILEMOCKUPPROVIDER_

#include <GSM3MobileNetworkProvider.h>
#include <GSM3MobileVoiceProvider.h>
		
class GSM3MobileMockupProvider: public GSM3MobileNetworkProvider
{				
	private:
		// Introducing this status is quite "heavy". But something like this should
		// be added to ShieldV1. Or not. 
		// Note, in ShieldV1 there is no "RECEIVINGSMS" status.
		enum GSM3_modemlinest_e { IDLE, WAITINGANSWER, SENDINGSMS};
		GSM3_modemlinest_e lineStatus;
		char* msgExample;
		int msgIndex;
		
	public:
	
		/** Minimum socket
			@return 1
		 */
		inline int minSocket(){return 1;};
		
		/** Maximum socket
			@return 8
		 */
		inline int maxSocket(){return 8;};

		/** Constructor */
		GSM3MobileMockupProvider();
		
		/** Get network status
			@return network status
		 */
		inline GSM3_NetworkStatus_t getStatus(){return ERROR;};
		
		/** Get voice call status
			@return call status
		 */
		inline GSM3_voiceCall_st getvoiceCallStatus(){return IDLE_CALL;};
		
		/** Get last command status
			@return Returns 0 if last command is still executing, 1 success, >1 error
		 */
		int ready();
		inline void closeCommand(int code){};
		
		//Configuration functions.
		
		/** Begin connection
			@param pin			PIN code
			@return 
		 */
		int begin(char* pin=0);
		
		/** Check if is modem alive
			@return 0
		 */
		inline int isModemAlive(){return 0;};
		
		/** Shutdown the modem (power off really)
			@return true if successful
		 */
		inline bool shutdown(){return false;};
		
		//Call functions
		
		/** Launch a voice call
			@param number	 	Phone number to be called
			@return If asynchronous, returns 0. If synchronous, 1 if success, other if error
		 */
		inline int voiceCall(const char* number){return 0;};
		
		/** Answer a voice call
			@return If asynchronous, returns 0. If synchronous, 1 if success, other if error
		 */
		inline int answerCall(){return 0;};
		
		/** Hang a voice call
			@return If asynchronous, returns 0. If synchronous, 1 if success, other if error
		 */
		inline int hangCall(){return 0;};
		
		/** Retrieve phone number of caller
			@param buffer		Buffer for copy phone number
			@param bufsize		Buffer size
			@return If asynchronous, returns 0. If synchronous, 1 if success, other if error
		 */
		inline int retrieveCallingNumber(char* buffer, int*& bufsize){return 0;};
		
		// SMS functions
		
		/** Begin a SMS to send it
			@param number		Destination
			@return error command if it exists
		 */
		int beginSMS(const char* number);
		
		/** End SMS
			@return error command if it exists
		 */
		int endSMS();
		
		/** Check if SMS available and prepare it to be read
			@return error command if it exists
		 */
		int availableSMS();
		
		/** Read a byte but do not advance the buffer header (circular buffer)
			@return character
		 */
		int peek();
		
		/** Delete the SMS from Modem memory and proccess answer
		 */
		void flushSMS();
		
		/** Read sender number phone
			@param number		Buffer for save number phone
			@param nlength		Buffer length
			@return 1 success, >1 error
		 */
		int remoteSMSNumber(char* number, int nlength); 
		
		/** Read one char for SMS buffer (advance circular buffer)
			@return character
		 */
		int readSMS();
		
		/** Write a SMS character by character
			@param c			Character
		 */
		void writeSMS(char c);
		
		// Socket functions
		
		/** Connect to a remote TCP server
			@param server		String with IP or server name
			@param port 		Remote port number
			@param id_socket	Local socket number
			@return 0 if command running, 1 if success, otherwise error
		*/
		int connectTCPClient(const char* server, int port, int id_socket);
		
		// Attention to parameter rewriting in ShieldV1
		/** Write buffer information into a socket
			@param buf			Buffer
			@param size			Buffer size
			@param idsocket		Socket
		 */
		void writeSocket(const uint8_t *buf, size_t size, int idsocket);
		
		// ShieldV1 will have two reading mechanisms:
		// Mechanism 1: Call AT+QIRD for size bytes. Put them in the circular buffer,
		// fill buf. Take care to xon/xoff effect, as we may copy just a part of the 
		// incoming bytes.
		/** Read socket and put information in a buffer
			@param buf			Buffer
			@param size			Buffer size
			@param idsocket		Socket
			@return
		 */
		int readSocket(uint8_t *buf, size_t size, int idsocket);
		
		// Mechanism 2 in ShieldV1:
		// When called "available()" or "read()" reuse readSocket code to execute 
		// QIRD SYNCHRONOUSLY. Ask the modem for 1500 bytes but do not copy them anywhere, 
		// leave data in the circular buffer. Put buffer head at the start of received data.
		// Peek() will get a character but will not advance the buffer head.
		// Read() will get one character. XON/XOFF will take care of buffer filling
		// If Read() gets to the end of the QIRD response, execute again QIRD SYNCHRONOUSLY 
		// If the user executes flush(), execute read() until there is nothing more to read()
		// (the modem gives no way to empty the socket of incoming data)
		
		/** Check if there are data to be read in socket.
			@param idsocket	Local socket number
			@return 0 if command running, 1 if there are data available, 4 if no data, otherwise error
		*/
		int availableSocket(int idsocket);
		
		/** Read data (get a character) available in socket
			@param idsocket		Socket
			@param advance		Determines if advance the buffer head
			@return character
		 */
		int readSocket(int idsocket, bool advance=true);
		
		/** Flush socket
			@param idsocket		Socket
		 */
		void flushSocket(int idsocket);
		
		// This is the same in ShieldV1
		/** Close a socket
			@param idsocket			Socket
			@return 0 if command running, 1 if success, otherwise error
		 */
		int disconnectTCP(int idsocket);
		
		// TCP Server. Attention. Changing the int*&. We'll receive a buffer for the IP
		// If the pointer ins NULL just forget it
		// I think that opening a server does not occupy a socket. Is that true?
		/** Establish a TCP connection
			@param port				Port
			@param localIP			IP address
			@param localIPlength	IP address size in characters
			@return	command error if exists
		 */
		int connectTCPServer(int port, char* localIP, int* localIPlength);
		
		// Modem sockets status. Return TRUE if the modem thinks the socket is occupied.
		// This should be detected through an unrequisited response
		/** Get modem status
			@param s			Socket
			@return modem status (true if connected)
		 */
		bool getSocketModemStatus(uint8_t s);


};
#endif
