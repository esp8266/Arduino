#ifndef _USART_
#define _USART_

// USART.cpp need this class to be predefined
class USARTClass ;

#include "wiring_private.h"

class USARTClass : public HardwareSerial
{
  protected:
    Usart* _pUsart ;
    IRQn_Type _dwIrq ;
    uint32_t _dwId ;

  public:
    USARTClass( Usart* pUsart, IRQn_Type dwIrq, uint32_t dwId, ring_buffer* pRx_buffer, ring_buffer *pTx_buffer ) ;

    void begin( const uint32_t dwBaudRate ) ;
    void end( void ) ;
    int available( void ) ;
    int peek( void ) ;
    int read( void ) ;
    void flush( void ) ;
    void write( const uint8_t c ) ;

    void IrqHandler( void ) ;

    using Print::write ; // pull in write(str) and write(buf, size) from Print
};

#endif // _USART_
