#ifndef _UART_
#define _UART_

#include <inttypes.h>
#include "wiring_private.h"

class UARTClass : public HardwareSerial
{
  protected:
    Uart* _pUart ;
    IRQn_Type _dwIrq ;
    uint32_t _dwId ;

  public:
    UARTClass( ring_buffer *rx_buffer, ring_buffer *tx_buffer, Uart* pUart, IRQn_Type dwIrq, uint32_t dwId ) ;

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

#endif // _UART_
