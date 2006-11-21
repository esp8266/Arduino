/*
  Sonar.h - Parallax PING))) Ultrasonic Sensor Library
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

#ifndef Sonar_h
#define Sonar_h

#include <inttypes.h>

class Sonar
{
  private:
    uint8_t _pinSignal;
  public:
    Sonar(uint8_t);
    unsigned long readMicroseconds();
    unsigned long readInches();
    unsigned long readCentimeters();
};

#endif

