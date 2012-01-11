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

#ifndef _WIRING_INTERRUPTS_
#define _WIRING_INTERRUPTS_

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif


//typedef struct _InterruptSource
//{
//    /* Pointer to the source pin instance. */
//    PinDescription *pPin ;
//
//    /* Interrupt handler. */
//    void (*handler)( const PinDescription *pPin ) ;
//} InterruptSource ;


/*
 * \brief Specifies a function to call when an external interrupt occurs.
 * Replaces any previous function that was attached to the interrupt.
 * All Arduino SAM3 based boards pins can be switched into INPUT mode and have 
 * an interrupt user function attached to an event.
 *
 * \param ulInterrupt
 * \param pfn_UserFunc
 * \param ulMode
 *
PIO_IT_RE_OR_HL = Interrupt High Level/Rising Edge detection is active
PIO_IT_EDGE = Interrupt Edge detection is active
PIO_IT_LOW_LEVEL = Low level interrupt is active
PIO_IT_HIGH_LEVEL = High level interrupt is active
PIO_IT_FALL_EDGE = Falling edge interrupt is active
PIO_IT_RISE_EDGE = Rising edge interrupt is active

interrupt: the number of the interrupt (int)

function: the function to call when the interrupt occurs; this function must take no parameters and return nothing. This function is sometimes referred to as an interrupt service routine.

mode defines when the interrupt should be triggered. Four contstants are predefined as valid values:

    LOW to trigger the interrupt whenever the pin is low,
    CHANGE to trigger the interrupt whenever the pin changes value
    RISING to trigger when the pin goes from low to high,
    FALLING for when the pin goes from high to low. 
 */
extern void attachInterrupt( uint32_t ulInterrupt, void (*pfn_UserFunc)(void), uint32_t ulMode ) ;

/*
Turns off the given interrupt.

Parameters

interrupt: the number of interrupt to disable (0 or 1). 
*/
extern void detachInterrupt( uint32_t ulInterrupt ) ;

#ifdef __cplusplus
}
#endif

#endif /* _WIRING_INTERRUPTS_ */
