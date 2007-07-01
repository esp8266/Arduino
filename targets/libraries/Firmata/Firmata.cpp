/*
  Firmata.cpp - Firmata library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Hans-Christoph Steiner <hans@at.or.at>

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
*/

//******************************************************************************
//* Includes
//******************************************************************************

extern "C" {
  // AVR LibC Includes
  #include <inttypes.h>
  #include <stdlib.h>

  // Wiring Core Includes
  #include "WConstants.h"
}

#include "Firmata.h"
#include "EEPROM.h"
#include "HardwareSerial.h"

//******************************************************************************
//* Definitions
//******************************************************************************

//******************************************************************************
//* Constructors
//******************************************************************************

FirmataClass::FirmataClass()
{
	// TODO: init serial here
	// TODO: printVersion
}

//******************************************************************************
//* Private Methods
//******************************************************************************
// resets the system state upon a SYSTEM_RESET message from the host software
void FirmataClass::systemReset(void)
{
	// TODO automatically call this in response to SYSTEM_RESET
	// TODO reset EEPROM to 0 here
}

//******************************************************************************
//* Public Methods
//******************************************************************************

// output type of message that is next on the queue
int FirmataClass::available(void)
{
	// TODO output next available message type, or -1 if nothing
}


// output the protocol version message to the serial port
void FirmataClass::printVersion() {
  Serial.print(REPORT_VERSION, BYTE);
  Serial.print(FIRMATA_MINOR_VERSION, BYTE);
  Serial.print(FIRMATA_MAJOR_VERSION, BYTE);
}

// send an analog message
void FirmataClass::sendAnalog(int pin, int value) 
{
	// pin can only be 0-15, so chop higher bits
	Serial.print(ANALOG_MESSAGE | (pin & 0xF), BYTE);
	Serial.print(value % 128, BYTE);
	Serial.print(value >> 7, BYTE); 
}

// send a single digital pin in a digital message
void FirmataClass::sendDigital(int pin, int value) 
{
	// TODO add single pin digital messages to the  protocol
}

// send 14-bits in a single digital message
void FirmataClass::sendDigitalPortPair(int port, int value) 
{
	// TODO: the digital message should not be sent on the serial port every
	// time sendDigital() is called.  Instead, it should add it to an int
	// which will be sent on a schedule.  If a pin changes more than once
	// before the digital message is sent on the serial port, it should send a
	// digital message for each change.
 
	// TODO: some math needs to happen for pin > 14 since MIDI channels are used
	Serial.print(DIGITAL_MESSAGE | (port & 0xF),BYTE);
	Serial.print(value % 128, BYTE); // Tx pins 0-6
	Serial.print(value >> 7, BYTE);  // Tx pins 7-13
}

// Internal Actions/////////////////////////////////////////////////////////////

void FirmataClass::loadState(void)
{
	// TODO load state from EEPROM
}

void FirmataClass::saveState(void)
{
	// TODO save state to EEPROM
}

void FirmataClass::resetState(void)
{
	// TODO reset state bytes in EEPROM
}

// make one instance for the user to use
FirmataClass Firmata;
