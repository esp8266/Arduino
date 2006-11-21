/*
  Sonar.cpp - Parallax PING))) Ultrasonic Sensor Library
  Copyright (c) 2006 David A. Mellis.  All right reserved.

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
  
  For more information on the Parallax PING))) Ultrasonic Sensor, see:
  http://www.parallax.com/detail.asp?product_id=28015
*/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "WConstants.h"
#include "Sonar.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

/******************************************************************************
 * Constructors
 ******************************************************************************/

Sonar::Sonar(uint8_t signal)
{
	_pinSignal = signal;
}

/******************************************************************************
 * User API
 ******************************************************************************/

unsigned long Sonar::readMicroseconds()
{
	pinMode(_pinSignal, OUTPUT);

	// The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
	// We give a short LOW pulse beforehand to ensure a clean HIGH pulse.
	digitalWrite(_pinSignal, LOW);
	delayMicroseconds(2);
	digitalWrite(_pinSignal, HIGH);
	delayMicroseconds(5);
	digitalWrite(_pinSignal, LOW);
	
	// The same pin is used to read the signal from the PING))): a HIGH
	// pulse whose duration is the time from the sending of the ping to
	// to reception of the echo.
	pinMode(_pinSignal, INPUT);
	
	return pulseIn(_pinSignal, HIGH);
}

unsigned long Sonar::readInches()
{
	// According to Parallax's datasheet for the PING))), there are
	// 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
	// second).  This gives the distance travelled by the ping, outbound
	// and return, so we divide by 2 to get the distance of the obstacle.
	// See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
	return readMicroseconds() / 74 / 2;
}

unsigned long Sonar::readCentimeters()
{
	// 73.746 microseconds per inch works out to about 29 microseconds per
	// centimeter (there are 2.54 centimeters per inch).
	return readMicroseconds() / 29 / 2;
}
