/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id$
*/

#include "wiring_private.h"

uint32_t millis( void )
{
// todo: ensure no interrupts
	return GetTickCount() ;
}

uint32_t micros( void )
{
  uint32_t dwTicks ;

  __disable_irq() ;
  dwTicks=SysTick->VAL ;
  __enable_irq() ;
 
  return (GetTickCount()*1000) + ((SysTick->LOAD + 1 - dwTicks)/(VARIANT_MCK/1000000)) ;
}

/// \brief Pauses the program for the amount of time (in miliseconds) specified as parameter. 
/// (There are 1000 milliseconds in a second.)
///
/// \param dwMs the number of milliseconds to pause (uint32_t) 
///
void delay( uint32_t dwMs )
{
	Wait( dwMs ) ;
}

/// \brief Pauses the program for the amount of time (in microseconds) specified as parameter. 
///
/// \param dwUs the number of microseconds to pause (uint32_t)
///
void delayMicroseconds( uint32_t dwUs )
{
  uint32_t dwStartMicros=micros() ;

	while ( (micros() - dwStartMicros) < dwUs )
	{
		//	do nothing
	}
}

/*
 * Cortex-M3 Systick IT handler
 */
extern void SysTick_Handler( void )
{
  // Increment tick count each ms
  TimeTick_Increment() ;
}

//! Check Variant for PLL/Clock inits
#if defined sam3s_ek
#define VARIANT_PLLAR     (CKGR_PLLAR_STUCKTO1 | \
                           CKGR_PLLAR_MULA( 0x0f ) | \
                           CKGR_PLLAR_PLLACOUNT( 0x1 ) | \
                           CKGR_PLLAR_DIVA( 0x3 ))
#else
    #error "No settings for current VARIANT"
#endif


/**
 * \brief Performs the low-level initialization of the chip.
 * This includes EFC and master clock configuration.
 * It also enable a low level on the pin NRST triggers a user reset.
 */
static void LowLevelInit_sam3s_ek( void )
{
  /* Set 3 FWS for Embedded Flash Access @ 64MHz, we are now at 4MHz on Internal FastRC */
  EFC->EEFC_FMR = EEFC_FMR_FWS( 3 ) ;

  /* Initialize main oscillator */
  if ( !(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL) )
  {
    PMC->CKGR_MOR = CKGR_MOR_KEY(0x37) | CKGR_MOR_MOSCXTST(0x8) | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN ;
    for ( ; !(PMC->PMC_SR & PMC_SR_MOSCXTS) ; ) ;
  }

  /* Switch to 3-20MHz Xtal oscillator */
  PMC->CKGR_MOR = CKGR_MOR_KEY(0x37) | CKGR_MOR_MOSCXTST(0x8) | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCSEL ;
  for ( ; !(PMC->PMC_SR & PMC_SR_MOSCSELS) ; ) ;

  PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK ;
  for ( ; !(PMC->PMC_SR & PMC_SR_MCKRDY) ; ) ;

  /* Initialize PLLA */
  PMC->CKGR_PLLAR = VARIANT_PLLAR ;
  for ( ; !(PMC->PMC_SR & PMC_SR_LOCKA) ; ) ;

  /* Switch to main clock */
  PMC->PMC_MCKR = ((PMC_MCKR_PRES_CLK | PMC_MCKR_CSS_PLLA_CLK) & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK ;
  for ( ; !(PMC->PMC_SR & PMC_SR_MCKRDY) ; ) ;

  PMC->PMC_MCKR = (PMC_MCKR_PRES_CLK | PMC_MCKR_CSS_PLLA_CLK) ;
  for ( ; !(PMC->PMC_SR & PMC_SR_MCKRDY) ; ) ;
}

void init( void )
{
  // Disable watchdog, common to all SAM variants
  WDT_Disable( WDT ) ;

#if defined sam3s_ek
  // Set Main clock to 64MHz using external 12MHz
  LowLevelInit_sam3s_ek() ;
#else
#  error "Board/Variant not defined"
#endif

  // Set Systick to 1ms interval, common to all SAM3 variants
  SysTick_Config( VARIANT_MCK/1000 ) ;

  // Initialize Serial port UART0, common to all SAM3 variants
  PIO_Configure( APinDescription[PINS_UART].pPort, APinDescription[PINS_UART].dwPinType, 
                 APinDescription[PINS_UART].dwPin, APinDescription[PINS_UART].dwPinAttribute ) ;
}
