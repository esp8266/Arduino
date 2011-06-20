
/*
 * UART objects
 */
ring_buffer rx_buffer0  =  { { 0 }, 0, 0 } ;
ring_buffer tx_buffer0  =  { { 0 }, 0, 0 } ;
ring_buffer rx_buffer1  =  { { 0 }, 0, 0 } ;
ring_buffer tx_buffer1  =  { { 0 }, 0, 0 } ;

UARTClass Serial( &rx_buffer0, &tx_buffer0, UART0, UART0_IRQn, ID_UART0 ) ;
UARTClass UART1( &rx_buffer1, &tx_buffer1, UART1, UART1_IRQn, ID_UART1 ) ;

// IT handlers
extern void UART0_IrqHandler( void )
{
  Serial.IrqHandler() ;
}

extern void UART1_IrqHandler( void )
{
  UART1.IrqHandler() ;
}


// ----------------------------------------------------------------------------


/*
 * USART objects
 */
ring_buffer rx_buffer2  =  { { 0 }, 0, 0 } ;
ring_buffer tx_buffer2  =  { { 0 }, 0, 0 } ;
ring_buffer rx_buffer3  =  { { 0 }, 0, 0 } ;
ring_buffer tx_buffer3  =  { { 0 }, 0, 0 } ;

USARTClass USART1( &rx_buffer2, &tx_buffer2, USART0, USART0_IRQn, ID_USART0 ) ;
USARTClass USART2( &rx_buffer3, &tx_buffer3, USART1, USART1_IRQn, ID_USART1 ) ;

// IT handlers
extern void USART0_IrqHandler( void )
{
  USART0.IrqHandler() ;
}

extern void USART1_IrqHandler( void )
{
  USART1.IrqHandler() ;
}

// ----------------------------------------------------------------------------

