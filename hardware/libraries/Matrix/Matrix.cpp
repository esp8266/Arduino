/*
  Matrix.cpp - Max7219 LED Matrix library for Arduino & Wiring
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

// TODO: Support segment displays in api?
// TODO: Support varying vendor layouts?

/******************************************************************************
 * Includes
 ******************************************************************************/

extern "C" {
  // AVR LibC Includes
  #include <inttypes.h>
  #include <stdlib.h>

  // Wiring Core Includes
  #undef abs
  #include "WConstants.h"

  // Wiring Core Prototypes
  //void pinMode(uint8_t, uint8_t);
  //void digitalWrite(int, uint8_t);
}

#include "Sprite.h"
#include "Matrix.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

// Matrix registers
#define REG_NOOP   0x00
#define REG_DIGIT0 0x01
#define REG_DIGIT1 0x02
#define REG_DIGIT2 0x03
#define REG_DIGIT3 0x04
#define REG_DIGIT4 0x05
#define REG_DIGIT5 0x06
#define REG_DIGIT6 0x07
#define REG_DIGIT7 0x08
#define REG_DECODEMODE  0x09
#define REG_INTENSITY   0x0A
#define REG_SCANLIMIT   0x0B
#define REG_SHUTDOWN    0x0C
#define REG_DISPLAYTEST 0x0F

/******************************************************************************
 * Constructors
 ******************************************************************************/

Matrix::Matrix(uint8_t data, uint8_t clock, uint8_t load, uint8_t screens /* = 1 */)
{
  // record pins for sw spi
  _pinData = data;
  _pinClock = clock;
  _pinLoad = load;

  // set ddr for sw spi pins
  pinMode(_pinClock, OUTPUT);
  pinMode(_pinData, OUTPUT);
  pinMode(_pinLoad, OUTPUT);

  // allocate screenbuffers
  _screens = screens;
  _buffer = (uint8_t*)calloc(_screens, 64);
  _maximumX = (_screens * 8);

  // initialize registers
  clear();             // clear display
  setScanLimit(0x07);  // use all rows/digits
  setBrightness(0x0F); // maximum brightness
  setRegister(REG_SHUTDOWN, 0x01);    // normal operation
  setRegister(REG_DECODEMODE, 0x00);  // pixels not integers
  setRegister(REG_DISPLAYTEST, 0x00); // not in test mode
}

/******************************************************************************
 * MAX7219 SPI
 ******************************************************************************/

// sends a single byte by sw spi (no latching)
void Matrix::putByte(uint8_t data)
{
  uint8_t i = 8;
  uint8_t mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);         // get bitmask
    digitalWrite(_pinClock, LOW);   // tick
    if (data & mask){               // choose bit
      digitalWrite(_pinData, HIGH); // set 1
    }else{
      digitalWrite(_pinData, LOW);  // set 0
    }
    digitalWrite(_pinClock, HIGH);  // tock
    --i;                            // move to lesser bit
  }
}

// sets register to a byte value for all screens
void Matrix::setRegister(uint8_t reg, uint8_t data)
{
  digitalWrite(_pinLoad, LOW); // begin
  for(uint8_t i = 0; i < _screens; ++i){
    putByte(reg);  // specify register
    putByte(data); // send data
  }
  digitalWrite(_pinLoad, HIGH);  // latch in data
  digitalWrite(_pinLoad, LOW); // end
}

// syncs row of display with buffer
void Matrix::syncRow(uint8_t row)
{
  if (!_buffer) return;
  
  // uint8_t's can't be negative, so don't test for negative row
  if (row >= 8) return;
  digitalWrite(_pinLoad, LOW); // begin
  for(uint8_t i = 0; i < _screens; ++i){
    putByte(8 - row);                // specify register
    putByte(_buffer[row + (8 * i)]); // send data
  }
  digitalWrite(_pinLoad, HIGH);  // latch in data
  digitalWrite(_pinLoad, LOW); // end
}

/******************************************************************************
 * MAX7219 Configuration
 ******************************************************************************/

// sets how many digits are displayed
void Matrix::setScanLimit(uint8_t value)
{
  setRegister(REG_SCANLIMIT, value & 0x07);
}

// sets brightness of the display
void Matrix::setBrightness(uint8_t value)
{
  setRegister(REG_INTENSITY, value & 0x0F);
}

/******************************************************************************
 * Helper Functions
 ******************************************************************************/

void Matrix::buffer(uint8_t x, uint8_t y, uint8_t value)
{
  if (!_buffer) return;
  
  // uint8_t's can't be negative, so don't test for negative x and y.
  if (x >= _maximumX || y >= 8) return;

  uint8_t offset = x; // record x
  x %= 8;             // make x relative to a single matrix
  offset -= x;        // calculate buffer offset

  // wrap shift relative x for nexus module layout
  if (x == 0){
    x = 8;
  }
  --x;

  // record value in buffer
  if(value){
    _buffer[y + offset] |= 0x01 << x;
  }else{
    _buffer[y + offset] &= ~(0x01 << x);
  }
}

/******************************************************************************
 * User API
 ******************************************************************************/

// buffers and writes to screen
void Matrix::write(uint8_t x, uint8_t y, uint8_t value)
{
  buffer(x, y, value);
  
  // update affected row
  syncRow(y);
}

void Matrix::write(uint8_t x, uint8_t y, Sprite sprite)
{
  for (uint8_t i = 0; i < sprite.height(); i++){
    for (uint8_t j = 0; j < sprite.width(); j++)
      buffer(x + j, y + i, sprite.read(j, i));
      
    syncRow(y + i);
  }
}

// clears screens and buffers
void Matrix::clear(void)
{
  if (!_buffer) return;

  // clear buffer
  for(uint8_t i = 0; i < 8; ++i){
    for(uint8_t j = 0; j < _screens; ++j){
      _buffer[i + (8 * j)] = 0x00;
    }
  }

  // clear registers
  for(uint8_t i = 0; i < 8; ++i){
    syncRow(i);
  }
}

