/*
  Matrix.h - Max7219 LED Matrix library file for Wiring
  Part of the Wiring project - http://wiring.org.co

  Copyright (c) 2004-2005 Hernando Barragan

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Max7219 LED Matrix library by Nicholas Zambetti 
*/


#ifndef Matrix_h
#define Matrix_h

#include <inttypes.h>

class Matrix
{
  private:
    uint8_t _pinClock;
    uint8_t _pinLoad;
    uint8_t _pinData;

    uint8_t _screenBuffer[8];

    void putByte(uint8_t);

  public:
    Matrix(uint8_t, uint8_t, uint8_t);
    void set(uint8_t, uint8_t);
    void setAll(uint8_t);
    void setScanLimit(uint8_t);
    void setIntensity(uint8_t);
    void storePixel(uint8_t, uint8_t, uint8_t);
    void updatePixels(void);
};

#endif

