/*
  Matrix.h - Max7219 LED Matrix library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

#ifndef Matrix_h
#define Matrix_h

// include core Wiring API
#include "WProgram.h"

// declare other libraries depended on (if any)
class Sprite;

class Matrix
{
  private:
    byte _pinData;
    byte _pinClock;
    byte _pinLoad;

    byte* _buffer;
    byte _screens;
    byte _maximumX;

    void putByte(byte);
    void setRegister(byte, byte);
    void syncRow(int);

    void setScanLimit(byte);

    void buffer(int, int, byte);
  public:
    Matrix(byte, byte, byte, byte = 1);
    void setBrightness(byte);
    void write(int, int, byte);
    void write(int, int, Sprite);
    void clear(void);
};

#endif

