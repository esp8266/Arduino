/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <stdint.h>
#include "Stream.h"
#include "RingBuffer.h"

class HardwareSerial : public Stream
{
  protected:
    RingBuffer *_rx_buffer ;
    RingBuffer *_tx_buffer ;

  public:
    virtual void begin( const uint32_t dwBaudRate ) =0 ;
    virtual void end( void ) =0 ;
    virtual int available( void ) =0 ;
    virtual int peek( void ) =0 ;
    virtual int read( void ) =0 ;
    virtual void flush( void ) =0 ;
    virtual void write( const uint8_t c ) =0 ;

//    virtual void write( const char *str ) ;
//    virtual void write( const uint8_t *buffer, size_t size ) ;
    using Print::write ; // pull in write(str) and write(buf, size) from Print
} ;

#endif // HardwareSerial_h
