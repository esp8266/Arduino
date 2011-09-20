/*
 %atmel_license%
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
