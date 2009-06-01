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

#include <inttypes.h>

class Sprite;

class Matrix
{
  private:
    uint8_t _pinData;
    uint8_t _pinClock;
    uint8_t _pinLoad;

    uint8_t* _buffer;
    uint8_t _screens;
    uint8_t _maximumX;

    void putByte(uint8_t);
    void setRegister(uint8_t, uint8_t);
    void syncRow(uint8_t);

    void setScanLimit(uint8_t);

    void buffer(uint8_t, uint8_t, uint8_t);
  public:
    Matrix(uint8_t, uint8_t, uint8_t, uint8_t = 1);
    void setBrightness(uint8_t);
    void write(uint8_t, uint8_t, uint8_t);
    void write(uint8_t, uint8_t, Sprite);
    void clear(void);
};

#endif

