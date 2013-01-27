#include "CAN.h"
#include "sn65hvd234.h"

/*----------------------------------------------------------------------------
 *        Variables
 *----------------------------------------------------------------------------*/

/* CAN0 Transceiver */
SSN65HVD234_Data gCanTransceiver0 ;
/* CAN1 Transceiver */
SSN65HVD234_Data gCanTransceiver1 ;

/* CAN0 Transfer */
SCanTransfer gCanTransfer0 ;
/* CAN1 Transfer */
SCanTransfer gCanTransfer1 ;

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

uint32_t CAN_InitShieldHardware( uint32_t )
{
  // Initialize CAN0 pins
  PIO_Configure(
    g_APinDescription[PINS_CAN0].pPort,
    g_APinDescription[PINS_CAN0].ulPinType,
    g_APinDescription[PINS_CAN0].ulPin,
    g_APinDescription[PINS_CAN0].ulPinConfiguration);

  /* Initialize CAN0 Transceiver */
  SN65HVD234_Init( &gCanTransceiver0 ) ;
  SN65HVD234_SetRs( &gCanTransceiver0, PIOB, PIO_PB20 ) ;
  SN65HVD234_SetEN( &gCanTransceiver0, PIOB, PIO_PB21 ) ;
  /* Enable CAN0 Transceiver */
  SN65HVD234_DisableLowPower( &gCanTransceiver0 ) ;
  SN65HVD234_Enable( &gCanTransceiver0 ) ;

  // Initialize CAN1 pins
  PIO_Configure(
    g_APinDescription[PINS_CAN1].pPort,
    g_APinDescription[PINS_CAN1].ulPinType,
    g_APinDescription[PINS_CAN1].ulPin,
    g_APinDescription[PINS_CAN1].ulPinConfiguration);

  /* Initialize CAN1 Transceiver */
  SN65HVD234_Init( &gCanTransceiver1 ) ;
  SN65HVD234_SetRs( &gCanTransceiver1, PIOE, PIO_PB15 ) ;
  SN65HVD234_SetEN( &gCanTransceiver1, PIOE, PIO_PB16 ) ;
  /* Enable CAN1 Transceiver */
  SN65HVD234_DisableLowPower( &gCanTransceiver1 ) ;
  SN65HVD234_Enable( &gCanTransceiver1 ) ;
}

uint32_t CAN_DeInitShieldHardware( uint32_t )
{
}


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


/**
 *  main function
 */
extern int main( void )
{

    if ( ( CAN_Init( CAN0, BOARD_MCK, 1000, &gCanTransfer0 ) == 1 ) &&
         ( CAN_Init( CAN1, BOARD_MCK, 1000, &gCanTransfer1 ) == 1 ) )
    {
        puts( "CAN initialization complete."STRING_EOL ) ;

        /* Run tests */
        puts( "Press any key to start test"STRING_EOL ) ;
        UART_GetChar() ;
        _Test1() ;

        puts( "Press any key to continue..."STRING_EOL ) ;
        UART_GetChar() ;
        _Test2() ;

        puts( "Press any key to continue..."STRING_EOL ) ;
        UART_GetChar() ;
        _Test3() ;

        puts( "Press any key to continue..."STRING_EOL ) ;
        UART_GetChar() ;
        _Test4() ;

        /* Disable CAN0 Controller */
        CAN_Disable(CAN0) ;
        /* Disable CAN0 Transceiver */
        SN65HVD234_EnableLowPower( &gCanTransceiver0 ) ;
        SN65HVD234_Disable( &gCanTransceiver0 ) ;

        /* Disable CAN1 Controller */
        CAN_Disable(CAN1) ;
        /* Disable CAN1 Transceiver */
        SN65HVD234_EnableLowPower( &gCanTransceiver1 ) ;
        SN65HVD234_Disable( &gCanTransceiver1 ) ;

        puts( "End of all test"STRING_EOL ) ;
    }
    else
    {
        puts( "ERROR CAN initialisation (synchro)"STRING_EOL ) ;
    }

    return 0 ;
}
