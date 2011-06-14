#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UART.h"


// Constructors ////////////////////////////////////////////////////////////////

UARTClass::UARTClass( ring_buffer* rx_buffer, ring_buffer* tx_buffer, Uart* pUart, IRQn_Type dwIrq, uint32_t dwId )
{
  _rx_buffer = rx_buffer ;
  _tx_buffer = tx_buffer ;

  _pUart=pUart ;
  _dwIrq=dwIrq ;
  _dwId=dwId ;
}

// Public Methods //////////////////////////////////////////////////////////////

void UARTClass::begin( const uint32_t dwBaudRate )
{
  /* Configure PMC */
  PMC_EnablePeripheral( _dwId ) ;

  /* Reset and disable receiver & transmitter */
  _pUart->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS ;

  /* Configure mode */
  _pUart->UART_MR = UART_MR_PAR_NO ;

  /* Configure baudrate */
  /* Asynchronous, no oversampling */
  _pUart->UART_BRGR = (BOARD_MCK / dwBaudRate) / 16 ;

  /* Disable PDC channel */
  _pUart->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS ;

  /* Enable receiver and transmitter */
  _pUart->UART_CR = UART_CR_RXEN | UART_CR_TXEN ;
}

void UARTClass::end( void )
{
  // wait for transmission of outgoing data
  while ( _tx_buffer->head != _tx_buffer->tail )
  {
  }

  // clear any received data
  _rx_buffer->head = _rx_buffer->tail ;

  PMC_DisablePeripheral( _dwId ) ;
}

int UARTClass::available( void )
{
  return (unsigned int)(SERIAL_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % SERIAL_BUFFER_SIZE ;
}

int UARTClass::peek( void )
{
  if ( _rx_buffer->head == _rx_buffer->tail )
  {
    return -1 ;
  }
  else
  {
    return _rx_buffer->buffer[_rx_buffer->tail] ;
  }
}

int UARTClass::read( void )
{
  // if the head isn't ahead of the tail, we don't have any characters
  if ( _rx_buffer->head == _rx_buffer->tail )
  {
    return -1 ;
  }
  else
  {
    unsigned char c = _rx_buffer->buffer[_rx_buffer->tail] ;

    _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE ;

    return c ;
  }
}

void UARTClass::flush( void )
{
  while ( _tx_buffer->head != _tx_buffer->tail )
  {
  }
}

void UARTClass::write( const uint8_t c )
{
  int i = (_tx_buffer->head + 1) % SERIAL_BUFFER_SIZE ;

  // If the output buffer is full, there's nothing for it other than to
  // wait for the interrupt handler to empty it a bit
  while ( i == _tx_buffer->tail )
  {
  }

  _tx_buffer->buffer[_tx_buffer->head] = c ;
  _tx_buffer->head = i ;

  /* Wait for the transmitter to be ready */
  while ( (_pUart->UART_SR & UART_SR_TXEMPTY) == 0 ) ;

  /* Send character */
  _pUart->UART_THR=c ;
}

void UARTClass::IrqHandler( void )
{
/*
  // RX char IT
  unsigned char c = _pUart->UART_RHR ;
  store_char(c, &rx_buffer3);

  // TX FIFO empty IT
  if ( _tx_buffer->head == _tx_buffer->tail )
  {
    // Buffer empty, so disable interrupts
  }
  else
  {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = _tx_buffer->buffer[_tx_buffer->tail] ;
    _tx_buffer->tail = (_tx_buffer->tail + 1) % SERIAL_BUFFER_SIZE ;
	
    _pUart->UART_THR = c ;
  }
*/
}

