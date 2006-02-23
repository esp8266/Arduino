/*
  Sprite.cpp - 2D sprite buffer library for Arduino & Wiring
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

#include <stdlib.h>
#include <stdarg.h>
//#include <stdio.h>

#include "Sprite.h"

void Sprite::init(uint8_t width, uint8_t height, uint8_t depth)
{
  _width = width;
  _height = height;
  _ppb = 8 / depth; // pixels per byte = 8 / bit depth
  _depth = 8 / _ppb; // round bit depth up to nearest divisor of 8
  _bpr = (_width + _ppb - 1) / _ppb; // bytes per row (rounding up)
  _mask = (1 << _depth) - 1; // mask = a byte with 1's in the _depth LSBs

  // _buffer packs _ppb pixels into each byte.  
  // for example, Sprite(3, 2, 3) yields:
  // _buffer[0] = LSN: pixel (0, 0); MSN: pixel (1, 0)
  // _buffer[1] = LSN: pixel (2, 0); MSN: empty
  // _buffer[2] = LSN: pixel (0, 1); MSN: pixel (1, 1)
  // _buffer[3] = LSN: pixel (2, 1); MSN: empty
  // LSN = least-significant nibble (bits 0 to 3),
  // MSN = most-significant nibble (bits 4 to 7)

  // for now, do nothing if this allocation fails.  methods that require it
  // should silently fail if _buffer is null.
  _buffer = (uint8_t *) calloc(_height * _bpr, 1);
}
  
Sprite::Sprite(uint8_t width, uint8_t height, uint8_t depth)
{
  init(width, height, depth);
}

Sprite::Sprite(uint8_t width, uint8_t height, uint8_t depth, uint8_t row, ...)
{
  init(width, height, depth);
  
  if (!_buffer) return;
  
  va_list ap;
  va_start(ap, row);

  int y = 0;
  
  for  (y = 0; ; y++) {
    for (int x = 0; x < width && x < 8; x++)
      write(x, y, (row >> (width - x - 1)) & 0x01);
    
    if (y == height - 1)
      break;
      
    row = va_arg(ap, int); // using '...' promotes uint8_t to int
  }
  
  va_end(ap);
}

uint8_t Sprite::width() const
{
  return _width;
}

uint8_t Sprite::height() const
{
  return _height;
}

void Sprite::write(uint8_t x, uint8_t y, uint8_t value)
{
  if (!_buffer) return;
  
  // XXX: this should mask according to bit depth specified in constructor,
  // not the rounded bit depth stored in _depth
  
  // (x, y) is in the x / _ppb byte of the y'th row (the y'th row starts at
  // byte y * _bpr of _buffer).  this byte may contain more than one pixel.
  uint8_t *p = _buffer + y * _bpr + x / _ppb;
  
  // pixel (x, y) is in position x % _ppb of the byte, and each position is
  // _depth bits.
  uint8_t offset = _depth * (x % _ppb);
  
  // we need to bitwise-or the value of the other pixels in the byte with
  // the new value, masked and shifted into the proper bits.
  *p = (*p & ~(_mask << offset)) | ((value & _mask) << offset);
}

uint8_t Sprite::read(uint8_t x, uint8_t y) const
{
  if (!_buffer) return 0;
  
  // (x, y) is in the x / _ppb byte of the y'th row (the y'th row starts at
  // byte y * _bpr of _buffer).  this byte may contain more than one pixel.
  uint8_t *p = _buffer + y * _bpr + x / _ppb;
  
  // pixel (x, y) is in position x % _ppb of the byte, and each position is
  // _depth bits.
  uint8_t offset = _depth * (x % _ppb);
  
  return (*p >> offset) & _mask;
}