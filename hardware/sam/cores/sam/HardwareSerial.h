#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <stdint.h>
#include "Stream.h"

// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which head is the index of the location
// to which to write the next incoming character and tail is the index of the
// location from which to read.
#define SERIAL_BUFFER_SIZE 64

typedef struct _ring_buffer
{
  uint8_t buffer[SERIAL_BUFFER_SIZE] ;
  volatile int head ;
  volatile int tail ;
} ring_buffer ;

class HardwareSerial : public Stream
{
  protected:
    ring_buffer *_rx_buffer ;
    ring_buffer *_tx_buffer ;

  public:
    virtual void begin( const uint32_t dwBaudRate ) =0 ;
    virtual void end( void ) =0 ;
    virtual int available( void ) =0 ;
    virtual int peek( void ) =0 ;
    virtual int read( void ) =0 ;
    virtual void flush( void ) =0 ;
    virtual void write( const uint8_t c ) =0 ;

    using Print::write ; // pull in write(str) and write(buf, size) from Print
} ;

// Complementary API
extern void store_char( uint8_t c, ring_buffer* pBuffer ) ;

#endif // HardwareSerial_h
