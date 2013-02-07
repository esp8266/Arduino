#include "CANRaw.h"
#include "sn65hvd234.h"

CANRawClass::CANRawClass( Can* pCan )
{
  m_pCan=pCan ;
}

uint32_t CANRawClass::begin( uint32_t dwPinEN, uint32_t dwPinRs, uint32_t dwBaudrate )
{
  uint32_t dwPins ;

  /* Initialize CAN pins */
  if ( m_pCan == CAN0 )
  {
    dwPins=PINS_CAN0 ;
  }
  else
  {
    dwPins=PINS_CAN1 ;
  }

  PIO_Configure(
    g_APinDescription[dwPins].pPort,
    g_APinDescription[dwPins].ulPinType,
    g_APinDescription[dwPins].ulPin,
    g_APinDescription[dwPins].ulPinConfiguration);

  /* Initialize CAN Transceiver */
  SN65HVD234_Init( &m_Transceiver ) ;
  SN65HVD234_SetRs( &m_Transceiver, dwPinRs ) ;
  SN65HVD234_SetEN( &m_Transceiver, dwPinEN ) ;
  /* Enable CAN Transceiver */
  SN65HVD234_DisableLowPower( &m_Transceiver ) ;
  SN65HVD234_Enable( &m_Transceiver ) ;

  if ( can_init( m_pCan, SystemCoreClock, 1000 ) == 1 )
  {
    can_reset_all_mailbox( m_pCan ) ;
  }
  else
  {
    puts( "ERROR CAN initialisation (synchro)" ) ;

    return -1 ;
  }

  return 0 ;
}

uint32_t CANRawClass::end( void )
{
  /* Disable CAN Controller */
  can_disable( m_pCan ) ;

  /* Disable CAN Transceiver */
  SN65HVD234_EnableLowPower( &m_Transceiver ) ;
  SN65HVD234_Disable( &m_Transceiver ) ;

  return 0 ;
}

/*----------------------------------------------------------------------------
 *        Exported objects
 *----------------------------------------------------------------------------*/
CANRawClass CANRaw0( CAN0 ) ;
CANRawClass CANRaw1( CAN1 ) ;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Default interrupt handler for CAN 0.
 */
void CAN0_IrqHandler( void )
{
  CAN_Handler( CAN0, &gCanTransfer0 ) ;
}

/**
 * \brief Default interrupt handler for CAN 1.
 */
void CAN1_IrqHandler( void )
{
  CAN_Handler( CAN1, &gCanTransfer1 ) ;
}


