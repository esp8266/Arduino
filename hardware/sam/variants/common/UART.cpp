/*
 %atmel_license%
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UART.h"

// Constructors ////////////////////////////////////////////////////////////////

UARTClass::UARTClass( Uart* pUart, IRQn_Type dwIrq, uint32_t dwId, RingBuffer* pRx_buffer, RingBuffer* pTx_buffer )
{
  _rx_buffer = pRx_buffer ;
  _tx_buffer = pTx_buffer ;

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
  _pUart->UART_BRGR = (SystemCoreClock / dwBaudRate) / 16 ;

  /* Disable PDC channel */
  _pUart->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS ;

  /* Enable receiver and transmitter */
  _pUart->UART_CR = UART_CR_RXEN | UART_CR_TXEN ;
}

void UARTClass::end( void )
{
  // wait for transmission of outgoing data
  while ( _tx_buffer->_iHead != _tx_buffer->_iTail )
  {
  }

  // clear any received data
  _rx_buffer->_iHead = _rx_buffer->_iTail ;

  PMC_DisablePeripheral( _dwId ) ;
}

int UARTClass::available( void )
{
  return (uint32_t)(SERIAL_BUFFER_SIZE + _rx_buffer->_iHead - _rx_buffer->_iTail) % SERIAL_BUFFER_SIZE ;
}

int UARTClass::peek( void )
{
  if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
  {
    return -1 ;
  }
  else
  {
    return _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;
  }
}

int UARTClass::read( void )
{
  // if the head isn't ahead of the _iTail, we don't have any characters
  if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
  {
    return -1 ;
  }
  else
  {
    unsigned char c = _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;

    _rx_buffer->_iTail = (unsigned int)(_rx_buffer->_iTail + 1) % SERIAL_BUFFER_SIZE ;

    return c ;
  }
}

void UARTClass::flush( void )
{
  while ( _tx_buffer->_iHead != _tx_buffer->_iTail )
  {
  }
}

void UARTClass::write( const uint8_t uc )
{
  int i = (_tx_buffer->_iHead + 1) % SERIAL_BUFFER_SIZE ;

  // If the output buffer is full, there's nothing for it other than to
  // wait for the interrupt handler to empty it a bit
  while ( i == _tx_buffer->_iTail )
  {
  }

  _tx_buffer->_aucBuffer[_tx_buffer->_iHead] = uc ;
  _tx_buffer->_iHead = i ;

  /* Wait for the transmitter to be ready */
  while ( (_pUart->UART_SR & UART_SR_TXEMPTY) == 0 ) ;

  /* Send character */
  _pUart->UART_THR=uc ;
}

void UARTClass::IrqHandler( void )
{
  // RX char IT
  uint8_t uc = _pUart->UART_RHR ;
  _rx_buffer->store_char( uc ) ;

  // TX FIFO empty IT
  if ( _tx_buffer->_iHead == _tx_buffer->_iTail )
  {
    // Buffer empty, so disable interrupts
  }
  else
  {
    // There is more data in the output buffer. Send the next byte
    uc = _tx_buffer->_aucBuffer[_tx_buffer->_iTail] ;
    _tx_buffer->_iTail = (_tx_buffer->_iTail + 1) % SERIAL_BUFFER_SIZE ;

    _pUart->UART_THR = uc ;
  }
}

