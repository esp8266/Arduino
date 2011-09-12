/*
 %atmel_license%
*/

#ifndef _PMC_
#define _PMC_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus
 extern "C" {
#endif

extern void PMC_EnablePeripheral( uint32_t dwId ) ;
extern void PMC_DisablePeripheral( uint32_t dwId ) ;

extern uint32_t PMC_IsPeripheralEnabled( uint32_t dwId ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _PMC_ */

