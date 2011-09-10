#ifndef _USART_CLASS_
#define _USART_CLASS_

// USART.cpp need this class to be predefined
//class USARTClass ;

#include "Arduino.h"

class USARTClass : public HardwareSerial
{
  protected:
    Usart* _pUsart ;
    IRQn_Type _dwIrq ;
    uint32_t _dwId ;

  public:
    USARTClass( Usart* pUsart, IRQn_Type dwIrq, uint32_t dwId, RingBuffer* pRx_buffer, RingBuffer* pTx_buffer ) ;

    void begin( const uint32_t dwBaudRate ) ;
    void end( void ) ;
    int available( void ) ;
    int peek( void ) ;
    int read( void ) ;
    void flush( void ) ;
    void write( const uint8_t c ) ;

    void IrqHandler( void ) ;

#if defined __GNUC__ /* GCC CS3 */
    using Print::write ; // pull in write(str) and write(buf, size) from Print
#elif defined __ICCARM__ /* IAR Ewarm 5.41+ */
    virtual void write( const char *str ) ;
    virtual void write( const uint8_t *buffer, size_t size ) ;
#endif
};

#endif // _USART_CLASS_
