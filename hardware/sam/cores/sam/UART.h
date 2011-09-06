#ifndef _UART_
#define _UART_

// UART.cpp need this class to be predefined
class UARTClass ;

#include "wiring_private.h"

class UARTClass : public HardwareSerial
{
  protected:
    Uart* _pUart ;
    IRQn_Type _dwIrq ;
    uint32_t _dwId ;

  public:
    UARTClass( Uart* pUart, IRQn_Type dwIrq, uint32_t dwId, ring_buffer* pRx_buffer, ring_buffer *pTx_buffer ) ;

    void begin( const uint32_t dwBaudRate ) ;
    void end( void ) ;
    int available( void ) ;
    int peek( void ) ;
    int read( void ) ;
    void flush( void ) ;
    void write( const uint8_t c ) ;

    void IrqHandler( void ) ;

    virtual void write( const char *str ) ;
    virtual void write( const uint8_t *buffer, size_t size ) ;
//    using Print::write ; // pull in write(str) and write(buf, size) from Print
};

#endif // _UART_
