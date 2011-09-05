/*! \file *********************************************************************
 *
 * \brief CMSIS Cortex-M# Device Peripheral Access Layer Header File
 *           for SAM3 devices.
 *
 * \par Purpose
 *
 * This file provides basic support for Cortex-M processor based 
 * microcontrollers.
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* $asf_license$ */

#ifndef __SYSTEM_SAM3U_
#define __SYSTEM_SAM3U_

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

#include <stdint.h>

extern uint32_t SystemCoreClock;     /* System Clock Frequency (Core Clock) */

/**
 * @brief Setup the microcontroller system.
 * Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
 * @brief Updates the SystemCoreClock with current core Clock 
 * retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* __SYSTEM_SAM3U_ */
