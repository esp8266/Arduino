/* %arduino_license% */

#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif

extern void pinMode( uint32_t ulPin, uint32_t ulMode )
{
	if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
    {
        return ;
    }

	switch ( ulMode )
    {
        case INPUT:
            /* Enable peripheral for clocking input */
            PMC_EnablePeripheral( g_APinDescription[ulPin].ulPeripheralId ) ;
            PIO_Configure( g_APinDescription[ulPin].pPort, PIO_INPUT, g_APinDescription[ulPin].ulPin, 0 ) ;
        break ;

        case OUTPUT:
            /* if all pins are output, disable PIO Controller clocking, reduce power consomption */
            if ( g_APinDescription[ulPin].pPort->PIO_OSR == 0xffffffff )
            {
                PMC_DisablePeripheral( g_APinDescription[ulPin].ulPeripheralId ) ;
            }
            PIO_Configure( g_APinDescription[ulPin].pPort, PIO_OUTPUT_1, g_APinDescription[ulPin].ulPin, g_APinDescription[ulPin].ulPinAttribute ) ;
        break ;

        default:
        break ;
    }
}

extern void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
  /* Handle */
	if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return ;
  }

  if ( PIO_GetOutputDataStatus( g_APinDescription[ulPin].pPort, g_APinDescription[ulPin].ulPin ) == 0 )
  {
    PIO_PullUp( g_APinDescription[ulPin].pPort, g_APinDescription[ulPin].ulPin, 1 ) ;
  }
  else
  {
    PIO_SetOutput( g_APinDescription[ulPin].pPort, g_APinDescription[ulPin].ulPin, ulVal, 0, PIO_PULLUP ) ;
  }
}

extern int digitalRead( uint32_t ulPin )
{
	if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
    {
        return LOW ;
    }

	if ( PIO_Get( g_APinDescription[ulPin].pPort, PIO_INPUT, g_APinDescription[ulPin].ulPin ) == 1 )
    {
        return HIGH ;
    }

	return LOW ;
}

#ifdef __cplusplus
}
#endif

