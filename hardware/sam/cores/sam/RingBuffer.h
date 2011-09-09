/*
 %atmel_license%
*/

#ifndef _RING_BUFFER_
#define _RING_BUFFER_

#include <stdint.h>

// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which head is the index of the location
// to which to write the next incoming character and tail is the index of the
// location from which to read.
#define SERIAL_BUFFER_SIZE 64

class RingBuffer
{
  public:
    uint8_t _aucBuffer[SERIAL_BUFFER_SIZE] ;
    int _iHead ;
    int _iTail ;

  public:
    RingBuffer( void ) ;
    void store_char( uint8_t c ) ;
} ;

#endif /* _RING_BUFFER_ */
