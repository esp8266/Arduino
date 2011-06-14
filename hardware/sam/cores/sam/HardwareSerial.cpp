#include "wiring_private.h"

#include "HardwareSerial.h"

inline void store_char( uint8_t c, ring_buffer *pBuffer )
{
  int i = (unsigned int)(pBuffer->head + 1) % SERIAL_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if ( i != pBuffer->tail )
  {
    pBuffer->buffer[pBuffer->head] = c ;
    pBuffer->head = i ;
  }
}
