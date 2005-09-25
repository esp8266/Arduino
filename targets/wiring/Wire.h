/*
  Wire.h - TWI/I2C library file for Wiring
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

  Wire TWI/I2C library by Nicholas Zambetti 
*/


#ifndef Wire_h
#define Wire_h

#include <inttypes.h>

#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

#ifndef TWI_BUFFERSIZE  
#define TWI_BUFFERSIZE 32
#endif

class Wire
{
  private:
    static volatile uint8_t _state;
    static uint8_t _slarw;

    static uint8_t _masterBuffer[TWI_BUFFERSIZE];
    static uint8_t _masterBufferLength;
    static volatile uint8_t _masterBufferIndex;

    static void (*Wire::_onSlaveTransmit)(void);
    static void (*Wire::_onSlaveReceive)(int);

    static uint8_t _txBuffer[TWI_BUFFERSIZE];
    static volatile uint8_t _txBufferIndex;
    static volatile uint8_t _txBufferLength;
    
    static uint8_t _rxBuffer[TWI_BUFFERSIZE];
    static volatile uint8_t _rxBufferIndex;

    uint8_t readFrom(uint8_t, uint8_t*, uint8_t);
    uint8_t writeTo(uint8_t, uint8_t*, uint8_t, uint8_t);
    static void reply(uint8_t);
    static void stop(void);
    static void releaseBus(void);
    static void interruptLogic(void);
  public:
    Wire(uint8_t);
    uint8_t read(int);
    void readBytes(int, uint8_t*, int);
    void readBytes(int, char*, int);
    void write(int, int);
    void writeBytes(int, uint8_t*, int);
    void writeBytes(int, char*);
    void transmit(uint8_t);
    void transmitBytes(uint8_t*, uint8_t);
    void transmitBytes(char*);
    uint8_t receive(void);
    void receiveBytes(uint8_t*);
    void receiveBytes(char*);
    void onDataIn( void (*)(int) );
    void onDataOut( void (*)(void) );
};

#endif

