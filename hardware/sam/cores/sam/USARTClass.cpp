/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "USARTClass.h"

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

  /* Enable UART interrupt in NVIC */
//  NVIC_EnableIRQ( _dwIrq ) ;

  /* Enable receiver and transmitter */
  _pUsart->US_CR = US_CR_RXEN | US_CR_TXEN ;
}

void USARTClass::end( void )
{
  // wait for transmission of outgoing data
  //while ( _tx_buffer->_iHead != _tx_buffer->_iTail )
  //{
  //}

  // clear any received data
  _rx_buffer->_iHead = _rx_buffer->_iTail ;

  PMC_DisablePeripheral( _dwId ) ;
}

int USARTClass::available( void )
{
  return (uint32_t)(SERIAL_BUFFER_SIZE + _rx_buffer->_iHead - _rx_buffer->_iTail) % SERIAL_BUFFER_SIZE ;
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
  // if the head isn't ahead of the tail, we don't have any characters
  if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
  {
    return -1 ;
  }
  else
  {
    uint8_t uc = _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;

    _rx_buffer->_iTail = (unsigned int)(_rx_buffer->_iTail + 1) % SERIAL_BUFFER_SIZE ;

    return uc ;
  }
}

void USARTClass::flush( void )
{
  //while ( _tx_buffer->_iHead != _tx_buffer->_iTail )
  //{
  //}
}

void USARTClass::write( const uint8_t uc_data )
{
  /* Check if the transmitter is ready */
  if ( (_pUsart->US_CSR & US_CSR_TXRDY) != US_CSR_TXRDY )
  {
    return ;
  }

  /* Send character */
  _pUsart->US_THR=uc_data ;
}

void USARTClass::IrqHandler( void )
{
  /* Did we receive data ? */
  if ( (_pUsart->US_IER & US_IER_ENDRX) == US_IER_ENDRX )
  {
    _rx_buffer->store_char( _pUsart->US_RHR ) ;
  }
}

