#ifndef _VARIANT_
#define _VARIANT_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "libsam/chip.h"

/**
 * Libc porting layers
 */
#if defined (  __GNUC__  ) /* GCC CS3 */
#    include <syscalls.h> /** RedHat Newlib minimal stub */
#endif

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

#ifndef sam3s_ek
#define sam3s_ek
#endif

/** Name of the board */
#define VARIANT_NAME "SAM3S-EK"

/*
#define VARIANT_REV_A
*/
#define VARIANT_REV_B

/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC           12000000

/** Master clock frequency */
#define VARIANT_MCK               64000000

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#  ifdef __cplusplus
#    include "UART.h"
#    include "USART.h"

extern UARTClass Serial1 ;
extern UARTClass Serial2 ;

//extern USARTClass Serial3 ;
//extern USARTClass Serial4 ;

#  endif

#endif /* #ifndef _VARIANT_ */

