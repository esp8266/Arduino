#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "USART.h"

// Constructors ////////////////////////////////////////////////////////////////

USARTClass::USARTClass( ring_buffer *rx_buffer, ring_buffer *tx_buffer, Usart* pUsart, IRQn_Type dwIrq, uint32_t dwId )
{
  _rx_buffer = rx_buffer ;
  _tx_buffer = tx_buffer ;

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
  _pUsart->US_BRGR = (BOARD_MCK / dwBaudRate) / 16 ;

  /* Disable PDC channel */
  _pUsart->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS ;

  /* Enable receiver and transmitter */
  _pUsart->US_CR = US_CR_RXEN | US_CR_TXEN ;
}

void USARTClass::end()
{
}

int USARTClass::available( void )
{
  return 0 ;
}

int USARTClass::peek( void )
{
  return 0 ;
}

int USARTClass::read( void )
{
  return 0 ;
}

void USARTClass::flush( void )
{
}

void USARTClass::write( uint8_t c )
{
}

void USARTClass::IrqHandler( void )
{
}

