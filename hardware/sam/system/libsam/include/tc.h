/*
 %atmel_license%
*/

/**
 *  \file
 *
 *  \section Purpose
 *
 *  Interface for configuring and using Timer Counter (TC) peripherals.
 *
 *  \section Usage
 *  -# Optionally, use TC_FindMckDivisor() to let the program find the best
 *     TCCLKS field value automatically.
 *  -# Configure a Timer Counter in the desired mode using TC_Configure().
 *  -# Start or stop the timer clock using TC_Start() and TC_Stop().
 */

#ifndef _TC_
#define _TC_

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include "../chip.h"

#include <stdint.h>

/*------------------------------------------------------------------------------
 *         Global functions
 *------------------------------------------------------------------------------*/

#ifdef __cplusplus
 extern "C" {
#endif

extern void TC_Configure( Tc *pTc, uint32_t dwChannel, uint32_t dwMode ) ;

extern void TC_Start( Tc *pTc, uint32_t dwChannel ) ;

extern void TC_Stop( Tc *pTc, uint32_t dwChannel ) ;

extern uint32_t TC_FindMckDivisor( uint32_t dwFreq, uint32_t dwMCk, uint32_t *dwDiv, uint32_t *dwTcClks, uint32_t dwBoardMCK ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _TC_ */

