
/*
 * UART objects
 */
ring_buffer rx_buffer1  =  { { 0 }, 0, 0 } ;
ring_buffer tx_buffer1  =  { { 0 }, 0, 0 } ;
ring_buffer rx_buffer2  =  { { 0 }, 0, 0 } ;
ring_buffer tx_buffer2  =  { { 0 }, 0, 0 } ;

UARTClass Serial1( &rx_buffer1, &tx_buffer1, UART0, UART0_IRQn, ID_UART0 ) ;
UARTClass Serial2( &rx_buffer2, &tx_buffer2, UART1, UART1_IRQn, ID_UART1 ) ;

// IT handlers
extern void UART0_IrqHandler( void )
{
  Serial1.IrqHandler() ;
}

extern void UART1_IrqHandler( void )
{
  Serial2.IrqHandler() ;
}


// ----------------------------------------------------------------------------


/*
 * USART objects
 */
ring_buffer rx_buffer3  =  { { 0 }, 0, 0 } ;
ring_buffer tx_buffer3  =  { { 0 }, 0, 0 } ;
ring_buffer rx_buffer4  =  { { 0 }, 0, 0 } ;
ring_buffer tx_buffer4  =  { { 0 }, 0, 0 } ;

USARTClass Serial3( &rx_buffer3, &tx_buffer3, USART0, USART0_IRQn, ID_USART0 ) ;
USARTClass Serial4( &rx_buffer4, &tx_buffer4, USART1, USART1_IRQn, ID_USART1 ) ;

// IT handlers
extern void USART0_IrqHandler( void )
{
  Serial3.IrqHandler() ;
}

extern void USART1_IrqHandler( void )
{
  Serial4.IrqHandler() ;
}

// ----------------------------------------------------------------------------

