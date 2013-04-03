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
#ifndef __GSM3_SOFTSERIAL__
#define __GSM3_SOFTSERIAL__

// An adaptation of NewSoftSerial for Modem Shields
// Assumes directly that Serial is attached to Pins 2 and 3, not inverse
// We are implementing it because NewSoftSerial does not deal correctly with floods
// of data
#include "GSM3CircularBuffer.h"
#include <avr/pgmspace.h>

/*
#define _COMSTATUS_ANSWERRECEIVED_ 0x100
#define _COMSTATUS_SMSRECEIVED_ 0x80
#define _COMSTATUS_CALLRECEIVED_ 0x40

// PLEASE, when accessing the sockets use "for" and >> (bitwise operator)
#define _COMSTATUS_SOCKET6RECEIVED_ 0x20
#define _COMSTATUS_SOCKET5RECEIVED_ 0x10
#define _COMSTATUS_SOCKET4RECEIVED_ 0x08
#define _COMSTATUS_SOCKET3RECEIVED_ 0x04
#define _COMSTATUS_SOCKET2RECEIVED_ 0x02
#define _COMSTATUS_SOCKET1RECEIVED_ 0x01

#define __CALLTABLEMASK__ 0x3
*/

class GSM3SoftSerialMgr
{
	public:
	
		/** Manages soft serial message
			@param from			Initial byte
			@param to			Final byte
		 */
		virtual void manageMsg(byte from, byte to);
};

// This class manages software serial communications
// Changing it so it doesn't know about modems or whatever

class GSM3SoftSerial : public GSM3CircularBufferManager
{
	private:
	
		uint8_t _receiveBitMask;
		volatile uint8_t *_receivePortRegister;
		uint8_t _transmitBitMask;
		volatile uint8_t *_transmitPortRegister;
	  
		static GSM3SoftSerial* _activeObject;
		GSM3SoftSerialMgr* mgr;
	  
		uint16_t _rx_delay_centering;
		uint16_t _rx_delay_intrabit;
		uint16_t _rx_delay_stopbit;
		uint16_t _tx_delay;
		uint8_t _flags;
	  
		/** Write in tx_pin
			@param pin_state		Pin state
		 */
		void tx_pin_write(uint8_t pin_state);
		
		/** Set transmission
		 */
		void setTX();
		
		/** Set receiver
		 */
		void setRX();
		
		/** Receive
		 */
		void recv();
		
		/** Read from rx_pin
			@return receive bit mask
		 */
		uint8_t rx_pin_read();
		
		void setComsReceived();
		
		/** Write a character in serial connection, final action after escaping
			@param c			Character
			@return	1 if succesful, 0 if transmission delay = 0
		 */
		virtual size_t finalWrite(uint8_t);

		/** Decide, attending to escapes, if the received character should we
		    kept, forgotten, or changed
			@param c			Character, may be changed
			@return	1 if shall be kept, 0 if forgotten
		 */
		bool keepThisChar(uint8_t* c);
		  
		// Checks the buffer for well-known events. 
		//bool recognizeUnsolicitedEvent(byte oldTail);
	  
	  public:
	  
		/** Tuned delay in microcontroller
			@param delay		Time to delay
		 */
		static /*inline */void tunedDelay(uint16_t delay);
		
		GSM3CircularBuffer cb; // Circular buffer
		
		/** Register serial manager
			@param manager		Serial manager
		 */
		inline void registerMgr(GSM3SoftSerialMgr* manager){mgr=manager;};
		
		/** If there is spaceAvailable in the buffer, lets send a XON
		 */
		void spaceAvailable();
		
		/** Write a character in serial connection
			@param c			Character
			@return	1 if succesful, 0 if transmission delay = 0
		 */
		virtual size_t write(uint8_t);
		
		/** Constructor */
		GSM3SoftSerial();
		
		/** Establish serial connection
			@param speed		Baudrate
			@return
		 */
		int begin(long speed);
		
		/** Manage interruptions
		 */
		static inline void handle_interrupt();
		
		/** Close serial connection
		 */
		void close();
};

#endif