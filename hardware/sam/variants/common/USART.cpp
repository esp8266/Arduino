#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "USART.h"

// Constructors ////////////////////////////////////////////////////////////////

USARTClass::USARTClass( Usart* pUsart, IRQn_Type dwIrq, uint32_t dwId, RingBuffer* pRx_buffer, RingBuffer* pTx_buffer )
{
  _rx_buffer = pRx_buffer ;
  _tx_buffer = pTx_buffer ;

  _pUsart=pUsart ;
  _dwIrq=dwIrq ;
  _dwId=dwId ;
}

// Public Methods //////////////////////////////////////////////////////////////

void USARTClass::begin( const uint32_t dwBaudRate )
{
  /* Configure PMC */
  PMC_EnablePeripheral( _dwId ) ;

  /* Reset and disable receiver & transmitter */
  _pUsart->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS ;

  /* Configure mode */
  _pUsart->US_MR = US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK | US_MR_CHRL_8_BIT | US_MR_PAR_NO |
                   US_MR_NBSTOP_1_BIT | US_MR_CHMODE_NORMAL;

  /* Configure baudrate */
  /* Asynchronous, no oversampling */
  _pUsart->US_BRGR = (SystemCoreClock / dwBaudRate) / 16 ;

  /* Disable PDC channel */
  _pUsart->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS ;

  /* Enable receiver and transmitter */
  _pUsart->US_CR = US_CR_RXEN | US_CR_TXEN ;
}

void USARTClass::end()
{
  // wait for transmission of outgoing data
  while ( _tx_buffer->_iHead != _tx_buffer->_iTail )
  {
  }

  // clear any received data
  _rx_buffer->_iHead = _rx_buffer->_iTail ;

  PMC_DisablePeripheral( _dwId ) ;
}

int USARTClass::available( void )
{
  return (unsigned int)(SERIAL_BUFFER_SIZE + _rx_buffer->_iHead - _rx_buffer->_iTail) % SERIAL_BUFFER_SIZE ;
}

int USARTClass::peek( void )
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

int USARTClass::read( void )
{
  // if the _iHead isn't ahead of the _iTail, we don't have any characters
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

void USARTClass::flush( void )
{
  while ( _tx_buffer->_iHead != _tx_buffer->_iTail )
  {
  }
}

void USARTClass::write( uint8_t uc )
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
  while ( (_pUsart->US_CSR & US_CSR_TXEMPTY) == 0 ) ;

  /* Send character */
  _pUsart->US_THR=uc ;
}

void USARTClass::IrqHandler( void )
{
  // RX char IT
  uint8_t uc = _pUsart->US_RHR ;

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
	
    _pUsart->US_THR = uc ;
  }
}

