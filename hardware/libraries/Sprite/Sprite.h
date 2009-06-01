/*
  Sprite.cpp - 2D sprite buffers library for Arduino & Wiring
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
*/

#ifndef Sprite_h
#define Sprite_h

#include <inttypes.h>

#include "binary.h"

class Sprite
{
  private:
    uint8_t _width;
    uint8_t _height;
    uint8_t _depth;
    uint8_t _ppb;
    uint8_t _bpr;
    uint8_t _mask;
    uint8_t *_buffer;
    
    void init(uint8_t width, uint8_t height);
  public: 
    Sprite(uint8_t width, uint8_t height);
    Sprite(uint8_t width, uint8_t height, uint8_t row, ...);
    uint8_t width() const;
    uint8_t height() const;
    void write(uint8_t x, uint8_t y, uint8_t value);
    uint8_t read(uint8_t x, uint8_t y) const;
};

#endif
