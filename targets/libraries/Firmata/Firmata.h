/*
  Firmata.h - Firmata library
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

#ifndef Firmata_h
#define Firmata_h

#include <inttypes.h>

class FirmataClass
{
private:
	void systemReset(void);
public:
	FirmataClass();
	int available(void);
	// serial receive actions
	// send serial messages
	void printVersion(void);
	void sendAnalog(int, int);
	void sendDigital(int, int);
	void sendDigitalPortPair(int, int);
	// internal actions
	void loadState(void);
	void saveState(void);
	void resetState(void);
};

extern FirmataClass Firmata;

/*==============================================================================
 * MACROS
 *============================================================================*/

/* Version numbers for the protocol.  The protocol is still changing, so these
 * version numbers are important.  This number can be queried so that host
 * software can test whether it will be compatible with the currently
 * installed firmware. */
#define FIRMATA_MAJOR_VERSION   1 // for non-compatible changes
#define FIRMATA_MINOR_VERSION   0 // for backwards compatible changes

// total number of pins currently supported
#define TOTAL_ANALOG_PINS       6
#define TOTAL_DIGITAL_PINS      14

// for comparing along with INPUT and OUTPUT
#define PWM                     2

// for selecting digital inputs
#define PB  2  // digital input, pins 8-13
#define PC  3  // analog input port
#define PD  4  // digital input, pins 0-7

#define MAX_DATA_BYTES 2 // max number of data bytes in non-SysEx messages
// message command bytes
#define DIGITAL_MESSAGE         0x90 // send data for a digital pin
#define ANALOG_MESSAGE          0xE0 // send data for an analog pin (or PWM)
//#define PULSE_MESSAGE           0xA0 // proposed pulseIn/Out message (SysEx)
//#define SHIFTOUT_MESSAGE        0xB0 // proposed shiftOut message (SysEx)
#define REPORT_ANALOG_PIN       0xC0 // enable analog input by pin #
#define REPORT_DIGITAL_PORTS    0xD0 // enable digital input by port pair
#define START_SYSEX             0xF0 // start a MIDI SysEx message
#define SET_DIGITAL_PIN_MODE    0xF4 // set a digital pin to INPUT or OUTPUT 
#define END_SYSEX               0xF7 // end a MIDI SysEx message
#define REPORT_VERSION          0xF9 // report firmware version
#define SYSTEM_RESET            0xFF // reset from MIDI

// these are used for EEPROM reading and writing
#define ANALOGINPUTSTOREPORT_LOW_BYTE   0x1F0 // analogInputsToReport is an int
#define ANALOGINPUTSTOREPORT_HIGH_BYTE  0x1F1 // analogInputsToReport is an int
#define REPORTDIGITALINPUTS_BYTE        0x1F2 // 
#define DIGITALPINSTATUS_LOW_BYTE       0x1F3 // digitalPinStatus is an int
#define DIGITALPINSTATUS_HIGH_BYTE      0x1F4 // digitalPinStatus is an int
#define PWMSTATUS_LOW_BYTE              0x1F5 // pwmStatus is an int
#define PWMSTATUS_HIGH_BYTE             0x1F6 // pwmStatus is an int


#endif /* Firmata_h */

